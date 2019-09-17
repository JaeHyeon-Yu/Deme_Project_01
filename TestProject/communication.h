#pragma once
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <string>
#include "Player.h"
#define �������� "192.168.200.128"
#define �츮�� "192.168.10.30"

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
	// �̵��� �ʿ��� ������ �ֱ�
};
struct CS_ROTATION_PACKET {
	char size;
	char type;

	float x, y, z;	// ȸ������
};

#pragma pack(pop)

void SetSocket(SOCKET& sock);
void SendPacket(const SOCKET& sock, CPlayer* cp);
void RecvPacket(const SOCKET& sock, CPlayer*& cp);

void PacketProcess(char* buf, CPlayer*& cp);