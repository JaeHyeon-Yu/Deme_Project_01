#pragma comment(lib, "ws2_32")
#include "function.h"
#include "define.h"
#include "..//..//TestProject/Player.h"


void myIP();

int users{ 0 };
vector<Account>	accInfo;
SOCKETINFO	clients[MAX_USER];
Player	playerData[MAX_USER];

// directX Ŭ���̾�Ʈ �÷��̾� ����ü
CPlayer* cPlayers = new CPlayer[MAX_USER];

int main() {
	int retval;
	cout << sizeof(Packet) << endl;
	// ip ���
	myIP();

	// ������ �ҷ�����
	ImportFile(accInfo);

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
		for (int i = 0; i < MAX_USER; ++i)
			if (!playerData[i].isConnect) clientId = i;

		// accept
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		Client_LogIn(client_sock, clientId);

		// �ʱ� ������ ����
		retval = send(client_sock, (char*)& clientId, sizeof(clientId), 0);
		if (retval == SOCKET_ERROR) err_display("send");
		retval = send(client_sock, (char*)& playerData, sizeof(playerData), 0);
		if (retval == SOCKET_ERROR) err_display("send");

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
	// ���� ������ �Լ�
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
			reinterpret_cast<LPDWORD>(&client_sock), (LPOVERLAPPED*)& ptr, INFINITE);
		// 3��° ���� (LPDWORD)& client_sock�� ������ PC������ ������ �߻����� �ʴµ� ��Ͽ����� ������ �߻��Ѵ�.
		// x86������ ������ �߻����� ������ x64���� ���� �߻���
		// ���� x64������ ���������� ����ǵ��� �����Ұ�

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

			int accKey = playerData[clientid].accIndex;
			cout << "[Server] Client ���� : " << accInfo[accKey].id << endl;
			delete ptr;
			continue;
		}
		
		// ������ ����
		retval = WSARecv(ptr->sock, &ptr->wsabuf, 1, &recvBytes, &flags, &ptr->overlapped, NULL);
		if (retval == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) err_display("WSARecv()");
		}

		clientid = (int)ptr->wsabuf.buf[0];
		PlayerState state = (PlayerState)ptr->wsabuf.buf[1];
		Control(clientid, state);

		retval = send(clients[clientid].sock, (char*)& playerData, sizeof(playerData), 0);
		if (retval == SOCKET_ERROR) err_display("send()");
	}
}
void Client_LogIn(const SOCKET& sock, const int& clientId) {
	// Ŭ���̾�Ʈ���� id�� password�� �Է¹޾� �α����Ѵ�.
	LOGIN_STATE loginState{ OFF };
	string id, password;
	int retval;

	while (loginState != OK) {
		int accIndex;
		ZeroMemory(&id, sizeof(string));
		ZeroMemory(&password, sizeof(string));

		retval = recv(sock, (char*)& id, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("recv");
		retval = recv(sock, (char*)& password, sizeof(string), 0);
		if (retval == SOCKET_ERROR) err_display("recv");

		for (int i = 0; i < accInfo.size(); ++i)
			if (accInfo[i].id == id) {
				if (accInfo[i].password == password) {
					loginState = OK;
					accIndex = i;
				}
				else loginState = ERR;
				break;
			}
		if (loginState == OFF)
			loginState = NEW;

		switch (loginState) {
		case OK:
			// ���� ��ǥ�� �޾ƿ��� �ʱⰪ�� �������ش�.
			playerData[clientId].x = accInfo[accIndex].x;
			playerData[clientId].y = accInfo[accIndex].y;
			playerData[clientId].z = accInfo[accIndex].z;
			playerData[clientId].accIndex = accIndex;
			playerData[clientId].state = IDLE;
			playerData[clientId].isConnect = true;
			clients[clientId].sock = sock;
			cout << "[Server] Client Access - " << id << endl;
			break;
		case ERR:
			break;
		case NEW:
			Account ptr;
			ptr.id = id;
			ptr.password = password;
			ptr.x = ptr.y = ptr.z = 0;
			accInfo.emplace_back(ptr);
			break;
		}
		retval = send(sock, (char*)& loginState, sizeof(loginState), 0);
		if (retval == SOCKET_ERROR) err_display("send");
	}
}
void ClientDiscoonect(const SOCKET& sock) {
	// Client�� ������ ���� �� �ڸ��� �ʱ�ȭ
	int key;
	for (int i = 0; i < MAX_USER; ++i)
		if (sock == clients[i].sock) {
			key = i;
			break;
		}
	int acckey = playerData[key].accIndex;
	accInfo[acckey].x = playerData[key].x;
	accInfo[acckey].y = playerData[key].y;
	accInfo[acckey].z = playerData[key].z;

	playerData[key].isConnect = false;
	playerData[key].state = NONE;
	playerData[key].x = 0;
	playerData[key].y = 0;
	playerData[key].z = 0;
	users--;
	SaveFile(accInfo);
}
void Control(const int& id, const PlayerState& state) {
	// �÷��̾ �̵���Ŵ
	switch (state) {
	case MOVE_FRONT:
		playerData[id].y += 10;
		break;
	case MOVE_BACK:
		playerData[id].y -= 10;
		break;
	case MOVE_RIGHT:
		playerData[id].x += 10;
		break;
	case MOVE_LEFT:
		playerData[id].x -= 10;
		break;
	default:
		break;
	}
}
void myIP() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	PHOSTENT hostinfo;
	char hostname[50];
	char ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int nError = gethostname(hostname, sizeof(hostname));
	if (nError == 0){
		hostinfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[0]));
	}
	WSACleanup();
	printf("This Computer's IP address : %s\n", ipaddr);
}
void SetCPlayer(const Packet& p) {
	// ���۹��� �����͸� CPlayer ��ü�� �����Ѵ�.
	short clientId{ p.id };

	cPlayers[clientId].SetPosition(p.m_xmf3Position);
	
}