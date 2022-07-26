#include "stdafx.h"
#include "MainIocp.h"
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>
#include <queue>

// static ���� �ʱ�ȭ
float MainIocp::HitPoint = 0.1f;
map<int, SOCKET> MainIocp::SessionSocket;
cCharactersInfo MainIocp::CharactersInfo;
DBConnector MainIocp::Conn;
CRITICAL_SECTION MainIocp::csPlayers;
CRITICAL_SECTION MainIocp::csMonsters;
MonsterSet MainIocp::MonstersInfo;
map<int, queue<int>> LevelMaster;

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	MainIocp* pOverlappedEvent = (MainIocp*)p;
	pOverlappedEvent->WorkerThread();
	return 0;
}

MainIocp::MainIocp()
{	
	InitializeCriticalSection(&csPlayers);

	// DB ����
	if (Conn.Connect(DB_ADDRESS, DB_ID, DB_PW, DB_SCHEMA, DB_PORT))
	{
		printf_s("[INFO] DB ���� ����\n");
	}
	else {
		printf_s("[ERROR] DB ���� ����\n");
	}

	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
	fnProcess[EPacketType::SET_CHARACTER].funcProcessPacket = SetCharacter;
	fnProcess[EPacketType::ENROLL_PLAYER].funcProcessPacket = EnrollCharacter;
	fnProcess[EPacketType::SEND_PLAYER].funcProcessPacket = SyncCharacters;
	fnProcess[EPacketType::HIT_PLAYER].funcProcessPacket = HitCharacter;
	fnProcess[EPacketType::CHAT].funcProcessPacket = BroadcastChat;
	fnProcess[EPacketType::LOGOUT_PLAYER].funcProcessPacket = LogoutCharacter;
	fnProcess[EPacketType::HIT_MONSTER].funcProcessPacket = HitMonster;
	fnProcess[EPacketType::SYNC_MONSTER].funcProcessPacket = SyncMonster;
	fnProcess[EPacketType::ACTION_SKILL].funcProcessPacket = ActionSkill;
}


MainIocp::~MainIocp()
{
	// winsock �� ����� ������
	WSACleanup();
	// �� ����� ��ü�� ����
	if (SocketInfo)
	{
		delete[] SocketInfo;
		SocketInfo = NULL;
	}

	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = NULL;
	}

	// DB ���� ����
	Conn.Close();
}

bool MainIocp::CreateWorkerThread()
{
	unsigned int threadId;
	// �ý��� ���� ������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] CPU ���� : %d\n", sysInfo.dwNumberOfProcessors);
	// ������ �۾� �������� ������ (CPU * 2) + 1
	nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	// thread handler ����
	hWorkerHandle = new HANDLE[nThreadCnt];
	// thread ����
	for (int i = 0; i < nThreadCnt; i++)
	{
		hWorkerHandle[i] = (HANDLE *)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (hWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] Worker Thread ���� ����\n");
			return false;
		}
		ResumeThread(hWorkerHandle[i]);
	}
	printf_s("[INFO] Worker Thread ����...\n");
	return true;
}

void MainIocp::Send(stSOCKETINFO * pSocket)
{
	int nResult;
	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	nResult = WSASend(
		pSocket->socket,
		&(pSocket->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSASend ���� : ", WSAGetLastError());
	}


}

void MainIocp::WorkerThread()
{
	// �Լ� ȣ�� ���� ����
	BOOL	bResult;
	int		nResult;
	// Overlapped I/O �۾����� ���۵� ������ ũ��
	DWORD	recvBytes;
	DWORD	sendBytes;
	// Completion Key�� ���� ������ ����
	stSOCKETINFO *	pCompletionKey;
	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
	stSOCKETINFO *	pSocketInfo;	
	DWORD	dwFlags = 0;
	

	while (bWorkerThread)
	{		
		/**
		 * �� �Լ��� ���� ��������� WaitingThread Queue �� �����·� ���� ��
		 * �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������
		 * ��ó���� ��
		 */
		bResult = GetQueuedCompletionStatus(hIOCP,
			&recvBytes,				// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pCompletionKey,	// completion key
			(LPOVERLAPPED *)&pSocketInfo,			// overlapped I/O ��ü
			INFINITE				// ����� �ð�
		);

		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] bResult false (%d)\n", recvBytes);
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}


		pSocketInfo->dataBuf.len = recvBytes;

		if (recvBytes == 0)
		{
			printf_s("[INFO] recvBytes == 0\n");
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		try
		{		
			// ��Ŷ ����
			int PacketType;
			// Ŭ���̾�Ʈ ���� ������ȭ
			stringstream RecvStream;

			RecvStream << pSocketInfo->dataBuf.buf;
			RecvStream >> PacketType;

			if (PacketType > 14)
				continue;
			// ��Ŷ ó��
			if (fnProcess[PacketType].funcProcessPacket != nullptr)
			{
				printf_s("[OK] PacketType : %d\n", PacketType);
				fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
			}
			else
			{
				printf_s("[ERROR] PacketType : %d\n", PacketType);
				//printf_s("[ERROR] RecvStream : %s\n", RecvStream.str());
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] What : %s\n", e.what());
		}
		
		// Ŭ���̾�Ʈ ���
		Recv(pSocketInfo);
	}
}

void MainIocp::Login(stringstream & RecvStream, stSOCKETINFO * pSocket)
{
	string Id;
	string Pw;

	RecvStream >> Id;
	RecvStream >> Pw;
	printf_s("[INFO] Socket check {%d}\n", pSocket);
	printf_s("[INFO] Login {%s}/{%s}\n", Id, Pw);

	int id = Conn.SearchAccount(Id, Pw);
	printf_s("[INFO] ID {%d}\n", id);

	stringstream SendStream;
	SendStream << EPacketType::LOGIN << endl;
	SendStream << id << endl;

	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();

	SessionSocket[id] = pSocket->socket;

	Send(pSocket);
}

void MainIocp::SetCharacter(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	int sessionID;
	int Id;
	RecvStream >> sessionID;
	RecvStream >> Id;

	cCharacter* pinfo = &CharactersInfo.players[sessionID]; // ID등록
	pinfo->SessionId = sessionID;
	pinfo->characterID = Id;

	printf_s("[INFO] SetCharacter {%d}\n", Id);


	int count = 0;
	auto iter = CharactersInfo.players.begin();
	while (iter != CharactersInfo.players.end()) {
		if (iter->second.characterID != 0) {
			count++;
		}
		++iter;
	}

	stringstream SendStream;
	SendStream << EPacketType::PLAY_GAME << endl;

	if (count == 2) {
		printf_s("[INFO] SEND PLAY GAME\n", Id);
		SendStream << true << endl;
	}
	else {
		SendStream << false << endl;
	}
	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();

	Broadcast(SendStream, 1);
}

void MainIocp::EnrollCharacter(stringstream & RecvStream, stSOCKETINFO * pSocket)
{
	cCharacter info;
	RecvStream >> info;

	printf_s("Enroll Character %d", info.SessionId);

	//printf_s("[INFO][%d] - UELevel : [%d]\n", info.SessionId, info.UELevel);

	EnterCriticalSection(&csPlayers);

	cCharacter* pinfo = &CharactersInfo.players[info.SessionId]; // ID등록, 차후에 선 처리

	// ĳ������ ��ġ�� ����						
	pinfo->SessionId = info.SessionId;
	pinfo->X = info.X;
	pinfo->Y = info.Y;
	pinfo->Z = info.Z;

	// ĳ������ ȸ������ ����
	pinfo->Yaw = info.Yaw;
	pinfo->Pitch = info.Pitch;
	pinfo->Roll = info.Roll;

	// ĳ������ �ӵ��� ����
	pinfo->VX = info.VX;
	pinfo->VY = info.VY;
	pinfo->VZ = info.VZ;

	// ĳ���� �Ӽ�
	pinfo->IsAlive = info.IsAlive;
	pinfo->HealthValue = info.HealthValue;
	pinfo->IsAttacking = info.IsAttacking;
	//pinfo->UELevel = info.UELevel;

	//LevelMaster[info.UELevel].push(info.SessionId);

	//pinfo->IsMaster = LevelMaster[info.UELevel].front() == info.SessionId;
	//printf_s("[Check Master][%d] - UELevel : [%d], IsMaster : [%s]\n", info.SessionId, info.UELevel, pinfo->IsMaster ? "true" : "false");

	SessionSocket[info.SessionId] = pSocket->socket;

	//Send(pSocket);
	BroadcastNewPlayer(CharactersInfo, info.UELevel);
	LeaveCriticalSection(&csPlayers);
}

void MainIocp::SyncCharacters(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	cCharacter info;
	RecvStream >> info;

	EnterCriticalSection(&csPlayers);

	if (CharactersInfo.players.count(info.SessionId) == 0)
		return;
	cCharacter * pinfo = &CharactersInfo.players[info.SessionId];
	//printf_s("[INFO] (%d) isMaster %s \n", info.SessionId, pinfo->IsMaster ? "true" : "false");

	// ĳ������ ��ġ�� ����						
	pinfo->SessionId = info.SessionId;
	pinfo->X = info.X;
	pinfo->Y = info.Y;
	pinfo->Z = info.Z;

	// ĳ������ ȸ������ ����
	pinfo->Yaw = info.Yaw;
	pinfo->Pitch = info.Pitch;
	pinfo->Roll = info.Roll;

	// ĳ������ �ӵ��� ����
	pinfo->VX = info.VX;
	pinfo->VY = info.VY;
	pinfo->VZ = info.VZ;

	pinfo->IsAttacking = info.IsAttacking;
	if (pinfo->IsAttacking == true)
	{
		cout << pinfo->SessionId << " Attack" << endl;
	}

	// Level이 변경하는 상황 발생.
	if (pinfo->UELevel != info.UELevel)
	{
		LevelMaster[pinfo->UELevel].pop(); // 마스터면 자신은 Queue에서 빠짐
		LevelMaster[info.UELevel].push(info.SessionId);

		pinfo->IsMaster = LevelMaster[info.UELevel].front() == info.SessionId;
		printf_s("[Change Level][%d] - UELevel : [%d], IsMaster : [%s]\n", info.SessionId, info.UELevel, pinfo->IsMaster ? "true" : "false");
	}
	pinfo->UELevel = info.UELevel;
	pinfo->IsMaster = LevelMaster[info.UELevel].front() == info.SessionId;

	//printf_s("[Change Level][%d] - UELevel : [%d], IsMaster : [%s]\n", info.SessionId, info.UELevel, pinfo->IsMaster ? "true" : "false");
	stringstream SendStream;
	// ����ȭ	
	SendStream << EPacketType::RECV_PLAYER << endl;
	SendStream << CharactersInfo << endl;

	OtherBroadcast(SendStream, pinfo->UELevel, pinfo->SessionId);
	pinfo->IsAttacking = false;

	LeaveCriticalSection(&csPlayers);

	//WriteCharactersInfoToSocket(pSocket);
	//Send(pSocket);
}

void MainIocp::OtherBroadcast(stringstream& SendStream, int UELevel, int sessionID)
{
	stSOCKETINFO* client = new stSOCKETINFO;
	for (const auto& kvp : SessionSocket)
	{
		//if (CharactersInfo.players[kvp.first].UELevel == UELevel) {
		//cout << kvp.first << endl;
		if (CharactersInfo.players[kvp.first].SessionId == sessionID)
			continue;
		if (CharactersInfo.players[kvp.first].IsAttacking)
			cout << CharactersInfo.players[kvp.first].SessionId << " by " << sessionID << " Attack" << endl;
		client->socket = kvp.second;
		CopyMemory(client->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
		client->dataBuf.buf = client->messageBuffer;
		client->dataBuf.len = SendStream.str().length();

		//printf_s("[INFO][%d] OtherBroadcast - %s _ %f\n", CharactersInfo.players[kvp.first].SessionId, (CharactersInfo.players[kvp.first].IsAlive) ? "true" : "false", CharactersInfo.players[kvp.first].Z);
		Send(client);
		//}
	}
	delete(client);
}
void MainIocp::LogoutCharacter(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	int SessionId;
	RecvStream >> SessionId;
	printf_s("[INFO] (%d) Log out\n", SessionId);
	EnterCriticalSection(&csPlayers);
	CharactersInfo.players[SessionId].IsAlive = false;
	if (CharactersInfo.players[SessionId].IsMaster)
	{
		LevelMaster.erase(CharactersInfo.players[SessionId].UELevel);
	}

	SessionSocket.erase(SessionId);
	printf_s("[INFO] Size : %d\n", SessionSocket.size());
	WriteCharactersInfoToSocket(pSocket);
	LeaveCriticalSection(&csPlayers);
}

void MainIocp::HitCharacter(stringstream & RecvStream, stSOCKETINFO * pSocket)
{
	// �ǰ� ó���� ���� ���̵�
	int DamagedSessionId;
	RecvStream >> DamagedSessionId;
	//printf_s("[INFO] %d ������ ���� \n", DamagedSessionId);
	EnterCriticalSection(&csPlayers);
	if (CharactersInfo.players.count(DamagedSessionId) == 0)
		return;
	CharactersInfo.players[DamagedSessionId].HealthValue -= HitPoint;
	if (CharactersInfo.players[DamagedSessionId].HealthValue < 0)
	{
		printf_s("[INFO] HealthValue : %f \n", CharactersInfo.players[DamagedSessionId].HealthValue);
		// ĳ���� ���ó��
		CharactersInfo.players[DamagedSessionId].IsAlive = false;
	}

	WriteCharactersInfoToSocket(pSocket);
	Send(pSocket);
	LeaveCriticalSection(&csPlayers);
}

void MainIocp::BroadcastChat(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	//stSOCKETINFO* client = new stSOCKETINFO;

	//int sessionID;
	//string Temp;
	//string Chat;

	//RecvStream >> sessionID;
	//getline(RecvStream, Temp);
	//Chat += to_string(sessionID) + "_:_";
	//while (RecvStream >> Temp)
	//{
	//	Chat += Temp + "_";
	//}
	//Chat += '\0';

	//printf_s("[CHAT] %s\n", Chat);

	//stringstream SendStream;
	//SendStream << EPacketType::CHAT << endl;
	//SendStream << Chat;
	//
	//Broadcast(SendStream, CharactersInfo.players[sessionID].UELevel);
}

void MainIocp::BroadcastNewPlayer(cCharactersInfo & player, int UELevel)
{
	stringstream SendStream;
	SendStream << EPacketType::ENTER_NEW_PLAYER << endl;
	SendStream << player << endl;

	Broadcast(SendStream, UELevel);
}

void MainIocp::Broadcast(stringstream & SendStream, int UELevel)
{
	stSOCKETINFO* client = new stSOCKETINFO;
	for (const auto& kvp : SessionSocket)
	{
		//cout << kvp.first << endl;
		//if (CharactersInfo.players[kvp.first].UELevel == UELevel) {
			client->socket = kvp.second;
			CopyMemory(client->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = SendStream.str().length();

			Send(client);
		//}
	}
	delete(client);
}

void MainIocp::WriteCharactersInfoToSocket(stSOCKETINFO * pSocket)
{
	stringstream SendStream;

	// ����ȭ	
	SendStream << EPacketType::RECV_PLAYER << endl;
	SendStream << CharactersInfo << endl;

	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();
}

void MainIocp::HitMonster(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	// ���� �ǰ� ó��
	int MonsterId;
	RecvStream >> MonsterId;


	InitializeCriticalSection(&csMonsters);
	MonstersInfo.monsters[MonsterId].Id = MonsterId;
	if (!MonstersInfo.monsters[MonsterId].IsAlive())
	{
		printf_s("[INFO] (%d) DESTROY_MONSTER \n", MonsterId);
		stringstream SendStream;
		SendStream << EPacketType::DESTROY_MONSTER << endl;
		SendStream << MonstersInfo.monsters[MonsterId] << endl;

		Broadcast(SendStream, MonstersInfo.monsters[MonsterId].UELevel);

		MonstersInfo.monsters.erase(MonsterId);
	}
	else
	{
		printf_s("[INFO] (%d) HIT_MONSTER \n", MonsterId);
		stringstream SendStream;
		SendStream << EPacketType::DESTROY_MONSTER << endl;
		SendStream << MonstersInfo.monsters[MonsterId] << endl;

		Broadcast(SendStream, MonstersInfo.monsters[MonsterId].UELevel);
	}
	LeaveCriticalSection(&csMonsters);
}

void MainIocp::SyncMonster(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	InitializeCriticalSection(&csMonsters);
	// Recv
	MonsterSet monsterSet;
	RecvStream >> monsterSet;
	MonstersInfo = monsterSet;

	// Send
	stringstream SendStream;
	SendStream << EPacketType::SYNC_MONSTER << endl;
	SendStream << MonstersInfo << endl;


	LeaveCriticalSection(&csMonsters);
	Broadcast(SendStream, 1);
	//printf_s("[INFO]SyncMonster %d \n", monsterSet.monsters[0].ueLevel);
}

void MainIocp::ActionSkill(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	int sessionID;
	int id;
	RecvStream >> sessionID;
	RecvStream >> id;
	stringstream SendStream;
	SendStream << EPacketType::ACTION_SKILL << endl;
	SendStream << sessionID << endl;
	SendStream << id << endl;

	printf_s("[INFO]Action Skill");

	OtherBroadcast(SendStream, 1, sessionID);
}