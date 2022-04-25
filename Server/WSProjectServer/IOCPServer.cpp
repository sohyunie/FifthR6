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
	// [TODO] �ӽ÷� DB �ݾƵ�
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

		// DB ������ �ϰ� id üũ�� �ؼ� ������ �� id�� �־��ִ°���. 
		// clientIndex�� �������� ������ ���� ��. uid
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
					//switch (evnt.EvntType)
					//{
					//case EventType::NPC_MOVE:
					//{
					//	WSAOVERLAPPEDEX* over_ex = new WSAOVERLAPPEDEX(OP::NPC_MOVE);
					//	over_ex->Target = evnt.TargetID;
					//	over_ex->Random_direction = evnt.Move_direction;
					//	server.mIOCP.PostToCompletionQueue(over_ex, evnt.ObjectID);
					//	break;
					//}
					//case EventType::NPC_REVIVE:
					//{
					//	gClients[evnt.ObjectID]->Revive();
					//	server.HandleRevivedPlayer(evnt.ObjectID);
					//	break;
					//}
					//}
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
	// ��ȿ�� �˻�
	if (gClients.count(id) == 0)
	{
		gClients[id] = std::make_shared<Session>();
	}
	gClients[id]->AssignAcceptedID(id, sck);
	mIOCP.RegisterDevice(sck, id);
	gClients[id]->RecvMsg();
}

// ������ ������� id �ο� 
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

// ���⼭ Ŭ���̾�Ʈ�� ���� ��Ŷ�� �޴� ��
void IOCPServer::ProcessPackets(int id, RingBuffer& msgQueue)
{
	while (!msgQueue.IsEmpty())
	{
		char type = msgQueue.GetMsgType();
		std::cout << "type : " + type << std::endl;

		// �ӽ÷� �����ؾ� �ϴ� �κ�.
		// C -> S CS_PACKET_LOGIN
		// S -> C SendLoginOkPacket
		// C -> S CS_PACKET_JOIN_ROOM, RoomNumber = 1�� �ӽ÷� ����.
		switch (type)
		{
		case CS_PACKET_LOGIN:
		{
			// ���ϸ��� ������ ��
			cs_packet_login packet_login{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_login), sizeof(cs_packet_login));
			SendLoginOkPacket(id);	// ���� ���ӵƴ��� ����
			// �α����� �ϴ� ���� ���� ���ٸ� �ϳ��� ��������� ������ �� ������ ��.
			break;
		}
		case CS_PACKET_JOIN_ROOM:
		{
			cs_packet_join_room packet_join_room{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_join_room), sizeof(cs_packet_join_room));
			ProcessJoinPacket(packet_join_room, id);
			break;
		}

		case CS_PACKET_READY:
		{
			break;
		}
		case CS_PACKET_MOVE:
		{
			cs_packet_move packet_move{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_move), sizeof(cs_packet_move));
			MovePosition(packet_move, id);

			std::shared_ptr<Room> room = gRooms[gClients[id]->roomID];
			for (int i = 0; i < 3; i++)
			{
				if (room->GetPlayerUidByIndex(i) != 0)
					SendMovePacket(room->GetPlayerUidByIndex(i), id);
			}	
			break;
		}
		case CS_PACKET_JUMP:
		{
			cs_packet_jump packet_jump{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_jump), sizeof(cs_packet_jump));
			Jump(gClients[id]->Info.x, gClients[id]->Info.y, gClients[id]->Info.z);

			SendMovePacket(id, id);
			break;
		}
		case CS_PACKET_GUARD:
		{
			cs_packet_guard packet_guard{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_guard), sizeof(cs_packet_guard));
			// guard �� ���� ���� ���� �ʿ� 
			SendMovePacket(id, id);
			break;
		}
		// ATTACK ���� �ʿ� 
		case CS_PACKET_MELEE_ATTACK:
		{
			cs_packet_melee_attack melee_attack_packet{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&melee_attack_packet), sizeof(cs_packet_melee_attack));

			if (gClients[id]->IsAttackTimeOut() == false) break;

			gClients[id]->ViewListLock.lock();
			auto viewlist = gClients[id]->GetViewList();
			gClients[id]->ViewListLock.unlock();

			gClients[id]->SetAttackDuration(1000ms);
			ProcessAttackPacket(id, viewlist);
			break;
		}
		case CS_PACKET_SKILL_ATTACK:
		{
			cs_packet_skill_attack skill_attack_packet{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&skill_attack_packet), sizeof(cs_packet_skill_attack));

			if (gClients[id]->IsAttackTimeOut() == false) break;

			gClients[id]->ViewListLock.lock();
			auto viewlist = gClients[id]->GetViewList();
			gClients[id]->ViewListLock.unlock();

			gClients[id]->SetAttackDuration(1000ms);
			ProcessAttackPacket(id, viewlist);
			break;
		}
		case CS_PACKET_MAP_CHANGE:
		{
			break;
		}
		/*case CS_PACKET_ROTATE:
		{
			cs_packet_rotate packet_rotate{};
			msgQueue.Pop(reinterpret_cast<uchar*>(&packet_rotate), sizeof(cs_packet_rotate));
			gClients[id]->Info.look = packet_rotate.vLook;
			gClients[id]->Info.up= packet_rotate.vUp;
			gClients[id]->Info.right = packet_rotate.vRight;

			SendRotatePacket(id);
			break;
		}*/

		default:
			std::cout << "Unkown packet\n";
			return;
		}
	}
}

// Ŭ���̾�Ʈ ���� ����
void IOCPServer::ProcessJoinPacket(cs_packet_join_room& pck, int myUid)
{
	// �켱 ���� ������ �����.
	if(gRooms.count(roomCount) == 0)
		gRooms[roomCount] = std::make_shared<Room>(0);

	int myIndex = gRooms[roomCount]->GetUidIndex(myUid);
	gClients[myUid]->roomID = 0; // �ӽ� roomid
	gClients[myUid]->Info.hp = 100;
	gClients[myUid]->Info.max_hp = 100;
	gClients[myUid]->Info.x = this->defaultPosition[myIndex][0];
	gClients[myUid]->Info.y = this->defaultPosition[myIndex][1];
	gClients[myUid]->Info.z = this->defaultPosition[myIndex][2];
	
	SendNewPlayerPosition(gRooms[0], myUid);	// �ٸ� �÷��̾����� ���� ���� �� �˷���
}

void IOCPServer::ProcessAttackPacket(int id, const std::unordered_set<int>& viewlist)
{
	
}

void IOCPServer::SendNewPlayerPosition(std::shared_ptr<Room> room, int newPlayerUid)
{
	for (int i = 0; i < 3; i++)
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

// ���� login �ߴ��� Ȯ��
void IOCPServer::SendLoginOkPacket(int id)
{
	sc_packet_login_ok ok_packet{};
	ok_packet.id = id;
	ok_packet.size = sizeof(sc_packet_login_ok);
	ok_packet.type = SC_PACKET_LOGIN_OK;
	gClients[id]->SendMsg(reinterpret_cast<char*>(&ok_packet), sizeof(ok_packet));
}

void IOCPServer::SendJoinPacket(short id)
{
	sc_packet_join_room ok_packet{};
	ok_packet.id = id;
	ok_packet.size = sizeof(sc_packet_join_room);
	ok_packet.type = SC_PACKET_JOIN_ROOM;
	// �߰� ���� �ʿ�
	// ok_packet.playerName1;
	// ok_packet.playerName2;
	// ok_packet.roomNumber;
	// ok_packet.success;
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
	put_packet.x = gClients[newPlayerID]->Info.x;
	put_packet.y = gClients[newPlayerID]->Info.y;
	put_packet.z = gClients[newPlayerID]->Info.z;
	put_packet.object_type = gClients[newPlayerID]->Info.characterType;
	*put_packet.name = *gClients[newPlayerID]->Info.name; // �ȵ� ���� ����. �ϴ� ���� �� �����...
	gClients[recieverID]->SendMsg(reinterpret_cast<char*>(&put_packet), sizeof(put_packet));
}

void IOCPServer::SendMovePacket(int sender, int target)
{
	if (sender == target) // ���� ������ ���� �ʿ� ����.
		return;
	sc_packet_move move_packet{};
	move_packet.id = target;
	move_packet.size = sizeof(sc_packet_move);
	move_packet.type = SC_PACKET_MOVE;
	move_packet.x = gClients[target]->Info.x;
	move_packet.y = gClients[target]->Info.y;
	move_packet.z = gClients[target]->Info.z;
	gClients[sender]->SendMsg(reinterpret_cast<char*>(&move_packet), sizeof(move_packet));
}

//void IOCPServer::SendRotatePacket(int ID)
//{
//	sc_packet_rotate rotate_packet{};
//	rotate_packet.type = SC_PACKET_ROTATE;
//	rotate_packet.size = sizeof(sc_packet_rotate);
//	rotate_packet.id = ID;
//	rotate_packet.look = gClients[ID]->Info.look;
//	rotate_packet.up = gClients[ID]->Info.up;
//	rotate_packet.right = gClients[ID]->Info.right;
//
//	for (int i = 0; i < MAX_USER; ++i)
//	{
//		// if (true == gClients[i].m_isconnected && gClients[ID].m_iRoomNumb == gClients[i].m_iRoomNumb)
//		gClients[ID]->SendMsg(reinterpret_cast<char*>(&rotate_packet), sizeof(rotate_packet));	//��Ŷ ����
//	}
//}

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

void IOCPServer::SendGameResultPacket(int id, char result)
{
	sc_packet_game_result result_packet{};
	result_packet.size = sizeof(sc_packet_game_result);
	result_packet.type = SC_PACKET_GAME_RESULT;
	result_packet.result = result;
	gClients[id]->SendMsg(reinterpret_cast<char*>(&result_packet), sizeof(result_packet));
}


bool IOCPServer::IsNear(int a_id, int b_id)
{
	if (ABS(gClients[a_id]->Info.x - gClients[b_id]->Info.x) > RANGE) return false;
	if (ABS(gClients[a_id]->Info.y - gClients[b_id]->Info.y) > RANGE) return false;
	if (ABS(gClients[a_id]->Info.z - gClients[b_id]->Info.z) > RANGE) return false;
	return true;
}

void IOCPServer::MovePosition(cs_packet_move packet, int id)
{
	gClients[id]->Info.x = packet.x;
	gClients[id]->Info.y = packet.y;
	gClients[id]->Info.z = packet.z;

	gClients[id]->Info.yaw = packet.yaw;
	gClients[id]->Info.pitch = packet.pitch;
	gClients[id]->Info.roll = packet.roll;
}

void IOCPServer::Jump(float x, float y, float z)
{
	// jump ���� �ʿ� 
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
	/*XMFLOAT3 position = gClients[id]->Info.position;
	return { position.x / SECTOR_WIDTH, position.y / SECTOR_HEIGHT };*/
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