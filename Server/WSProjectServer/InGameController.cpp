#include "InGameController.h"
#include "Object.h"
#include "common.h"

InGameController::InGameController()
{
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
	mObjects.push_back(Object(eObjectType::STATIC_OBJECT, XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0)));
}

bool InGameController::CollisionCheck(Player player)
{
	for (Object obj : mObjects) {
		ContainmentType type = player.GetBoundingBox().Contains(obj.GetBoundingBox());
		if (type == ContainmentType::INTERSECTS)
		{
			switch (obj.GetObjectType()) {
			case eObjectType::BOMB:
				break;
			case eObjectType::PLAYER:
				break;
			case eObjectType::RANDOMBOX:
				break;
			case eObjectType::STATIC_OBJECT:
				break;
			}
		}
	}
	return false;
}
