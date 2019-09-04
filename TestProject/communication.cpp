#include "communication.h"
#include "..//Server/Server/define.h"
#include "Player.h"
#include <iostream>
using namespace std;

void SetPacket(const CPlayer& p);

void SetSocket(SOCKET& sock) {
	// ������ �����ϰ� Server�� �������ش�.
	
	int retval;
	char serverip[32];
	cout << "Input Server's IP : ";
	cin >> serverip;

	// ���� �ʱ�ȭ
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;

	// WSASocket
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock == INVALID_SOCKET) err_quit("WSASocket");

	// connect
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(serverip);
	server_addr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)& server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR) err_display("connect");

	// Log-in ���� �κ�

	// �ʱ� ������ �������� �κ�
	// �ϴ� CPlayer ��ü�� ���۹޴´�.
	CPlayer* temp = new CPlayer[MAX_USER];
	retval = recv(sock, (char*)& temp, sizeof(temp), 0);
	if (retval == SOCKET_ERROR) err_display("send");
}
void SendPacket(const SOCKET& sock) {
	// ������ ���� ��Ŷ �����͸� ���� �����ش�.
	int clientid = 0;
	int retval;

	Packet* pack = new Packet;
	pack->id = clientid;
	// ��Ŷ �����ϱ�...
	
	// ��Ŷ ����
	retval = send(sock, (char*)& pack, sizeof(pack), 0);
	if (retval == SOCKET_ERROR) err_display("send");
	delete pack;
}
void RecvPacket(const SOCKET& sock) {
	// �������� ��Ŷ �����͸� �޾ƿ´�.
	int retval;

	Packet* pack = new Packet;
	retval = recv(sock, (char*)& pack, sizeof(pack), 0);
	if (retval == SOCKET_ERROR) err_display("recv");

	// ���� ��Ŷ���� �����͸� ó�����ش�.

	delete pack;
}
void SetPacket(Packet& pack, const CPlayer& p) {
	// ��Ŷ�� ����� �����ͷ� �������ش�.
}