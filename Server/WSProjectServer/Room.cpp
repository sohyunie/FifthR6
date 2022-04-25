#include "Room.h"

Room::Room()
{

}

Room::Room(int uid)
{
	this->JoinRoom(uid);
}

bool Room::JoinRoom(int uid)
{
	for (int i = 0; i < 4; i++)
	{
		if (playerUids[i] == 0)
		{
			playerUids[i] = uid;
			return true;
		}
	}
	return false;
}

int Room::GetUidIndex(int uid)
{
	for (int i = 0; i < 4; i++)
	{
		if (playerUids[i] == uid)
		{
			return i;
		}
	}
	return -1;
}

int Room::GetPlayerUidByIndex(int index)
{
	return this->playerUids[index];
}

bool Room::FindPlayerByUid()
{
	return false;
}

void Room::Loop()
{
	if (!bStartGame)
		return;



}
