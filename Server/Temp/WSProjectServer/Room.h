#pragma once

class Room
{
public:
	Room();
	Room(int uid);
	bool JoinRoom(int uid);			// ���� �� �� �ִ� room�� ������ true�� return 
	int GetUidIndex(int uid);		// �� room�� ���̵� ����� �뵵 (id�� private���� �Ǿ� �����ϱ�
	int GetPlayerUidByIndex(int index);		// �� room�� ���̵� ����� �뵵 (id�� private���� �Ǿ� �����ϱ�)
	bool FindPlayerByUid();		// ���� � ������ ã������ ���� �� �� ������ �� �濡 �ִ��� �Ǵ� 
private:
	int id;
	int playerUids[4] = {0,0,0,0};
	int playerCount;
};