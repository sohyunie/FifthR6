#pragma once
class InGameController
{
private:
	list<Object> mObjects;
public:
	InGameController();
	bool CollisionCheck(Player player);
};

