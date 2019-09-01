#pragma once
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define BUFSIZE 512
using namespace std;
struct Account {
	string id;
	string password;
	// position
};
struct SOCKETINFO {
	OVERLAPPED overlapped;
	SOCKET sock;
	WSABUF wsabuf;
	int recvBytes, sendBytes;
	char buf[BUFSIZE];
};

void ImportFile();
void SaveFile();
DWORD WINAPI WorkerThread(LPVOID arg);
void ClientDiscoonect(const SOCKET& sock);
void Control(const int id, const int state);

