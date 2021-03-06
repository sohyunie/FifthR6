#pragma once

#include "CommonClass.h"

class Monster
{
public:
	Monster();
	virtual ~Monster();

	// 플레이어로 이동
	void MoveTo(const cCharacter& target);
	// 플레이어 타격
	void HitPlayer(cCharacter& target);
	// 피격
	void Damaged(float damage);
	// 살아있는지 여부
	bool IsAlive();
	// 공격중인지 여부
	//bool IsAttacking();
	// 플레이어가 추격 범위에 있는지
	bool IsPlayerInTraceRange(const cCharacter& target);
	// 플레이어가 타격 범위에 있는지
	bool IsPlayerInHitRange(const cCharacter& target);
	// 위치 설정
	void SetLocation(float x, float y, float z);

	friend ostream& operator<<(ostream& stream, Monster& info)
	{
		stream << info.X << endl;
		stream << info.Y << endl;
		stream << info.Z << endl;
		stream << info.VX << endl;
		stream << info.VY << endl;
		stream << info.VZ << endl;
		stream << info.Yaw << endl;
		stream << info.Pitch << endl;
		stream << info.Roll << endl;
		stream << info.Health << endl;
		stream << info.Id << endl;
		stream << info.IsAttacking << endl;
		stream << info.UELevel << endl;

		return stream;
	}

	friend istream& operator>>(istream& stream, Monster& info)
	{
		stream >> info.X;
		stream >> info.Y;
		stream >> info.Z;
		stream >> info.VX;
		stream >> info.VY;
		stream >> info.VZ;
		stream >> info.Yaw;
		stream >> info.Pitch;
		stream >> info.Roll;
		stream >> info.Health;
		stream >> info.Id;
		stream >> info.IsAttacking;
		stream >> info.UELevel;

		return stream;
	}

	float	X;				// X좌표
	float	Y;				// Y좌표
	float	Z;				// Z좌표
	float	VX;				// X좌표
	float	VY;				// Y좌표
	float	VZ;				// Z좌표
	float Yaw;
	float Pitch;
	float Roll;
	float	Health;			// 체력
	int		Id;				// 고유 id
	bool	IsAttacking;	// 공격중인지
	int		UELevel;

private:
	//bool	bIsTracking;	// 추격중인지
};

class MonsterSet
{
public:
	map<int, Monster> monsters;

	friend ostream& operator<<(ostream& stream, MonsterSet& info)
	{
		stream << info.monsters.size() << endl;
		for (auto& kvp : info.monsters)
		{
			stream << kvp.first << endl;
			stream << kvp.second << endl;
		}

		return stream;
	}

	friend istream& operator>>(istream& stream, MonsterSet& info)
	{
		int nMonsters = 0;
		int PrimaryId = 0;
		Monster monster;
		//info.monsters.clear();

		stream >> nMonsters;
		for (int i = 0; i < nMonsters; i++)
		{
			stream >> PrimaryId;
			stream >> monster;
			info.monsters[PrimaryId] = monster;
		}

		return stream;
	}
};
