#include "function.h"
#include <iostream>
#include "../Server/define.h"
using namespace std;

void Log_In(const SOCKET& sock) {
	// id와 password를 서버로 전송해 Log-in한다.
	LOGIN_STATE loginState{ OFF };
	int retval;

	while (loginState != OK) {
		string id, password;
		cout << endl << "[Log-in] id와 password를 입력하십시오." << endl;
		cout << "새로운 id 입력시 회원가입됩니다." << endl;
		cout << "===============================================" << endl;
		cout << "id : ";
		cin >> id;
		cout << "password : ";
		cin >> password;

		// id, password 전송
		retval = send(sock, (char*)& id, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("send()");
		retval = send(sock, (char*)& password, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("send()");

		retval = recv(sock, (char*)& loginState, sizeof(loginState), 0);
		// 결과값 recv

		switch (loginState) {
		case OK:
			cout << "Success" << endl;
			break;
		case ERR:
			cout << "Error - 잘못된 password를 입력" << endl;
			break;
		case NEW:
			cout << "계정이 생성되었습니다." << endl;
			break;
		}
	}
}