#pragma once
//#include"stdafx.h"
// D3dx
#include <d3d12.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d12.lib")

const short SERVER_PORT = 4000;

const int  WORLD_HEIGHT = 2000;
const int  WORLD_WIDTH = 2000;
const int  MAX_NAME_SIZE = 20;
const int  MAX_CHAT_SIZE = 100;
const int  MAX_USER = 10000;
const int  MAX_NPC = 200000;
constexpr int NPC_ID_START = MAX_USER;
constexpr int NPC_ID_END = MAX_USER + MAX_NPC - 1;

constexpr int SECTOR_WIDTH = WORLD_WIDTH / 10;
constexpr int SECTOR_HEIGHT = WORLD_HEIGHT / 10;

const char CS_PACKET_LOGIN = 1;
const char CS_PACKET_MOVE = 2;
const char CS_PACKET_ATTACK = 3;
const char CS_PACKET_CHAT = 4;
const char CS_PACKET_TELEPORT = 5;
const char CS_PACKET_GAME_JOIN = 6;
const char CS_PACKET_ROTATE = 7;

const char SC_PACKET_LOGIN_OK = 1;
const char SC_PACKET_MOVE = 2;
const char SC_PACKET_PUT_OBJECT = 3;
const char SC_PACKET_REMOVE_OBJECT = 4;
const char SC_PACKET_CHAT = 5;
const char SC_PACKET_LOGIN_FAIL = 6;
const char SC_PACKET_STATUS_CHANGE = 7;
const char SC_PACKET_BATTLE_RESULT = 8;
const char SC_PACKET_GAME_JOIN = 9;
const char SC_PACKET_ROTATE = 10;

const int MaxBufferSize = 1024;
const int RANGE = 7;

#pragma pack (push, 1)
struct cs_packet_login {
	unsigned char size;
	char	type;
	char	name[MAX_NAME_SIZE];
};


//struct cs_packet_move {
//	unsigned char size;
//	char	type;
//	char	direction;			// 0 : up,  1: down, 2:left, 3:right
//	int		move_time;
//};

struct cs_packet_attack {
	unsigned char size;
	char	type;
};

struct cs_packet_chat {
	unsigned char size;
	char	type;
	char	message[MAX_CHAT_SIZE];
};

struct cs_packet_teleport { 
	// 서버에서 장애물이 없는 랜덤 좌표로 텔레포트 시킨다.
	// 더미 클라이언트에서 동접 테스트용으로 사용.
	unsigned char size;
	char	type;
};

struct sc_packet_login_ok {
	unsigned char size;
	char type;
	int		id;
};

//struct sc_packet_move {
//	unsigned char size;
//	char type;
//	int		id;
//	char  x, y;
//	int		move_time;
//};

struct sc_packet_put_object {
	unsigned char size;
	unsigned char type;
	unsigned short id;
	XMFLOAT3 position;
	unsigned char object_type;
};

struct sc_packet_remove_object {
	unsigned char size;
	char type;
	int id;
};

struct sc_packet_chat {
	unsigned char size;
	char type;
	int id;
	char message[MAX_CHAT_SIZE];
};

struct sc_packet_login_fail {
	unsigned char size;
	char type;
	char reason;		// 0: 중복 ID,  1:사용자 Full
};

struct sc_packet_status_change {
	unsigned char size;
	char type;
	char	level;
	char	hp, maxhp;
	int		exp;
};

struct sc_packet_battle_result
{
	unsigned char size;
	char type;
	char result_type; // -1: None, 0: 플레이어 공격, 1: 플레이어 피해, 2: 경험치 획득
	int target;
	int value;
};

////////////////////////////////////////////////////////////////////////////////////////////
// gmae_join 패킷
struct cs_packet_game_join {
	unsigned char size;
	unsigned char type;
	unsigned short id;
};

struct sc_packet_game_join {
	unsigned char size;
	unsigned char type;
	unsigned short id;
	unsigned char hp;
	unsigned char maxHp;
	XMFLOAT3 position;
};

// move 패킷
struct cs_packet_move {
	unsigned char size;
	unsigned char type;
	unsigned short id;
	unsigned short key;
};

struct sc_packet_move {
	unsigned char size;
	unsigned char type;
	unsigned short id;
	unsigned short key;
	XMFLOAT3 position;
	XMFLOAT4 quaternion;
};

// rotate 패킷
struct cs_packet_rotate {
	unsigned char size;
	unsigned char type;
	unsigned short id;
	XMFLOAT3 vLook;
	XMFLOAT3 vRight;
	XMFLOAT3 vUp;
};

struct sc_packet_rotate {
	unsigned char size;
	unsigned char type;
	unsigned short id;
	XMFLOAT3 look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 right = XMFLOAT3(1.0f, 0.0f, 0.0f);
};

// 게임 결과, 공수교체 관련, 아이템 관련 프로토콜 추가 필요 

#pragma pack(pop)