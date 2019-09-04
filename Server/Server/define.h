#pragma once
#include <string>

#define SERVERPORT 9000
#define MAX_USER 10


enum LOGIN_STATE {
	OFF, OK, ERR, NEW
};

struct Packet {
	// ������ ��ſ� ����� ��Ŷ
	short id;	// �迭 index��

	//�÷��̾��� ��ġ ����, x��, y��, z�� �����̴�.
	XMFLOAT3	m_xmf3Position;
	XMFLOAT3	m_xmf3Right;
	XMFLOAT3	m_xmf3Up;
	XMFLOAT3	m_xmf3Look;

	//�÷��̾ ���� x��, y��, z������ �󸶸�ŭ ȸ���ߴ°��� ��Ÿ����.
	float		m_fPitch;
	float		m_fYaw;
	float		m_fRoll;

	//�÷��̾��� �̵� �ӵ��� ��Ÿ���� �����̴�.
	XMFLOAT3	m_xmf3Velocity;
	//�÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� �����̴�.
	XMFLOAT3	m_xmf3Gravity;
	//�÷��̾ �ۿ��ϴ� �������� ��Ÿ����.
	float		m_fFriction;
};

void err_display(const char* msg);
void err_quit(const char* msg);