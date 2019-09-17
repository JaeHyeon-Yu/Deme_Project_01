#pragma comment(lib, "ws2_32")
#include <iostream>
#include <gl/freeglut.h>
#include "player.h"
#include "function.h"
#include "../Server/define.h"
using namespace std;
SOCKET sock;
Player playerData[MAX_USER];
unsigned clientid;

// OpenGL 함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void SendTimer(int value);
void RecvTimer(int value);

int main(int argc, char* argv[]) {
	int retval;
	string ip;
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 0;

	// WSASocket
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock == INVALID_SOCKET) err_quit("WSASocket()");

	char serverip[32];
	cout << "Input Server's IP : ";
	cin >> serverip;

	// connect
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(serverip);
	server_addr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)& server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR) err_display("connect()");

	Log_In(sock);

	//  초기 데이터 recv
	retval = recv(sock, (char*)& clientid, sizeof(clientid), 0);
	if (retval == SOCKET_ERROR) err_display("recv");
	retval = recv(sock, (char*)& playerData, sizeof(playerData), 0);
	if (retval == SOCKET_ERROR) err_display("recv");

	//초기화 함수들
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치지정
	glutInitWindowSize(800, 800); // 윈도우의 크기 지정
	glutCreateWindow("Test Client"); // 윈도우 생성 (윈도우 이름)
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수의 지정
	glutKeyboardFunc(Keyboard);		// 키보드 입력
	glutTimerFunc(100, SendTimer, 1);
	glutTimerFunc(100, RecvTimer, 2);
	glutMainLoop();
}
GLvoid drawScene(GLvoid) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 바탕색을 'blue' 로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기

	for (const Player& p : playerData) {
		if (!p.isConnect) continue;
		glRecti(p.x - 10, p.y - 10, p.x + 10, p.y + 10);
	}

	glFlush(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glOrtho(-400, 400, -400, 400, -1, 1);
}
void SendTimer(int value) {
	// 송신 타이머
	int retval;

	// id index와 state만 패킷에 담아 전송
	char pack[2];
	pack[0] = clientid;
	pack[1] = playerData[clientid].state;

	retval = send(sock, pack, sizeof(pack), 0);
	if (retval == SOCKET_ERROR) err_display("send");

	glutPostRedisplay();
	glutTimerFunc(100, SendTimer, 1);
}
void RecvTimer(int value) {
	// 수신 타이머
	// 플레이어 배열 전체를 수신
	int retval = recv(sock, (char*)playerData, sizeof(playerData), 0);
	if (retval == SOCKET_ERROR) err_display("recv()");

	glutPostRedisplay();
	glutTimerFunc(100, RecvTimer, 2);
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		playerData[clientid].state = MOVE_FRONT;
		break;
	case 'a':
		playerData[clientid].state = MOVE_LEFT;
		break;
	case 's':
		playerData[clientid].state = MOVE_BACK;
		break;
	case 'd':
		playerData[clientid].state = MOVE_RIGHT;
		break;
	}
	glutPostRedisplay();	// 화면 재출력!
}
void err_quit(const char* msg) {
	// 소켓함수 오류 출력 후 종료
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(const char* msg) {
	// 소켓함수 오류 출력
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	std::cout << msg << (char*)lpMsgBuf;
	LocalFree(lpMsgBuf);
}
