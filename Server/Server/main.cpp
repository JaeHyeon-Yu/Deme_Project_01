#pragma comment(lib, "ws2_32")
#include "function.h"
#include "define.h"
#include "..//..//TestProject/Player.h"


void myIP();

int users{ 0 };
vector<Account>	accInfo;
SOCKETINFO	clients[MAX_USER];
Player	playerData[MAX_USER];

// directX 클라이언트 플레이어 구조체
CPlayer* cPlayers = new CPlayer[MAX_USER];

int main() {
	int retval;
	cout << sizeof(Packet) << endl;
	// ip 출력
	myIP();

	// 데이터 불러오기
	ImportFile(accInfo);

	// 플레이어 정보 초기화

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// iocp 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;

	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// 스레드 생성
	HANDLE hthread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; ++i) {
		hthread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hthread == NULL) return 1;
		CloseHandle(hthread);
	}

	// socket 생성
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

	// 통신용 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen{ sizeof(clientaddr) };
	DWORD flags, recvBytes;
	unsigned int clientId{ 0 };

	while (true) {
		if (users >= MAX_USER) break;

		// 클라이언트 id 배정
		for (int i = 0; i < MAX_USER; ++i)
			if (!playerData[i].isConnect) clientId = i;

		// accept
		client_sock = accept(listen_sock, (SOCKADDR*)& clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		Client_LogIn(client_sock, clientId);

		// 초기 데이터 전송
		retval = send(client_sock, (char*)& clientId, sizeof(clientId), 0);
		if (retval == SOCKET_ERROR) err_display("send");
		retval = send(client_sock, (char*)& playerData, sizeof(playerData), 0);
		if (retval == SOCKET_ERROR) err_display("send");

		// socket - iocp 연결
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

		// 소켓 정보 구조체 할당
		SOCKETINFO* ptr{ new SOCKETINFO };
		if (ptr == NULL) break;

		// 소켓 정보 구조체 초기화
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recvBytes = ptr->sendBytes = 0;
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;

		// 입출력 시작
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
	// 소켓 스레드 함수
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
		// 3번째 인자 (LPDWORD)& client_sock를 넣으면 PC에서는 에러가 발생하지 않는데 놋북에서는 에러가 발생한다.
		// x86에서는 에러가 발생하지 않으나 x64에서 에러 발생함
		// 추후 x64에서도 정상적으로 실행되도록 수정할것

		// 클라정보 얻기
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
			cout << "[Server] Client 종료 : " << accInfo[accKey].id << endl;
			delete ptr;
			continue;
		}
		
		// 데이터 수신
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
	// 클라이언트에서 id와 password를 입력받아 로그인한다.
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
			// 기존 좌표를 받아오고 초기값을 설정해준다.
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
	// Client의 접속을 끊고 그 자리를 초기화
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
	// 플레이어를 이동시킴
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
	// 전송받은 데이터를 CPlayer 객체에 삽입한다.
	short clientId{ p.id };

	cPlayers[clientId].SetPosition(p.m_xmf3Position);
	
}