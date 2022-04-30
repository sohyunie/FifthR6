#pragma once

class Room
{
public:
	Room();
	Room(int uid);
	bool JoinRoom(int uid);			// 내가 들어갈 수 있는 room이 있으면 true로 return 
	int GetUidIndex(int uid);		// 이 room의 아이디를 뱉어줄 용도 (id가 private으로 되어 있으니까
	int GetPlayerUidByIndex(int index);		// 이 room의 아이디를 뱉어줄 용도 (id가 private으로 되어 있으니까)
	bool FindPlayerByUid();		// 내가 어떤 유저를 찾으려고 했을 때 그 유저가 이 방에 있는지 판단 
private:
	int id;
	int playerUids[4] = {0,0,0,0};
	int playerCount;
};