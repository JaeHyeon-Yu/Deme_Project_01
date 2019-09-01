#pragma comment(lib, "ws2_32")
#include "function.h"
#include "define.h"

int users{ 0 };
vector<Account> accInfo;

int main() {
	int retval;

	// ������ �ҷ�����
	ImportFile();

	// �÷��̾� ���� �ʱ�ȭ

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// iocp ����
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;

	// CPU ���� Ȯ��
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// ������ ����
	HANDLE hthread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; ++i) {
		hthread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hthread == NULL) return 1;
		CloseHandle(hthread);
	}

	// socket ����
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET) err_quit("WSASocket()");

	// bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)& serveraddr, sizeof(serveraddr));

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ��ſ� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen{ sizeof(clientaddr) };
	DWORD flags, recvBytes;
	unsigned int clientId{ 0 };

	while (true) {
		if (users >= MAX_USER) break;

		// Ŭ���̾�Ʈ id ����

		// accept
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		LOGIN_STATE loginState{ NONE };
		string id, password;
		while (loginState != OK) {
		int accIndex;
		ZeroMemory(&id, sizeof(string));
		ZeroMemory(&password, sizeof(string));

		retval = recv(client_sock, (char*)& id, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("recv");
		retval = recv(client_sock, (char*)& password, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("recv");

		for (int i = 0; i < accInfo.size(); ++i) {
			if (accInfo[i].id == id) {
				if (accInfo[i].password == password) {
					loginState = OK;
					accIndex = i;
					break;
				}
				loginState = ERR;
				break;
			}
		}

		if (loginState == NONE)
			loginState = NEW;

		switch (loginState) {
		case OK:
			// ���� ��ǥ�� �޾ƿ��� �ʱⰪ�� �������ش�.


			cout << "[Server] Client Access - " << id << "\t" << inet_ntoa(clientaddr.sin_addr) <<
				" : " << ntohs(clientaddr.sin_port) << endl;
			break;
		case ERR:
			break;
		case NEW:
			Account ptr;
			ptr.id = id;
			ptr.password = password;

			accInfo.emplace_back(ptr);
			break;
		}
		retval = send(client_sock, (char*)& loginState, sizeof(loginState), 0);
		if (retval == SOCKET_ERROR) err_display("send");
	}

		// ����
		retval = send(client_sock, (char*)& clientId, sizeof(clientId), 0);
		if (retval == SOCKET_ERROR) err_display("send");
		// retval = send(client_sock, (char*)& players, sizeof(players), 0);
		// if (retval == SOCKET_ERROR) err_display("send");
		// �÷��̾� ��ü ���� ������ (�迭)

		// socket - iocp ����
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		// ���� ���� ����ü �Ҵ�
		SOCKETINFO* ptr{ new SOCKETINFO };
		if (ptr == NULL) break;

		// ���� ���� ����ü �ʱ�ȭ
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recvBytes = ptr->sendBytes = 0;
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;

		// ����� ����
		flags = 0;
		retval = WSARecv(client_sock, &ptr->wsabuf, 1, &recvBytes, &flags, &ptr->overlapped, NULL);
		if (retval == SOCKET_ERROR) {
			if (WSAGetLastError() != ERROR_IO_PENDING) err_display("WSARecv()");
			continue;
		}
	}

	WSACleanup();
	return 0;
}
DWORD WINAPI WorkerThread(LPVOID arg) {
	HANDLE hcp = (HANDLE)arg;

	int retval;

	int clientid;
	char recvBuf[BUFSIZE];
	ZeroMemory(recvBuf, BUFSIZE);
	
	while (true) {
		DWORD recvBytes, flags{ 0 };
		DWORD cbTransferred;
		SOCKET client_sock;
		SOCKETINFO* ptr;

		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			(LPDWORD)& client_sock, (LPOVERLAPPED*)& ptr, INFINITE);

		// Ŭ������ ���
		SOCKADDR_IN clientaddr;
		int addrlen{ sizeof(clientaddr) };
		getpeername(ptr->sock, (SOCKADDR*)& clientaddr, &addrlen);

		if (retval == 0 || cbTransferred == 0) {
			if (retval == 0) {
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped, &temp1, FALSE, &temp2);
				err_display("WSAGetOverlappedResult()");
			}
			closesocket(ptr->sock);
			ClientDiscoonect(ptr->sock);
			cout << "[Server] Client ���� : IP�ּ� : " << inet_ntoa(clientaddr.sin_addr) <<
				"\t��Ʈ��ȣ : " << ntohs(clientaddr.sin_port) << endl;
			delete ptr;
			continue;
		}
		////
		// ������ ����
		retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvBytes, &flags, &ptr->overlapped, NULL);
		if (retval == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) err_display("WSARecv()");
		}

		// ������ �۽ź�
	}
}