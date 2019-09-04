#pragma once
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "..//TestClient_byOpenGL/player.h"
#include "define.h"

#define BUFSIZE 512
using namespace std;
struct Account {
	string id;
	string password;
	int x, y, z;
};
struct SOCKETINFO {
	OVERLAPPED overlapped;
	SOCKET sock;
	WSABUF wsabuf;
	int recvBytes, sendBytes;
	char buf[BUFSIZE];
};

void ImportFile(vector<Account>& v);
void SaveFile(vector<Account>& v);
DWORD WINAPI WorkerThread(LPVOID arg);
void ClientDiscoonect(const SOCKET& sock);
void Control(const int& id, const PlayerState& state);
void Client_LogIn(const SOCKET& sock, const int& clientId);

void SetCPlayer(const Packet& p);