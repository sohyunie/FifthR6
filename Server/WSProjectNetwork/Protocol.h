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
const char CS_PACKET_JOIN_ROOM = 2;
const char CS_PACKET_READY = 3;
const char CS_PACKET_MOVE = 4;
const char CS_PACKET_JUMP = 5;
const char CS_PACKET_GUARD = 6;
const char CS_PACKET_MELEE_ATTACK = 7;
const char CS_PACKET_SKILL_ATTACK = 8;
const char CS_PACKET_MAP_CHANGE = 9;

const char SC_PACKET_LOGIN_OK = 1;
const char SC_PACKET_JOIN_ROOM = 2;
const char SC_PACKET_GAME_START = 3;
const char SC_PACKET_PUT_OBJECT = 4;
const char SC_PACKET_MOVE = 5;
const char SC_PACKET_JUMP = 6;
const char SC_PACKET_GUARD = 7;
const char SC_PACKET_REMOVE_OBJECT = 8;
const char SC_PACKET_LOGIN_FAIL = 9;
const char SC_PACKET_STATUS_CHANGE = 10;
const char SC_PACKET_MELEE_ATTACK = 11;
const char SC_PACKET_SKILL_ATTACK = 12;
const char SC_PACKET_GAME_RESULT = 13;
const char SC_PACKET_MAP_CHANGE = 14;

const int MaxBufferSize = 1024;
const int RANGE = 7;

#pragma pack (push, 1)
struct cs_packet_login {
	unsigned char size;
	char	type;
	char	name[MAX_NAME_SIZE];
};

// 룸 선택
struct cs_packet_join_room {
	unsigned char size;
	char type;
	short id;
	short roomNumber;
};

struct cs_packet_ready {
	unsigned char size;
	char type;
	short roomNumber;
	bool state;			// ready 취소 가능
};

struct cs_packet_move {
	unsigned char size;
	char type;
	short id;
	unsigned short key;
};

struct cs_packet_jump {
	unsigned char size;
	char type;
	short id;
};

struct cs_packet_guard {
	unsigned char size;
	char type;
	short id;
};

struct cs_packet_melee_attack
{
	unsigned char size;
	char type;
	short id;
	char characterType;
};

struct cs_packet_skill_attack
{
	unsigned char size;
	char type;
	short id;
	char characterType;
};

struct cs_packet_map_change {
	unsigned char size;
	char type;
	short id;
	short mapID;
};




struct sc_packet_login_ok
{
	unsigned char size;
	char type;
	int		id;
	short	level;
};

struct sc_packet_join_room
{
	unsigned char size;
	char type;
	short id;
	char playerName1[MAX_NAME_SIZE];
	char playerName2[MAX_NAME_SIZE];
	short roomNumber;
	bool success;		// 해당 룸에 들어갈 수 있는지 체크 (인원 초과면 false return)
};

struct sc_packet_game_start
{
	unsigned char size;
	char type;
	short	id;
	short	x, y, z;
	short	level;
	short	hp, maxhp;
	int		exp;
	char	characterType;		// 불, 물, 번개 판단
	short mapID;
};

// object에 플레이어, 몬스터 등 다 포함
struct sc_packet_put_object 
{
	unsigned char size;
	char type;
	short id;
	short x, y, z;
	char object_type;
	char name[MAX_NAME_SIZE];
}; 


// move 패킷
struct sc_packet_move {
	unsigned char size;
	char type;
	short id;
	short x;
	short y;
	short z;
	// XMFLOAT4 quaternion;
};

struct sc_packet_jump {
	unsigned char size;
	char type;
	short id;
	short x;
	short y;
	short z;
	// XMFLOAT4 quaternion;
};

struct sc_packet_guard {
	unsigned char size;
	char type;
	short id;
};

struct sc_packet_remove_object 
{
	unsigned char size;
	char type;
	short id;
};

struct sc_packet_login_fail {
	unsigned char size;
	char type;
	int	 reason;		// 0: 중복 ID,  1:사용자 Full
};

struct sc_packet_status_change {
	unsigned char size;
	char	type;
	short	level;
	short	hp, maxhp;
	int		exp;
	short	mapID;
};


// 근접공격
struct sc_packet_melee_attack
{
	unsigned char size;
	char type;
	short id;
	char characterType;
};


// 스킬 공격
struct sc_packet_skill_attack
{
	unsigned char size;
	char type;
	short id;
	char characterType;
};

struct sc_packet_game_result
{
	unsigned char size;
	char type;
	char result;
};

struct sc_packet_map_change {
	unsigned char size;
	char type;
	short id;
	short mapID;
};




// 보류
// rotate 패킷
//struct cs_packet_rotate 
//{
//	unsigned char size;
//	unsigned char type;
//	unsigned short id;
//	XMFLOAT3 vLook;
//	XMFLOAT3 vRight;
//	XMFLOAT3 vUp;
//};
//
//struct sc_packet_rotate 
//{
//	unsigned char size;
//	unsigned char type;
//	unsigned short id;
//	XMFLOAT3 look = XMFLOAT3(0.0f, 0.0f, 1.0f);
//	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
//	XMFLOAT3 right = XMFLOAT3(1.0f, 0.0f, 0.0f);
//};


#pragma pack(pop)