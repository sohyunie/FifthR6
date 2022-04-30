#pragma once

enum eObjectType {
	STATIC_OBJECT,
	BOMB,
	PLLAYER,
	LANDOMBOX,
};

// ������Ʈ: �ʿ� ��ġ�Ǵ� �ֵ�, ��ź, �÷��̾�, �����ڽ�(������)
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