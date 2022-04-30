#pragma once

enum eObjectType {
	STATIC_OBJECT,
	BOMB,
	PLLAYER,
	LANDOMBOX,
};

// 오브젝트: 맵에 설치되는 애들, 폭탄, 플레이어, 랜덤박스(아이템)
class Object {
protected:
	eObjectType mObjectType;
	BoundingBox mBB;
public:
	eObjectType GetObjectType();
	Object();
	Object(eObjectType objectType, XMFLOAT3 position, XMFLOAT3 extents);
	BoundingBox GetBoundingBox();
};

class Player : public Object
{
private :
	PlayerInfo mPlayerInfo;
public : 
	PlayerInfo GetPlayerInfo();
};