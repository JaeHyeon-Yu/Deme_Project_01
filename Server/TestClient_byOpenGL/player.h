#pragma once

enum PlayerState{
	NONE, IDLE, MOVE_FRONT, MOVE_BACK,
	MOVE_RIGHT, MOVE_LEFT
};

struct Player {
	PlayerState state;
	int x, y, z;
	int accIndex;		// Account vectorÀÇ Index°ª
	bool isConnect;
};