#pragma once
#include <WinSock2.h>
void SetSocket(SOCKET& sock);
void SendPacket(const SOCKET& sock);
void RecvPacket(const SOCKET& sock);