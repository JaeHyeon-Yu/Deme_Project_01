#pragma once
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <string>
#include "Player.h"
#define 석준이집 "192.168.200.128"
#define 우리집 "192.168.10.30"

#define MAX_USER 2
#define SERVERPORT 9000
#define LOGIN_ERROR -1
#define LOGIN_OK 0
#define NEW_ID 1

#define MOVE_POS 10
#define ROTATION 11

#pragma pack(push, 1)

struct SC_LOGIN_PACKET {
	char size;
	char type;
	short id;
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

void SetSocket(SOCKET& sock);
void SendPacket(const SOCKET& sock, CPlayer* cp);
void RecvPacket(const SOCKET& sock, CPlayer*& cp);

void PacketProcess(char* buf, CPlayer*& cp);