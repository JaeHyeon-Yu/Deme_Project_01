#include "communication.h"
#include "..//Server/Server/define.h"
#include "Player.h"
#include <iostream>
using namespace std;

void SetPacket(const CPlayer& p);

void SetSocket(SOCKET& sock) {
	// 소켓을 설정하고 Server와 연결해준다.
	
	int retval;
	char serverip[32];
	cout << "Input Server's IP : ";
	cin >> serverip;

	// 윈속 초기화
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

	// Log-in 넣을 부분

	// 초기 데이터 전송해줄 부분
	// 일단 CPlayer 객체를 전송받는다.
	CPlayer* temp = new CPlayer[MAX_USER];
	retval = recv(sock, (char*)& temp, sizeof(temp), 0);
	if (retval == SOCKET_ERROR) err_display("send");
}
void SendPacket(const SOCKET& sock) {
	// 서버에 보낼 패킷 데이터를 만들어서 보내준다.
	int clientid = 0;
	int retval;

	Packet* pack = new Packet;
	pack->id = clientid;
	// 패킷 저장하기...
	
	// 패킷 전송
	retval = send(sock, (char*)& pack, sizeof(pack), 0);
	if (retval == SOCKET_ERROR) err_display("send");
	delete pack;
}
void RecvPacket(const SOCKET& sock) {
	// 서버에서 패킷 데이터를 받아온다.
	int retval;

	Packet* pack = new Packet;
	retval = recv(sock, (char*)& pack, sizeof(pack), 0);
	if (retval == SOCKET_ERROR) err_display("recv");

	// 받은 패킷으로 데이터를 처리해준다.

	delete pack;
}
void SetPacket(Packet& pack, const CPlayer& p) {
	// 패킷을 사용자 데이터로 설정해준다.
}