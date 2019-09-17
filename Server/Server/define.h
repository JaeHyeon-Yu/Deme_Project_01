#pragma once
#include <string>
#define SERVERPORT 9000
#define MAX_USER 2


enum LOGIN_STATE {
	OFF, OK, ERR, NEW
};


void err_display(const char* msg);
void err_quit(const char* msg);