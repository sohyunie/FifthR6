#pragma once

#ifdef COMMONCLASS_EXPORTS
#define COMMONCLASS_API __declspec(dllexport)
#else
#define COMMONCLASS_API __declspec(dllimport)
#endif

#include <iostream>
#include <map>

using namespace std;

#define MAX_CLIENTS 100

enum COMMONCLASS_API EPacketType
{
	LOGIN,
	SET_CHARACTER,
	ENROLL_PLAYER,
	SEND_PLAYER,
	RECV_PLAYER,
	LOGOUT_PLAYER,
	HIT_PLAYER,
	DAMAGED_PLAYER,
	CHAT,
	ENTER_NEW_PLAYER,
	HIT_MONSTER,
	SYNC_MONSTER,
	ACTION_SKILL,
	DESTROY_MONSTER,
	PLAY_GAME,
	DESTRUCT_KEY,
	CLEAR_GAME,
};

class COMMONCLASS_API cCharacter {
public:
	cCharacter();
	~cCharacter();

	// 세션 아이디
	int		SessionId;
	// 위치
	float	X;
	float	Y;
	float	Z;
	// 회전값
	float	Yaw;
	float	Pitch;
	float	Roll;
	// 속도
	float VX;
	float VY;
	float VZ;
	// 속성
	bool	IsAlive;		
	float	HealthValue;
	bool	IsAttacking;

	int		UELevel;
	bool	IsMaster;

	int		characterID;

	friend ostream& operator<<(ostream &stream, cCharacter& info)
	{
		stream << info.SessionId << endl;
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.VX << endl;
		stream << info.VY << endl;
		stream << info.VZ << endl;
		stream << info.Yaw << endl;
		stream << info.Pitch << endl;
		stream << info.Roll << endl;
		stream << info.IsAlive << endl;		
		stream << info.HealthValue << endl;
		stream << info.IsAttacking << endl;
		stream << info.UELevel << endl;
		stream << info.IsMaster << endl;
		stream << info.characterID << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, cCharacter& info)
	{
		stream >> info.SessionId;
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.VX;
		stream >> info.VY;
		stream >> info.VZ;
		stream >> info.Yaw;
		stream >> info.Pitch;
		stream >> info.Roll;
		stream >> info.IsAlive;		
		stream >> info.HealthValue;
		stream >> info.IsAttacking;
		stream >> info.UELevel;
		stream >> info.IsMaster;
		stream >> info.characterID;

		return stream;
	}
};

class COMMONCLASS_API cCharactersInfo
{
public:
	cCharactersInfo();
	~cCharactersInfo();
	
	map<int, cCharacter> players;

	friend ostream& operator<<(ostream &stream, cCharactersInfo& info)
	{
		stream << info.players.size() << endl;
		//cout << "cCharactersInfo size : " << info.players.size() << endl;
		for (auto& kvp : info.players)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream &operator>>(istream &stream, cCharactersInfo& info)
	{
		int nPlayers = 0;
		int SessionId = 0;
		cCharacter Player;
		//info.players.clear();

		stream >> nPlayers;
		cout << "cCharactersInfo nPlayers : " << nPlayers << endl;
		for (int i = 0; i < nPlayers; i++)
		{
			stream >> SessionId;
			stream >> Player;
			info.players[SessionId] = Player;			
		}

		return stream;
	}
};

class COMMONCLASS_API CommonClass
{
public:
	CommonClass();
	~CommonClass();
};

