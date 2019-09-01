#pragma once
#include <string>

#define SERVERPORT 9000
#define MAX_USER 10


enum LOGIN_STATE {
	NONE, OK, ERR, NEW
};

struct Account {
	std::string id;
	std::string password;
	// ÁÂÇ¥
};

void err_display(const char* msg);
void err_quit(const char* msg);