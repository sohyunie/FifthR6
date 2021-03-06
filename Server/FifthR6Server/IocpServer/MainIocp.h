#pragma once

// 멀티바이트 집합 사용시 define
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <map>
#include <vector>
#include <iostream>
#include "CommonClass.h"
#include "DBConnector.h"
#include "Monster.h"
#include "IocpBase.h"

using namespace std;

// DB 정보
#define DB_ADDRESS		"localhost"
#define	DB_PORT			3306
#define DB_ID			"test"
#define DB_PW			"Kk471601"
#define DB_SCHEMA		"user_data"

class MainIocp : public IocpBase
{
public:
	MainIocp();
	virtual ~MainIocp();
	
	// 작업 스레드 생성
	virtual bool CreateWorkerThread() override;
	// 작업 스레드
	virtual void WorkerThread() override;
	
	// 클라이언트에게 송신
	static void Send(stSOCKETINFO * pSocket);	

private:
	static cCharactersInfo	CharactersInfo;	// 접속한 클라이언트의 정보를 저장	
	static map<int, SOCKET> SessionSocket;	// 세션별 소켓 저장
	static float			HitPoint;		// 타격 데미지
	static DBConnector 		Conn;			// DB 커넥터
	static CRITICAL_SECTION csPlayers;		// CharactersInfo 임계영역
	static CRITICAL_SECTION csMonsters;		// CharactersInfo 임계영역
	static MonsterSet		MonstersInfo;	// 몬스터 집합 정보
	static int				KeyCount;
	static int				ClearUserCount;

	FuncProcess fnProcess[100];

	// DB에 로그인
	static void Login(stringstream & RecvStream, stSOCKETINFO * pSocket);
	// 캐릭터 등록
	static void SetCharacter(stringstream& RecvStream, stSOCKETINFO* pSocket);
	// 캐릭터 초기 등록
	static void EnrollCharacter(stringstream & RecvStream, stSOCKETINFO * pSocket);
	// 캐릭터 위치 동기화
	static void SyncCharacters(stringstream & RecvStream, stSOCKETINFO * pSocket);
	// 캐릭터 로그아웃 처리
	static void LogoutCharacter(stringstream & RecvStream, stSOCKETINFO * pSocket);
	// 캐릭터 피격 처리
	static void HitCharacter(stringstream & RecvStream, stSOCKETINFO * pSocket);
	// 채팅 수신 후 클라이언트들에게 송신
	static void BroadcastChat(stringstream & RecvStream, stSOCKETINFO * pSocket);
	// 몬스터 피격 처리
	static void HitMonster(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void SyncMonster(stringstream& RecvStream, stSOCKETINFO* pSocket);
	static void ActionSkill(stringstream& RecvStream, stSOCKETINFO* pSocket);
	static void DestructKey(stringstream& RecvStream, stSOCKETINFO* pSocket);
	static void ClearGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	// 브로드캐스트 함수
	static void Broadcast(stringstream & SendStream, int UELevel);	
	// 다른 클라이언트들에게 새 플레이어 입장 정보 보냄
	static void BroadcastNewPlayer(cCharactersInfo & player, int UELevel);
	// 캐릭터 정보를 버퍼에 기록
	static void WriteCharactersInfoToSocket(stSOCKETINFO * pSocket);	
	static void OtherBroadcast(stringstream& SendStream, int UELevel, int sessionID);
};
