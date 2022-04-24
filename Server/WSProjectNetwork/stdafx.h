#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "MSWSock.lib")


#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <exception>
#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <concurrent_priority_queue.h>
#include <windows.h>
#include "NetException.h"

// D3dx
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#pragma comment(lib, "d3d12.lib")
using namespace DirectX;

#define ABS(x) ((x < 0) ? -(x) : (x))

using namespace std::chrono_literals;

typedef unsigned char uchar;

struct PlayerInfo
{
	char name[20];
	short x;
	short y;
	short z;
	char characterType;
	XMFLOAT3 look = XMFLOAT3(0.0f,0.0f,1.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 right = XMFLOAT3(1.0f, 0.0f, 0.0f);

	short hp;
	short max_hp;
};
