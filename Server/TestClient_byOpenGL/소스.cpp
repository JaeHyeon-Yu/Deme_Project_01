#include "function.h"
#include <iostream>
#include "../Server/define.h"
using namespace std;

void Log_In(const SOCKET& sock) {
	// id�� password�� ������ ������ Log-in�Ѵ�.
	LOGIN_STATE loginState{ OFF };
	int retval;

	while (loginState != OK) {
		string id, password;
		cout << endl << "[Log-in] id�� password�� �Է��Ͻʽÿ�." << endl;
		cout << "���ο� id �Է½� ȸ�����Ե˴ϴ�." << endl;
		cout << "===============================================" << endl;
		cout << "id : ";
		cin >> id;
		cout << "password : ";
		cin >> password;

		// id, password ����
		retval = send(sock, (char*)& id, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("send()");
		retval = send(sock, (char*)& password, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("send()");

		retval = recv(sock, (char*)& loginState, sizeof(loginState), 0);
		// ����� recv

		switch (loginState) {
		case OK:
			cout << "Success" << endl;
			break;
		case ERR:
			cout << "Error - �߸��� password�� �Է�" << endl;
			break;
		case NEW:
			cout << "������ �����Ǿ����ϴ�." << endl;
			break;
		}
	}
}