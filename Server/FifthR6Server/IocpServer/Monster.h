#pragma once

#include "CommonClass.h"

class Monster
{
public:
	Monster();
	virtual ~Monster();

	// �÷��̾�� �̵�
	void MoveTo(const cCharacter& target);
	// �÷��̾� Ÿ��
	void HitPlayer(cCharacter& target);
	// �ǰ�
	void Damaged(float damage);
	// ����ִ��� ����
	bool IsAlive();
	// ���������� ����
	bool IsAttacking();
	// �÷��̾ �߰� ������ �ִ���
	bool IsPlayerInTraceRange(const cCharacter& target);
	// �÷��̾ Ÿ�� ������ �ִ���
	bool IsPlayerInHitRange(const cCharacter& target);
	// ��ġ ����
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
		stream << info.bIsAttacking << endl;
		stream << info.ueLevel << endl;

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
		stream >> info.bIsAttacking;
		stream >> info.ueLevel;

		return stream;
	}

	float	X;				// X��ǥ
	float	Y;				// Y��ǥ
	float	Z;				// Z��ǥ
	float	VX;				// X��ǥ
	float	VY;				// Y��ǥ
	float	VZ;				// Z��ǥ
	float Yaw;
	float Pitch;
	float Roll;
	float	Health;			// ü��
	int		Id;				// ���� id
	bool	bIsAttacking;	// ����������
	int		ueLevel;

private:
	bool	bIsTracking;	// �߰�������
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
		info.monsters.clear();

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
