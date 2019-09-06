#pragma once
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <string>
#define 석준이집 "192.168.200.128"

void SetSocket(SOCKET& sock);
void SendPacket(const SOCKET& sock);
void RecvPacket(const SOCKET& sock);