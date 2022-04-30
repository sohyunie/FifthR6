#include "common.h"
#include "Object.h"

eObjectType Object::GetObjectType()
{
	return mObjectType;
}

Object::Object(eObjectType objectType, XMFLOAT3 position, XMFLOAT3 extents)
	: mObjectType(objectType),
	mBB(BoundingBox(position, extents))
{
	
}

BoundingBox Object::GetBoundingBox()
{
	return mBB;
}

Object::Object()
{
}

PlayerInfo Player::GetPlayerInfo()
{
	return mPlayerInfo;
}
