#include "communication.h"
#include <iostream>
using namespace std;
void err_quit(const char* msg);
void err_display(const char* msg);

unsigned short clientId{};

void SetSocket(SOCKET& sock) {
	// 소켓을 설정하고 Server와 연결해준다.
	
	int retval;
	// char serverip[32];
	// cout << "Input Server's IP : ";
	// cin >> serverip;

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
	server_addr.sin_addr.s_addr = inet_addr(우리집);
	server_addr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)& server_addr, sizeof(server_addr));	
	if (retval == SOCKET_ERROR) err_display("connect");

	// Log-in 넣을 부분

	// 초기 데이터 전송해줄 부분
	// 일단 CPlayer 객체를 전송받는다.
	// string id = "yujae";
	// string password = "1234";
	// send(sock, (char*)& id, sizeof(id), 0);
	// send(sock, (char*)& password, sizeof(password), 0);

	// Client ID 수신
	// retval = recv(sock, (char*)& clientId, sizeof(clientId), 0);
	// if (retval == SOCKET_ERROR) err_display("recv");

	// CPlayer* temp = new CPlayer[MAX_USER];
	// retval = recv(sock, (char*)& temp, sizeof(temp), 0);
	// if (retval == SOCKET_ERROR) err_display("send");
}
void SendPacket(const SOCKET& sock, CPlayer *cp) {
	// 서버에 보낼 패킷 데이터를 만들어서 보내준다.
	int retval;
	
	// 플레이어의 위치 정보를 담은 패킷을 만든다.
	CS_MOVE_PACKET *pack = new CS_MOVE_PACKET;
	pack->size = sizeof(CS_MOVE_PACKET);
	pack->type = MOVE_POS;
	pack->id = clientId;
	pack->position = cp->GetPosition();
	// 패킷 전송
	retval = send(sock, (char*)& pack, pack->size, 0);
	if (retval == SOCKET_ERROR) err_display("send");
}
void RecvPacket(const SOCKET& sock, CPlayer*& cp) {
	// 서버에서 패킷 데이터를 받아온다.
	int retval;
	char buf[512];

	for (int i = 0; i < MAX_USER; ++i) {
		ZeroMemory(buf, sizeof(buf));
		retval = recv(sock, buf, sizeof(buf), 0);
		if (retval == SOCKET_ERROR) err_display("recv");
		PacketProcess(buf, cp);
	}
}
void PacketProcess(char* buf, CPlayer*& cp){
	// 패킷 관리
	buf[(int)buf[0]] = '\0';
	int packetType = (int)buf[1];

	switch (packetType) {		
	case MOVE_POS:
		SC_MOVE_PACKET* scMove = reinterpret_cast<SC_MOVE_PACKET*>(buf);
		cp->SetPosition(scMove->position);
		break;
	}
}
void err_quit(const char* msg) {
}
void err_display(const char* msg) {

}