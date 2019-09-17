#pragma once
// #pragma comment(lib,"d3dcompiler.lib")
// #pragma comment(lib,"d3d12.lib")
// #pragma comment(lib,"dxgi.lib")
// #pragma comment(lib,"winmm.lib")
// 
// #include <d3d12.h>
// #include <dxgi1_4.h>
// #include <d3dcompiler.h>
// #include <DirectXMath.h>
// #include <DirectXPackedVector.h>
// #include <DirectXColors.h>
// #include <DirectXCollision.h>
// 
// using namespace DirectX;
// using namespace DirectX::PackedVector;
#include "..//..//TestProject/Player.h"

#define LOGIN_ERROR -1
#define LOGIN_OK 0
#define NEW_ID 1

#define MOVE_POS 10
#define ROTATION 11

void ProcessPacket(char* buf);

/*
패킷 구조체
SC  Server -> Client
CS	Client -> Server
*/
#pragma pack(push, 1)

struct SC_LOGIN_PACKET {
	char size;
	char type;
};
struct SC_MOVE_PACKET {
	char size;
	char type;
	short id;
	XMFLOAT3 position;
};

struct CS_MOVE_PACKET {
	char size;
	char type;
	short id;
	XMFLOAT3 position;
	// 이동에 필요한 정보들 넣기
};
struct CS_ROTATION_PACKET {
	char size;
	char type;

	float x, y, z;	// 회전각도
};

#pragma pack(pop)