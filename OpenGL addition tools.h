#pragma once
#include "framework.h"
#include "Globals.h"
#define _USE_MATH_DEFINES
#include <math.h>

#define MAP_X 64
#define MAP_Y 64
#define ENEMY_COUNT 240

VOID	MapInit();
VOID	GameInit();
VOID	CameraApply();
VOID	WndResize(short x,short y);
VOID	CameraRotation(float x, float z);
VOID	PlayerMove();
VOID	GameMove();
VOID	EnemyInit();
VOID	EnemyDraw();
VOID	PlayerShoot();

BOOL showMask = FALSE;

float cubeVert[] = { 0,0,0,
	0,1,0,
	1,1,0,
	1,0,0,
	0,0,1,
	0,1,1,
	1,1,1,
	1,0,1 };

GLuint cubeInd[] = { 0,1,2,
	2,3,0,
	4,5,6,
	6,7,4,
	3,2,5,
	6,7,3,
	0,1,5,
	5,4,0,
	1,2,6,
	6,5,1,
	0,3,7,
	7,4,0};

struct colr
{
	float r, g, b;
};

struct cell
{
	colr clr;
};

struct
{
	float px, py, pz;
	float rx, rz;
} camera = {0,0,1.7,70,-40};

struct 
{
	float x, y, z;
	BOOL active;
} enemy[ENEMY_COUNT];

cell map[MAP_X][MAP_Y];

VOID MapInit()
{
	for (size_t i = 0; i < MAP_X; i++) 
	{
		for (size_t j = 0; j < MAP_Y; j++)
		{
			float delta = (rand() % 20) * 0.01f;
			map[i][j].clr.r = 0.31f + delta;
			map[i][j].clr.g = 0.5f + delta;
			map[i][j].clr.b = 0.13f + delta;
		}
	}
}

VOID GameInit()
{
	glEnable(GL_DEPTH_TEST);
	MapInit();
	EnemyInit();

	RECT rct;
	GetClientRect(hWnd, &rct);
	WndResize(rct.right, rct.bottom);
	ShowCursor(FALSE);
}

VOID CameraApply()
{
	glRotatef(-camera.rx, 1, 0, 0);
	glRotatef(-camera.rz,0,0,1);
	glTranslatef(-camera.px, -camera.py, -camera.pz);
}

VOID WndResize(short x, short y)
{
	glViewport(0, 0, x, y);
	float k = x / (float)y;
	float sz = 0.1;
	glLoadIdentity();
	glFrustum(-k * sz, k * sz, -sz, sz, sz * 2, 100);
}

VOID CameraRotation(float x, float z)
{
	camera.rz += z;
	if (camera.rz < 0) camera.rz += 360;
	if (camera.rz > 360) camera.rz -= 360;
	camera.rx += x;
	if (camera.rx < 0) camera.rx = 0;
	if (camera.rx > 180) camera.rx = 180;
}

VOID PlayerMove()
{
	if (GetForegroundWindow() != hWnd) return;
	
	float kut = -camera.rz / 180 * M_PI;
	float speed = 0;
	if (GetAsyncKeyState('W')) speed = 0.1;
	if (GetAsyncKeyState('S')) speed = -0.1;
	if (GetAsyncKeyState('A')) {
		speed = 0.1;
		kut -= M_PI * 0.5;
	}
	if (GetAsyncKeyState('D')) {
		speed = 0.1;
		kut += M_PI * 0.5;
	}
	if (speed != 0) {
		camera.px += sin(kut) * speed;
		camera.py += cos(kut) * speed;
	}

	POINT cur;
	static POINT sBase = { 1920 / 2, 1080/2};
	GetCursorPos(&cur);
	CameraRotation((sBase.y - cur.y) / 5.0f, (sBase.x - cur.x) / 5.0);
	SetCursorPos(sBase.x, sBase.y);
}

VOID GameMove()
{
	PlayerMove();
}

VOID EnemyInit()
{
	for (size_t i = 0; i < ENEMY_COUNT; i++)
	{
		enemy[i].active = TRUE;
		enemy[i].x = rand() % MAP_X;
		enemy[i].y = rand() % MAP_Y;
		enemy[i].z = rand() % 5;
	}
}

VOID EnemyDraw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	{
		glVertexPointer(3, GL_FLOAT, 0, cubeVert);
		for (size_t i = 0; i < ENEMY_COUNT; i++) 
		{
			if (!enemy[i].active) continue;

			glPushMatrix();
			{
				glTranslatef(enemy[i].x, enemy[i].y, enemy[i].z);

				if (showMask) 
					glColor3ub(255 - i, 0, 0);
				else
					glColor3ub(244, 60, 43);

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, cubeInd);
			}
			glPopMatrix();
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

VOID PlayerShoot()
{
	showMask = TRUE;
	Render();
	showMask = FALSE;

	RECT rct;
	GLubyte clr[3];
	GetClientRect(hWnd, &rct);
	glReadPixels(rct.right / 2.0, rct.bottom / 2.0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, clr);
	if (clr[0] > 0)
		enemy[255 - clr[0]].active = FALSE;

	PlaySound(L"shoot.wav", NULL, SND_ASYNC);
}
