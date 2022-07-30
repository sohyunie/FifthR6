// Fill out your copyright notice in the Description page of Project Settings.

#include "ClientSocket.h"
#include <sstream>
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformAffinity.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include <algorithm>
#include <string>
#include "NetPlayerController.h"
#include "TitleGameMode.h"

ClientSocket::ClientSocket()
	:StopTaskCounter(0)
{
	CharactersInfo.players.clear();
}


ClientSocket::~ClientSocket()
{
	delete Thread;
	Thread = nullptr;

	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::InitSocket()
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket InitSocket"));
	WSADATA wsaData;
	// ���� ������ 2.2�� �ʱ�ȭ
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0) {
		// std::cout << "Error : " << WSAGetLastError() << std::endl;		
		return false;
	}

	// TCP ���� ����
	// m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	// UdpServerSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
	if (ServerSocket == INVALID_SOCKET) {
		// std::cout << "Error : " << WSAGetLastError() << std::endl;
		return false;
	}

	// std::cout << "socket initialize success." << std::endl;
	return true;
}

bool ClientSocket::Connect(const char* pszIP, int nPort)
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket Connect"));

	// ������ ���� ������ ������ ����ü
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	// ������ ���� ��Ʈ �� IP
	stServerAddr.sin_port = htons(nPort);
	stServerAddr.sin_addr.s_addr = inet_addr(pszIP);

	// 	UdpServerAddr.sin_family = AF_INET;
	// 	// ������ ���� ��Ʈ �� IP
	// 	UdpServerAddr.sin_port = htons(UDP_SERVER_PORT);
	// 	UdpServerAddr.sin_addr.s_addr = inet_addr(pszIP);

	int nRet = connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) {
		// std::cout << "Error : " << WSAGetLastError() << std::endl;
		return false;
	}

	// std::cout << "Connection success..." << std::endl;

	return true;
}

bool ClientSocket::Login(const FText& Id, const FText& Pw)
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket Login"));
	stringstream SendStream;

	SendStream << EPacketType::LOGIN << endl;
	SendStream << 999 << endl;
	SendStream << TCHAR_TO_UTF8(*Id.ToString()) << endl;
	SendStream << TCHAR_TO_UTF8(*Pw.ToString()) << endl;

	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);

	if (nSendLen == -1)
		return false;

	int nRecvLen = recv(
		ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0
	);

	if (nRecvLen <= 0)
		return false;

	stringstream RecvStream;
	int PacketType;
	ID = 0;

	RecvStream << recvBuffer;
	RecvStream >> PacketType;
	RecvStream >> ID;

	UE_LOG(LogClass, Log, TEXT("1---[%d]---"), ID);
	if (PacketType != EPacketType::LOGIN)
		return false;

	if (ID != 0) {
		StartListen();
	}
	return ID != 0;
}

void ClientSocket::SetCharacterID(int id)
{
	CharacterID = id;
	UE_LOG(LogClass, Log, TEXT("SetCharacterID"));
	UE_LOG(LogClass, Log, TEXT("2---[%d]---"), ID);
	stringstream SendStream;
	SendStream << EPacketType::SET_CHARACTER << endl;
	SendStream << 999 << endl;
	SendStream << ID << endl;
	SendStream << CharacterID << endl;

	UE_LOG(LogClass, Log, TEXT("3---[%d]---"), ID);
	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);

	if (nSendLen == -1)
	{
		return;
	}
}

void ClientSocket::EnrollPlayer(cCharacter& info)
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket EnrollPlayer"));
	// ĳ���� ���� ����ȭ
	stringstream SendStream;
	// ��û ����
	SendStream << EPacketType::ENROLL_PLAYER << endl;
	SendStream << 999 << endl;
	SendStream << info << endl;
	UE_LOG(LogClass, Log, TEXT("Enroll info.characterID : [%d]"), info.characterID);

	// ĳ���� ���� ����
	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);

	if (nSendLen == -1)
	{
		return;
	}
	isEnroll = true;
}

void ClientSocket::SendPlayer(cCharacter& info)
{
	//UE_LOG(LogClass, Log, TEXT("ClientSocket SendPlayer"));
	// ĳ���� ���� ����ȭ
	stringstream SendStream;
	// ��û ����
	SendStream << EPacketType::SEND_PLAYER << endl;
	SendStream << 999 << endl;
	SendStream << info << endl;

	// ĳ���� ���� ����
	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);

	if (nSendLen == -1)
	{
		return;
	}
}

cCharactersInfo* ClientSocket::RecvCharacterInfo(stringstream& RecvStream)
{
	//CharactersInfo.players.clear();
	//UE_LOG(LogClass, Log, TEXT("ClientSocket RecvCharacterInfo"));
	// �������� ĳ���� ������ ��� ��ȯ		
	RecvStream >> CharactersInfo;
	return &CharactersInfo;
}

string* ClientSocket::RecvChat(stringstream& RecvStream)
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket RecvChat"));
	// �������� ä�� ������ ��� ��ȯ
	RecvStream >> sChat;
	std::replace(sChat.begin(), sChat.end(), '_', ' ');
	return &sChat;
}

cCharactersInfo* ClientSocket::RecvNewPlayer(stringstream& RecvStream)
{
	NewPlayer.players.clear();
	UE_LOG(LogClass, Log, TEXT("Client Socket RecvNewPlayer"));
	RecvStream >> NewPlayer;
	return &NewPlayer;
}

MonsterSet* ClientSocket::RecvMonsterSet(stringstream& RecvStream)
{
	// ���� ���� ������ ��� ��ȯ
	RecvStream >> MonsterSetInfo;
	return &MonsterSetInfo;
}

Monster* ClientSocket::RecvMonster(stringstream& RecvStream)
{
	// ���� ���� ������ ��� ��ȯ
	RecvStream >> MonsterInfo;
	return &MonsterInfo;
}

void ClientSocket::LogoutPlayer(int SessionId)
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket LogoutPlayer"));
	// �������� �α׾ƿ���ų ĳ���� ���� ����
	stringstream SendStream;
	SendStream << EPacketType::LOGOUT_PLAYER << endl;
	SendStream << 999 << endl;
	SendStream << SessionId << endl;

	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);

	if (nSendLen == -1)
	{
		return;
	}

	closesocket(ServerSocket);
	WSACleanup();
}

char* ClientSocket::UdpTest()
{
	int nResult;
	char* sendBuffer = (char*)"hello";
	int sendBufferLen = strlen(sendBuffer);

	SOCKADDR_IN fromAddr;
	int fromAddrLen = sizeof(fromAddr);
	nResult = sendto(
		UdpServerSocket, sendBuffer, sendBufferLen, 0, (sockaddr*)&UdpServerAddr, sizeof(UdpServerAddr)
	);

	nResult = recvfrom(
		UdpServerSocket, UdpRecvBuffer, MAX_BUFFER, 0, (sockaddr*)&fromAddr, &fromAddrLen
	);

	return UdpRecvBuffer;
}

void ClientSocket::DamagePlayer(int SessionId)
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket DamagePlayer"));
	// �������� �������� �� ĳ���� ���� ����
	stringstream SendStream;
	SendStream << EPacketType::HIT_PLAYER << endl;
	SendStream << 999 << endl;
	SendStream << SessionId << endl;

	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);
}

void ClientSocket::SendChat(const int& SessionId, const string& Chat)
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket SendChat"));
	// �������� ä�� ����
	stringstream SendStream;
	SendStream << EPacketType::CHAT << endl;
	SendStream << 999 << endl;
	SendStream << SessionId << endl;
	SendStream << Chat << endl;

	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);
}

void ClientSocket::SetPlayerController(ANetPlayerController* pPlayerController)
{
	// �÷��̾� ��Ʈ�ѷ� ����
	if (pPlayerController)
	{
		PlayerController = pPlayerController;
	}
}

void ClientSocket::SetTitleGameMode(ATitleGameMode* pTitleGameMode) {
	if (pTitleGameMode) {
		titleGameMode = pTitleGameMode;
	}
}

void ClientSocket::CloseSocket()
{
	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::Init()
{
	return true;
}

uint32 ClientSocket::Run()
{
	UE_LOG(LogClass, Log, TEXT("ClientSocket Run"));
	// �ʱ� init ������ ��ٸ�
	FPlatformProcess::Sleep(0.03);
	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (StopTaskCounter.GetValue() == 0 && PlayerController != nullptr)
	{
		stringstream RecvStream;
		int PacketType;
		int nRecvLen = recv(
			ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0
		);
		if (nRecvLen > 0)
		{
			RecvStream << recvBuffer;
			RecvStream >> PacketType;

			switch (PacketType)
			{
			case EPacketType::RECV_PLAYER:
			{
				if (isEnroll)
					PlayerController->RecvWorldInfo(RecvCharacterInfo(RecvStream));
			}
			break;
			case EPacketType::CHAT:
			{
				if (isEnroll)
					PlayerController->RecvChat(RecvChat(RecvStream));
			}
			break;
			case EPacketType::ENTER_NEW_PLAYER:
			{
				if (isEnroll)
					PlayerController->RecvNewPlayer(RecvNewPlayer(RecvStream));
			}
			break;
			case EPacketType::SYNC_MONSTER:
			{
				if (isEnroll)
					PlayerController->RecvMonsterSet(RecvMonsterSet(RecvStream));
			}
			break;
			case EPacketType::ACTION_SKILL:
			{
				if (isEnroll) {
					int sessionID;
					int id;
					RecvStream >> sessionID;
					RecvStream >> id;
					PlayerController->RecvActionSkill(sessionID, id);
				}
			}
			break;
			case EPacketType::DESTROY_MONSTER:
			{
				if (isEnroll)
					PlayerController->RecvDestroyMonster(RecvMonster(RecvStream));
			}
			break;
			case EPacketType::PLAY_GAME:
			{
				if (isStart)
					break;
				RecvStream >> isStart;
				// �� �̵�
				if (isStart) {
					UE_LOG(LogClass, Log, TEXT("PLAY_GAME!!"));
					titleGameMode->MoveInGame();
				}
				else {
					UE_LOG(LogClass, Log, TEXT("PLAY_GAME WAIT!!"));
				}
			}
			case EPacketType::DESTRUCT_KEY:
			{
				if (isEnroll) {
					int checkSum = 0;
					int id;
					RecvStream >> checkSum;
					RecvStream >> id;
					if (checkSum == 999) {
						PlayerController->RecvDestructKey(id);
					}
					else {
						UE_LOG(LogClass, Log, TEXT("DESTRUCT_KEY checkSum error"));
					}
				}
			}
			break;
			default:
				break;
			}
		}
	}
	return 0;
}

void ClientSocket::Stop()
{
	// thread safety ������ ������ while loop �� ���� ���ϰ� ��
	StopTaskCounter.Increment();
}

void ClientSocket::Exit()
{
}

bool ClientSocket::StartListen()
{
	UE_LOG(LogClass, Log, TEXT("Listen Start"));
	// ������ ����
	if (Thread != nullptr) return false;
	Thread = FRunnableThread::Create(this, TEXT("ClientSocket"), 0, TPri_BelowNormal);
	return (Thread != nullptr);
}

void ClientSocket::StopListen()
{
	// ������ ����
	Stop();
	Thread->WaitForCompletion();
	Thread->Kill();
	delete Thread;
	Thread = nullptr;
	StopTaskCounter.Reset();
}

void ClientSocket::HitMonster(const int& MonsterId)
{
	// �������� �������� �� ���� ���� ����
	stringstream SendStream;
	SendStream << EPacketType::HIT_MONSTER << endl;
	SendStream << 999 << endl;
	SendStream << MonsterId << endl;

	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);
}

void ClientSocket::SendSyncMonster(MonsterSet& monsterSet)
{
	// �������� ���� ������ ����
	stringstream SendStream;
	SendStream << EPacketType::SYNC_MONSTER << endl;
	SendStream << 999 << endl;
	SendStream << monsterSet << endl;
	//FString name = SendStream.str().c_str();
	//TArray<FStringFormatArg> args;

	//args.Add(FStringFormatArg(name));
	//FString string = FString::Format(TEXT("SendSyncMonster = {0}"), args);


	//UE_LOG(LogTemp, Warning, TEXT("SendSyncMonster : %s"), *string);

	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);
}


void ClientSocket::SendActionSkill(int sessionID, int id)
{
	UE_LOG(LogClass, Log, TEXT("Action Skill"));
	// ĳ���� ���� ����ȭ
	stringstream SendStream;
	// ��û ����
	SendStream << EPacketType::ACTION_SKILL << endl;
	SendStream << 999 << endl;
	SendStream << sessionID << endl;
	SendStream << id << endl;

	// ĳ���� ���� ����
	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);

	if (nSendLen == -1)
	{
		return;
	}
}

void ClientSocket::SendDestructKey(int sessionID, int keyID)
{
	UE_LOG(LogClass, Log, TEXT("SendDestructKey"));
	// ĳ���� ���� ����ȭ
	stringstream SendStream;
	// ��û ����
	SendStream << EPacketType::DESTRUCT_KEY << endl;
	SendStream << 999 << endl;
	SendStream << sessionID << endl;
	SendStream << keyID << endl;

	// ĳ���� ���� ����
	int nSendLen = send(
		ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0
	);

	if (nSendLen == -1)
	{
		return;
	}
}