#pragma once
#include <string>

#define SERVERPORT 9000
#define MAX_USER 10


enum LOGIN_STATE {
	OFF, OK, ERR, NEW
};

struct Packet {
	// 데이터 통신에 사용할 패킷
	short id;	// 배열 index값

	//플레이어의 위치 벡터, x축, y축, z축 벡터이다.
	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Right;
	XMFLOAT3	m_xmf3Up;
	XMFLOAT3	m_xmf3Look;

	//플레이어가 로컬 x축, y축, z축으로 얼마만큼 회전했는가를 나타낸다.
	float		m_fPitch;
	float		m_fYaw;
	float		m_fRoll;

	//플레이어의 이동 속도를 나타내는 벡터이다.
	XMFLOAT3	m_xmf3Velocity;
	//플레이어에 작용하는 중력을 나타내는 벡터이다.
	XMFLOAT3	m_xmf3Gravity;
	//플레이어에 작용하는 마찰력을 나타낸다.
	float		m_fFriction;
};

void err_display(const char* msg);
void err_quit(const char* msg);