#include "common.h"
#include "IOCPServer.h"

//std::array<std::shared_ptr<Session>, MAX_USER + MAX_NPC> IOCPServer::gClients;
std::unordered_map<int, std::shared_ptr<Session>> IOCPServer::gClients;
std::unordered_map<int, std::shared_ptr<Room>> IOCPServer::gRooms;
std::array<std::array<std::unordered_set<int>, SECTOR_WIDTH>, SECTOR_HEIGHT> IOCPServer::gSectors;
concurrency::concurrent_priority_queue<TimerEvent> IOCPServer::gTimerQueue;

IOCPServer::IOCPServer(const EndPoint& ep)
	: mLoop(true)
{
	// [TODO] 임시로 DB 닫아둠
	//if (mDBHandler.ConnectToDB(L"sql_server") == false)
	//	std::cout << "failed to connect to DB\n";

	//for (int i = 0; i < gClients.size(); i++) {
	//	gClients[i] = std::make_shared<Session>();
	//	gClients[i]->ID = i;
	//}

	mListenSck.Init();
	mListenSck.Bind(ep);
}

IOCPServer::~IOCPServer()
{
}

void IOCPServer::Run()
{
	mListenSck.Listen();
	mIOCP.RegisterDevice(mListenSck.mSocket, 0);
	std::cout << "Listening to clients...\n";

	WSAOVERLAPPEDEX acceptEx;
	mListenSck.AsyncAccept(acceptEx);

	for (int i = 0; i < MaxThreads; i++)
		mThreads.emplace_back(NetworkThreadFunc, std::ref(*this));
	mTimerThread = std::thread{ TimerThreadFunc, std::ref(*this) };

	for (std::thread& thrd : mThreads)
		thrd.join();
	mTimerThread.join();
}

void IOCPServer::NetworkThreadFunc(IOCPServer& server)
{
	try {
		while (server.mLoop)
		{
			CompletionInfo info = server.mIOCP.GetCompletionInfo();

			int client_id = static_cast<int>(info.key);
			WSAOVERLAPPEDEX* over_ex = reinterpret_cast<WSAOVERLAPPEDEX*>(info.overEx);

			if (info.success == FALSE)
			{
				server.Disconnect(client_id);
				if (over_ex->Operation == OP::SEND)
					delete over_ex;
				continue;
			}

			server.HandleCompletionInfoByOperation(over_ex, client_id, info.bytes);
		}
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void IOCPServer::HandleCompletionInfoByOperation(WSAOVERLAPPEDEX* over, int id, int bytes)
{
	switch (over->Operation)
	{
	case OP::RECV:
	{
		if (bytes == 0)
		{
			Disconnect(id);
			break;
		}
		Session* client = gClients[id].get();
		over->MsgQueue.Push(over->NetBuffer, bytes);
		ProcessPackets(id, over->MsgQueue);
		client->RecvMsg();
		break;
	}
	case OP::SEND:
	{
		if (bytes != over->WSABuffer.len)
			Disconnect(id);
		delete over;
		break;
	}
	case OP::ACCEPT:
	{
		SOCKET clientSck = *reinterpret_cast<SOCKET*>(over->NetBuffer);

		// DB 연결을 하고 id 체크를 해서 있으면 그 id를 넣어주는거지. 
		// clientIndex는 유저마다 고유한 정수 값. uid
		//int uid = GetAvailableID();
		int uid = clientCount++;

		if (uid == -1) {
			std::cout << "Max number of clients overflow\n";
			SendLoginFailPacket(uid, 1);
		}
		else
			AcceptNewClient(uid, clientSck);
		mListenSck.AsyncAccept(*over);
		break;
	}
	case OP::PLAYER_MOVE:
	{
		delete over;
		break;
	}
	}
}

void IOCPServer::HandleRevivedPlayer(int id)
{
	for (int i = 0; i < NPC_ID_START; i++)
	{
		if (IsNear(id, gClients[i]->ID) == false)
			continue;
		if (!gClients[i]->IsStateWithoutLock(State::INGAME))
			continue;
		gClients[i]->InsertViewID(id);
		SendPutObjectPacket(i, id);
	}
}

void IOCPServer::TimerThreadFunc(IOCPServer& server)
{
	try {
		while (server.mLoop)
		{
			while (gTimerQueue.empty() == false)
			{
				TimerEvent evnt;
				gTimerQueue.try_pop(evnt);
				if (evnt.StartTime <= std::chrono::system_clock::now())
				{
					switch (evnt.EvntType)
					{
					case EventType::NPC_MOVE:
					{
						WSAOVERLAPPEDEX* over_ex = new WSAOVERLAPPEDEX(OP::NPC_MOVE);
						over_ex->Target = evnt.TargetID;
						over_ex->Random_direction = evnt.Move_direction;
						server.mIOCP.PostToCompletionQueue(over_ex, evnt.ObjectID);
						break;
					}
					case EventType::NPC_REVIVE:
					{
						gClients[evnt.ObjectID]->Revive();
						server.HandleRevivedPlayer(evnt.ObjectID);
						break;
					}
					}
				}
				else
				{
					gTimerQueue.push(evnt);
					break;
				}
			}
			std::this_thread::sleep_for(10ms);
		}
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void IOCPServer::Disconnect(int id)
{
	std::cout << "Disconnect [" << id << "]\n";
	
	gClients[id]->ViewListLock.lock();
	auto viewlist = gClients[id]->GetViewList();
	gClients[id]->ViewListLock.unlock();

	for (int pid : viewlist)
	{
		if (!gClients[pid]->IsStateWithoutLock(State::INGAME))
			continue;

		if (gClients[pid]->FindAndEraseViewID(id))
			SendRemovePacket(pid, id);
	}
	if(strncmp(gClients[id]->Info.name, "GM", 2)!=0)
		mDBHandler.DisconnectAndUpdate(gClients[id]->Info);
	gClients[id]->Disconnect();
}

void IOCPServer::AcceptNewClient(int id, SOCKET sck)
{
	// 유효성 검사
	if (gClients.count(id) == 0)
	{
		gClients[id] = std::make_shared<Session>();
	}
	gClients[id]->AssignAcceptedID(id, sck);
	mIOCP.RegisterDevice(sck, id);
	gClients[id]->RecvMsg();
}

// 들어오는 순서대로 id 부여 
int IOCPServer::GetAvailableID()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (gClients[i]->CompareAndChangeState(State::FREE, State::ACCEPT))
			return i;
	}
	std::cout << "Maximum Number of Clients Overflow!!\n";
	return -1;
}

// 여기서 클라이언트가 보낸 패킷을 받는 곳
void IOCPServer::ProcessPackets(int id, RingBuffer& msgQueue)
{
	while (!msgQueue.IsEmpty())
	{
		char type = msgQueue.GetMsgType();
		std::cout << "type : " + type << std::endl;

		switch (type)
		{
		case CS_PACKET_LOGIN:
		{
			// 제일먼저 들어오는 곳
			cs_packet_login packet_login{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_login), sizeof(cs_packet_login));
			SendLoginOkPacket(id);	// 내가 접속됐는지 보냄
			break;
		}
		case CS_PACKET_GAME_JOIN:
		{
			cs_packet_game_join packet_game_join{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_game_join), sizeof(cs_packet_game_join));
			ProcessJoinPacket(packet_game_join, id);
			break;
		}
		case CS_PACKET_TELEPORT:
		{
			break;
		}
		case CS_PACKET_MOVE:
		{
			cs_packet_move packet_move{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_move), sizeof(cs_packet_move));
			MovePosition(gClients[id]->Info.position, packet_move.key);

			SendMovePacket(id, id);			
			break;
		}
		case CS_PACKET_ROTATE:
		{
			cs_packet_rotate packet_rotate{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_rotate), sizeof(cs_packet_rotate));
			gClients[id]->Info.look = packet_rotate.vLook;
			gClients[id]->Info.up= packet_rotate.vUp;
			gClients[id]->Info.right = packet_rotate.vRight;

			SendRotatePacket(id);
			break;
		}
		case CS_PACKET_CHAT:
		{
			cs_packet_chat chat_packet{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&chat_packet), sizeof(cs_packet_chat));

			gClients[id]->ViewListLock.lock();
			auto viewlist = gClients[id]->GetViewList();
			gClients[id]->ViewListLock.unlock();

			break;
		}
		case CS_PACKET_ATTACK:
		{
			cs_packet_attack attack_packet{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&attack_packet), sizeof(cs_packet_attack));
			
			if (gClients[id]->IsAttackTimeOut() == false) break;

			gClients[id]->ViewListLock.lock();
			auto viewlist = gClients[id]->GetViewList();
			gClients[id]->ViewListLock.unlock();

			gClients[id]->SetAttackDuration(1000ms);
			ProcessAttackPacket(id, viewlist);
			break;
		}
		default:
			std::cout << "Unkown packet\n";
			return;
		}
	}
}

// 클라이언트 정보 설정
void IOCPServer::ProcessJoinPacket(cs_packet_game_join& pck, int myUid)
{
	if(gRooms.count(roomCount) == 0)
		gRooms[roomCount] = std::make_shared<Room>(myUid);

	int roomIndex = gRooms[roomCount]->GetUidIndex(myUid);
	gClients[myUid]->Info.hp = 100;
	gClients[myUid]->Info.max_hp = 100;
	gClients[myUid]->Info.position = this->defaultPosition[roomIndex];
	
	SendNewPlayerPosition(gRooms[roomCount], myUid);	// 다른 플레이어한테 내가 들어온 걸 알려줌
}

void IOCPServer::ProcessAttackPacket(int id, const std::unordered_set<int>& viewlist)
{
	
}

void IOCPServer::SendNewPlayerPosition(std::shared_ptr<Room> room, int newPlayerUid)
{
	for (int i = 0; i < 4; i++)
	{
		if(room->GetPlayerUidByIndex(i) != 0)
			SendPutObjectPacket(room->GetPlayerUidByIndex(i), newPlayerUid);
	}
}

void IOCPServer::HandlePlayersInSight(
	const std::unordered_set<int>& sights,
	const std::unordered_set<int>& viewlist, int myId)
{	
	for (int pid : sights)
	{
		// Handle players not in my view list.
		if (viewlist.find(pid) == viewlist.end())
		{
			gClients[myId]->InsertViewID(pid);
			SendPutObjectPacket(myId, pid);
		}


		if (gClients[pid]->FindAndInsertViewID(myId))
			SendPutObjectPacket(pid, myId);
		else
			SendMovePacket(pid, myId);
	}
}

void IOCPServer::HandleDisappearedPlayers(
	const std::unordered_set<int>& sights, 
	const std::unordered_set<int>& viewlist, int myId)
{
	for (int pid : viewlist)
	{
		if (sights.find(pid) == sights.end())
		{
			gClients[myId]->EraseViewID(pid);
			SendRemovePacket(myId, pid);


			if (gClients[pid]->FindAndEraseViewID(myId))
				SendRemovePacket(pid, myId);
		}
	}
}

// 내가 login 했는지 확인
void IOCPServer::SendLoginOkPacket(int id)
{
	sc_packet_login_ok ok_packet{};
	ok_packet.id = id;
	ok_packet.size = sizeof(sc_packet_login_ok);
	ok_packet.type = SC_PACKET_LOGIN_OK;
	gClients[id]->SendMsg(reinterpret_cast<char*>(&ok_packet), sizeof(ok_packet));
}

void IOCPServer::SendJoinPacket(int id)
{
	sc_packet_game_join ok_packet{};
	ok_packet.id = id;
	ok_packet.size = sizeof(sc_packet_game_join);
	ok_packet.type = SC_PACKET_GAME_JOIN;
	ok_packet.position = gClients[id]->Info.position;
	ok_packet.hp = gClients[id]->Info.hp;
	ok_packet.maxHp = gClients[id]->Info.max_hp;
	gClients[id]->SendMsg(reinterpret_cast<char*>(&ok_packet), sizeof(ok_packet));
}

void IOCPServer::SendLoginFailPacket(int id, char reason)
{
	sc_packet_login_fail fail_packet{};
	fail_packet.size = sizeof(sc_packet_login_fail);
	fail_packet.type = SC_PACKET_LOGIN_FAIL;
	fail_packet.reason = reason;
	gClients[id]->SendMsg(reinterpret_cast<char*>(&fail_packet), sizeof(fail_packet));
}

void IOCPServer::SendPutObjectPacket(int recieverID, int newPlayerID)
{
	sc_packet_put_object put_packet{};
	put_packet.id = newPlayerID;
	put_packet.size = sizeof(sc_packet_put_object);
	put_packet.type = SC_PACKET_PUT_OBJECT;
	put_packet.position = gClients[newPlayerID]->Info.position;
	gClients[recieverID]->SendMsg(reinterpret_cast<char*>(&put_packet), sizeof(put_packet));
}

void IOCPServer::SendMovePacket(int sender, int target)
{
	sc_packet_move move_packet{};
	move_packet.id = target;
	move_packet.size = sizeof(sc_packet_move);
	move_packet.type = SC_PACKET_MOVE;
	move_packet.position = gClients[target]->Info.position;
	gClients[sender]->SendMsg(reinterpret_cast<char*>(&move_packet), sizeof(move_packet));
}

void IOCPServer::SendRotatePacket(int ID)
{
	sc_packet_rotate rotate_packet{};
	rotate_packet.type = SC_PACKET_ROTATE;
	rotate_packet.size = sizeof(sc_packet_rotate);
	rotate_packet.id = ID;
	rotate_packet.look = gClients[ID]->Info.look;
	rotate_packet.up = gClients[ID]->Info.up;
	rotate_packet.right = gClients[ID]->Info.right;

	for (int i = 0; i < MAX_USER; ++i)
	{
		// if (true == gClients[i].m_isconnected && gClients[ID].m_iRoomNumb == gClients[i].m_iRoomNumb)
		gClients[ID]->SendMsg(reinterpret_cast<char*>(&rotate_packet), sizeof(rotate_packet));	//패킷 전송
	}
}

void IOCPServer::SendRemovePacket(int sender, int target)
{
	sc_packet_remove_object remove_packet{};
	remove_packet.id = target;
	remove_packet.size = sizeof(sc_packet_remove_object);
	remove_packet.type = SC_PACKET_REMOVE_OBJECT;
	gClients[sender]->SendMsg(reinterpret_cast<char*>(&remove_packet), sizeof(remove_packet));
}

void IOCPServer::SendStatusChangePacket(int sender)
{
	sc_packet_status_change status_packet{};
	status_packet.size = sizeof(sc_packet_status_change);
	status_packet.type = SC_PACKET_STATUS_CHANGE;
	status_packet.hp = gClients[sender]->Info.hp;
	status_packet.maxhp = gClients[sender]->Info.max_hp;
	gClients[sender]->SendMsg(reinterpret_cast<char*>(&status_packet), sizeof(status_packet));
}

void IOCPServer::SendBattleResultPacket(int sender, int target, int val, char type)
{
	sc_packet_battle_result result_packet{};
	result_packet.size = sizeof(sc_packet_battle_result);
	result_packet.type = SC_PACKET_BATTLE_RESULT;
	result_packet.result_type = type;
	result_packet.target = target;
	result_packet.value = val;
	gClients[sender]->SendMsg(reinterpret_cast<char*>(&result_packet), sizeof(result_packet));
}


bool IOCPServer::IsNear(int a_id, int b_id)
{
	if (ABS(gClients[a_id]->Info.position.x - gClients[b_id]->Info.position.x) > RANGE) return false;
	if (ABS(gClients[a_id]->Info.position.y - gClients[b_id]->Info.position.y) > RANGE) return false;
	return true;
}

void IOCPServer::MovePosition(XMFLOAT3& position, char direction)
{
	switch (direction)
	{
	case 0: if (position.z > -WORLD_HEIGHT / 2 + 1) position.z -= 0.5f; break;
	case 1: if (position.z < WORLD_HEIGHT/2 - 1) position.z += 0.5f; break;
	case 2: if (position.x > -WORLD_WIDTH / 2 + 1) position.x-= 0.5f; break;
	case 3: if (position.x < WORLD_WIDTH/2 - 1) position.x += 0.5f; break;
	}
}

void IOCPServer::AddTimer(int obj_id, int player_id, EventType type, int direction, int duration)
{
	TimerEvent ev{};
	ev.ObjectID = obj_id;
	ev.StartTime = std::chrono::system_clock::now() + std::chrono::milliseconds(duration);
	ev.EvntType = type;
	ev.TargetID = player_id;
	ev.Move_direction = direction;
	gTimerQueue.push(ev);
}

void IOCPServer::ActivatePlayerMoveEvent(int target, int player)
{
	WSAOVERLAPPEDEX* over_ex = new WSAOVERLAPPEDEX(OP::PLAYER_MOVE);
	over_ex->Target = player;
	mIOCP.PostToCompletionQueue(over_ex, target);
}

std::pair<short, short> IOCPServer::GetSectorIndex(int id)
{
	XMFLOAT3 position = gClients[id]->Info.position;
	return { position.x / SECTOR_WIDTH, position.y / SECTOR_HEIGHT };
}

void IOCPServer::InsertIntoSectorWithoutLock(int id)
{
	auto secIdx = GetSectorIndex(id);
	gSectors[secIdx.first][secIdx.second].insert(id);
}

void IOCPServer::InsertIntoSectorWithLock(int id)
{
	auto sector = GetSectorIndex(id);

	mSectorLock.lock();
	gSectors[sector.first][sector.second].insert(id);
	mSectorLock.unlock();
}

void IOCPServer::EraseFromSectorWidthLock(int id)
{
	auto sector = GetSectorIndex(id);

	mSectorLock.lock();
	gSectors[sector.first][sector.second].erase(id);
	mSectorLock.unlock();
}