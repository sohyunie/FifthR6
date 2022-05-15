#include "stdafx.h"
#include "MainIocp.h"
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

// static ���� �ʱ�ȭ
float MainIocp::HitPoint = 0.1f;
map<int, SOCKET> MainIocp::SessionSocket;
cCharactersInfo MainIocp::CharactersInfo;
//DBConnector MainIocp::Conn;
CRITICAL_SECTION MainIocp::csPlayers;
CRITICAL_SECTION MainIocp::csMonsters;
MonsterSet MainIocp::MonstersInfo;
map<int, int> LevelMaster;

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
	//if (Conn.Connect(DB_ADDRESS, DB_ID, DB_PW, DB_SCHEMA, DB_PORT))
	//{
	//	printf_s("[INFO] DB ���� ����\n");
	//}
	//else {
	//	printf_s("[ERROR] DB ���� ����\n");
	//}

	// ��Ŷ �Լ� �����Ϳ� �Լ� ����
	fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
	fnProcess[EPacketType::ENROLL_PLAYER].funcProcessPacket = EnrollCharacter;
	fnProcess[EPacketType::SEND_PLAYER].funcProcessPacket = SyncCharacters;
	fnProcess[EPacketType::HIT_PLAYER].funcProcessPacket = HitCharacter;
	fnProcess[EPacketType::CHAT].funcProcessPacket = BroadcastChat;
	fnProcess[EPacketType::LOGOUT_PLAYER].funcProcessPacket = LogoutCharacter;
	fnProcess[EPacketType::HIT_MONSTER].funcProcessPacket = HitMonster;
	fnProcess[EPacketType::SYNC_MONSTER].funcProcessPacket = SyncMonster;
	fnProcess[EPacketType::SYNC_CUBE].funcProcessPacket = SyncCube;
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
	//Conn.Close();
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
			printf_s("[INFO] socket(%d) ���� ����\n", pSocketInfo->socket);
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}


		pSocketInfo->dataBuf.len = recvBytes;

		if (recvBytes == 0)
		{
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

			// ��Ŷ ó��
			if (fnProcess[PacketType].funcProcessPacket != nullptr)
			{
				fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
			}
			else
			{
				printf_s("[ERROR] ���� ���� ���� ��Ŷ : %d\n", PacketType);
			}
		}
		catch (const std::exception& e)
		{
			printf_s("[ERROR] �� �� ���� ���� �߻� : %s\n", e.what());
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

	printf_s("[INFO] �α��� �õ� {%s}/{%s}\n", Id, Pw);

	stringstream SendStream;
	SendStream << EPacketType::LOGIN << endl;
	//SendStream << Conn.SearchAccount(Id, Pw) << endl;

	CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->dataBuf.len = SendStream.str().length();

	Send(pSocket);
}

void MainIocp::EnrollCharacter(stringstream & RecvStream, stSOCKETINFO * pSocket)
{
	cCharacter info;
	RecvStream >> info;

	printf_s(RecvStream.str().c_str());

	printf_s("[INFO][%d]ĳ���� ��� - X : [%f], Y : [%f], Z : [%f], Yaw : [%f], Alive : [%d], Health : [%f], UELevel : [%d]\n",
		info.SessionId, info.X, info.Y, info.Z, info.Yaw, info.IsAlive, info.HealthValue, info.UELevel);

	EnterCriticalSection(&csPlayers);

	cCharacter* pinfo = &CharactersInfo.players[info.SessionId];

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

	if (LevelMaster.find(info.UELevel) == LevelMaster.end())
	{
		printf_s("���� �����;� : %d\n", info.SessionId);

		LevelMaster[info.UELevel] = info.SessionId;
		pinfo->IsMaster = true;
	}
	else {
		if (LevelMaster[info.UELevel] == pinfo->SessionId)
		{
			pinfo->IsMaster = true;
		}
		else
		{
			pinfo->IsMaster = false;
		}
	}


	SessionSocket[info.SessionId] = pSocket->socket;

	printf_s("[INFO] Ŭ���̾�Ʈ �� : %d\n", SessionSocket.size());

	//Send(pSocket);
	BroadcastNewPlayer(CharactersInfo, info.UELevel);
	LeaveCriticalSection(&csPlayers);
}

void MainIocp::SyncCharacters(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	cCharacter info;
	RecvStream >> info;

	EnterCriticalSection(&csPlayers);

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
	if (pinfo->IsAttacking == true) {
		cout << pinfo->SessionId << " Attack" << endl;
	}
	pinfo->UELevel = info.UELevel;
	if (LevelMaster[info.UELevel] == pinfo->SessionId)
	{
		pinfo->IsMaster = true;
	}
	else
	{
		pinfo->IsMaster = false;
	}


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
void MainIocp::OtherBroadcast(stringstream& SendStream, int ueLevel, int sessionID)
{
	stSOCKETINFO* client = new stSOCKETINFO;
	for (const auto& kvp : SessionSocket)
	{
		if (CharactersInfo.players[kvp.first].UELevel == ueLevel) {
			if (CharactersInfo.players[kvp.first].SessionId == sessionID)
				continue;
			if(CharactersInfo.players[sessionID].IsAttacking)
				cout << CharactersInfo.players[kvp.first].SessionId << " by " << sessionID << " Attack" << endl;
			client->socket = kvp.second;
			CopyMemory(client->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = SendStream.str().length();

			printf_s("[INFO][%d] OtherBroadcast - %s _ %f\n", CharactersInfo.players[kvp.first].SessionId, (CharactersInfo.players[kvp.first].IsAlive) ? "true" : "false", CharactersInfo.players[kvp.first].Z);
			Send(client);
		}
	}
}
void MainIocp::LogoutCharacter(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	int SessionId;
	RecvStream >> SessionId;
	printf_s("[INFO] (%d)�α׾ƿ� ��û ����\n", SessionId);
	EnterCriticalSection(&csPlayers);
	CharactersInfo.players[SessionId].IsAlive = false;
	if (CharactersInfo.players[SessionId].IsMaster)
	{
		LevelMaster.erase(CharactersInfo.players[SessionId].UELevel);
	}

	SessionSocket.erase(SessionId);
	printf_s("[INFO] Ŭ���̾�Ʈ �� : %d\n", SessionSocket.size());
	WriteCharactersInfoToSocket(pSocket);
	LeaveCriticalSection(&csPlayers);
}

void MainIocp::HitCharacter(stringstream & RecvStream, stSOCKETINFO * pSocket)
{
	// �ǰ� ó���� ���� ���̵�
	int DamagedSessionId;
	RecvStream >> DamagedSessionId;
	printf_s("[INFO] %d ������ ���� \n", DamagedSessionId);
	EnterCriticalSection(&csPlayers);
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
	stSOCKETINFO* client = new stSOCKETINFO;

	int sessionID;
	string Temp;
	string Chat;

	RecvStream >> sessionID;
	getline(RecvStream, Temp);
	Chat += to_string(sessionID) + "_:_";
	while (RecvStream >> Temp)
	{
		Chat += Temp + "_";
	}
	Chat += '\0';

	printf_s("[CHAT] %s\n", Chat);

	stringstream SendStream;
	SendStream << EPacketType::CHAT << endl;
	SendStream << Chat;
	
	Broadcast(SendStream, CharactersInfo.players[sessionID].UELevel);
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
		if (CharactersInfo.players[kvp.first].UELevel == UELevel) {
			client->socket = kvp.second;
			CopyMemory(client->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
			client->dataBuf.buf = client->messageBuffer;
			client->dataBuf.len = SendStream.str().length();

			Send(client);
		}
	}
}

void MainIocp::WriteCharactersInfoToSocket(stSOCKETINFO * pSocket)
{
	stringstream SendStream;

	// ����ȭ	
	SendStream << EPacketType::RECV_PLAYER << endl;
	SendStream << CharactersInfo << endl;

	// !!! �߿� !!! data.buf ���� ���� �����͸� ���� �����Ⱚ�� ���޵� �� ����
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
	if (!MonstersInfo.monsters[MonsterId].IsAlive())
	{
		printf_s("[INFO] (%d) DESTROY_MONSTER \n", MonsterId);
		stringstream SendStream;
		SendStream << EPacketType::DESTROY_MONSTER << endl;
		SendStream << MonstersInfo.monsters[MonsterId] << endl;

		Broadcast(SendStream, MonstersInfo.monsters[MonsterId].ueLevel);

		MonstersInfo.monsters.erase(MonsterId);
	}
	else
	{
		printf_s("[INFO] (%d) HIT_MONSTER \n", MonsterId);
		stringstream SendStream;
		SendStream << EPacketType::DESTROY_MONSTER << endl;
		SendStream << MonstersInfo.monsters[MonsterId] << endl;

		Broadcast(SendStream, MonstersInfo.monsters[MonsterId].ueLevel);
	}
	LeaveCriticalSection(&csMonsters);
}

void MainIocp::SyncMonster(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	InitializeCriticalSection(&csMonsters);
	MonsterSet monsterSet;
	RecvStream >> monsterSet;
	stringstream SendStream;
	SendStream << EPacketType::SYNC_MONSTER << endl;
	SendStream << monsterSet << endl;

	MonstersInfo = monsterSet;
	//printf_s("[INFO]SyncMonster %f \n", MonstersInfo.monsters[2].Health);
	
	Broadcast(SendStream, monsterSet.monsters[0].ueLevel);
	LeaveCriticalSection(&csMonsters);
}

void MainIocp::SyncCube(stringstream& RecvStream, stSOCKETINFO* pSocket)
{
	bool isOn;
	RecvStream >> isOn;
	stringstream SendStream;
	SendStream << EPacketType::SYNC_CUBE << endl;
	SendStream << isOn << endl;

	printf_s("[INFO]Sync Cube");

	Broadcast(SendStream, 1);
}