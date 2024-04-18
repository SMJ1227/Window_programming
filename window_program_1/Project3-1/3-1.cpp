#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define M_PI 3.14
#define WINDOW_X 500
#define WINDOW_Y 500
#define MAX 10
#define COLORS 4
#define GRID 40


HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_HAND);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_QUESTION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 495, 518, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

typedef struct PLAYER {
	int x;
	int y;
	int x1;
	int y1;
	int x2;
	int y2;
	int rl;
	int ud;
	int rlud;
	int is_move;
	COLORREF color;
};

typedef struct BRICK {
	int x;
	int y;
	int x1;
	int y1;
	int x2;
	int y2;
};

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

void DrawRectangle(HDC hdc, int x1, int y1, int x2, int y2) {
	Rectangle(hdc, x1, y1, x2, y2);
}

void DrawTriangle(HDC hdc, int x1, int y1, int x2, int y2) {
	POINT points[3] = { {(x1 + x2) / 2, y1}, {x1, y2}, {x2, y2} };
	Polygon(hdc, points, 3);
}

void DrawCircle(HDC hdc, int x1, int y1, int x2, int y2) {
	Ellipse(hdc, x1, y1, x2, y2);
}

void DrawGrid(HDC hdc, int grid) {
	for (double i = 0; i < grid; i++) {
		DrawLine(hdc, 0, i * (WINDOW_Y / grid), WINDOW_X, i * (WINDOW_Y / grid));
	}
	for (double j = 0; j < grid; j++) {
		DrawLine(hdc, j * (WINDOW_X / grid), 0, j * (WINDOW_X / grid), WINDOW_Y);
	}
}
void InitBRICK(BRICK *brick, int mouse_x, int mouse_y, int brick_num, int grid) {
	brick[brick_num].x = mouse_x / grid;
	brick[brick_num].y = mouse_y / grid;
	brick[brick_num].x1 = brick[brick_num].x * grid;
	brick[brick_num].y1 = brick[brick_num].y * grid;
	brick[brick_num].x2 = brick[brick_num].x1 + grid;
	brick[brick_num].y2 = brick[brick_num].y1 + grid;
}

void InitPlayer(PLAYER *player, int grid) {
	player->x = 0;
	player->y = 0;
	player->x1 = player->x * grid;
	player->y1 = player->y * grid;
	player->x2 = player->x1 + grid;
	player->y2 = player->y1 + grid;
	player->color = RGB(255, 0, 0);
	player->rl = 0;	// rl = 0 오른쪽 rl = 1 왼쪽
	player->ud = 0;	// ud = 0 아래쪽 ud = 1위쪽
	player->is_move = 1;
}

void initStones(PLAYER* player, int grid) {
	player->x = rand() % GRID;
	player->y = rand() % GRID;
	player->x1 = player->x * grid;
	player->y1 = player->y * grid;
	player->x2 = player->x1 + grid;
	player->y2 = player->y1 + grid;
	player->color = RGB(rand() % 256 , rand() % 256, rand() % 256);
	player->rl = rand() % 2;	// rl = 0 오른쪽 rl = 1 왼쪽
	player->ud = rand() % 2;	// ud = 0 아래쪽 ud = 1위쪽
	player->rlud = rand() % 4;
	player->is_move = 0;
}

void move_to_mouse(PLAYER *player, int mouse_x, int mouse_y) {
	if (player[0].x1 > mouse_x) {
		player[0].rlud = 2;
	}
	else if (player[0].x1 < mouse_x) {
		player[0].rlud = 0;
	}
	if (player[0].y1 > mouse_y) {
		player[0].ud = 1;
	}
	else if (player[0].y1 < mouse_y) {
		player[0].ud = 0;
	}
}

void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	srand((unsigned int)time(NULL));

	COLORREF brick_color = RGB(0,0,0);
	HBRUSH brickBrush = CreateSolidBrush(brick_color);
	HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	static SIZE size;
	static HBRUSH hBrush;

	static BRICK brick[21];
	static int brick_num = 0;

	static PLAYER player[21];
	static int prev_move[21];
	static int tail[21];
	tail[0] = 0;
	static int tail_length = 1;
	
	int grid = WINDOW_X / GRID;
	static int speed = 150;

	static int l_mouse_x;
	static int l_mouse_y;
	static int smaller = 0;

	static int r_mouse_x;
	static int r_mouse_y;
	static int foundPlayer = 0;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		SetTimer(hWnd, 1, speed, NULL);
		InitPlayer(&player[0], grid);
		for (int i = 1; i < 21; i++) {
			initStones(&player[i], grid);
		}
		break;

	case WM_LBUTTONDOWN:
		l_mouse_x = LOWORD(IParam);
		l_mouse_y = HIWORD(IParam);
		for (int i = 0; i < 21; i++) {
			if (player[i].x1 <= l_mouse_x && player[i].x2 >= l_mouse_x) {	// x좌표 일치
				if (player[i].y1 <= l_mouse_y && player[i].y2 >= l_mouse_y) {	// y좌표 일치
					if (i == 0) {	// 주인공이면
						smaller = 1;
					}
				}
			}
			else {
				move_to_mouse(player, l_mouse_x, l_mouse_y);
			}
		}
		break;

	case WM_RBUTTONDOWN:
		r_mouse_x = LOWORD(IParam);
		r_mouse_y = HIWORD(IParam);
		for (int i = 0; i < 21; i++) {
			if (player[i].x1 <= r_mouse_x && player[i].x2 >= r_mouse_x) {	// x좌표 일치
				if (player[i].y1 <= r_mouse_y && player[i].y2 >= r_mouse_y) {	// y좌표 일치
					foundPlayer = 1;
					break;
				}
			}
		}
		if (!foundPlayer) { 
			InitBRICK(brick, r_mouse_x, r_mouse_y, brick_num, grid);
			brick_num++;
		}
		break;

	case WM_TIMER:
		for (int a = 0; a < 21; a++) {
			if (player[a].is_move == 1) {
				switch (player[a].rlud)
				{
				case 0:	// 오른쪽
					if (player[a].x == GRID - 1) {	// 오른쪽 벽에 닿으면
						if (player[a].ud == 0) {	// 아래 방향
							if (player[a].y == GRID - 1) {	// 마지막줄이면 위쪽 방향 왼쪽 이동
								player[a].rlud = 2;
								player[a].ud = 1;
							}
							else {	// 아래줄 왼쪽 이동
								player[a].y++;
								player[a].rlud = 2;
							}
						}
						else {	// 윗방향
							if (player[a].y == 0) {	// 첫째줄이면 아래 방향 왼쪽 이동
								player[a].rlud = 2;
								player[a].ud = 0;
							}
							else {	// 윗줄 왼쪽 이동
								player[a].y--;
								player[a].rlud = 2;
							}
						}
					}
					else { player[a].x++; }	// 오른쪽으로
					break;
				case 1:	// 아래 이동
					if (player[a].y == GRID - 1) {	// 맨 아래 줄이면
						if (player[a].rl == 0) {	// 오른쪽 아래로 진행중
							if (player[a].x == GRID - 1) {	// 마지막 칸이면 왼쪽 위로
								player[a].rlud = 3;
								player[a].rl = 1;

							}
							else {
								player[a].x++;
								player[a].rlud = 3;
							}
						}
						else {	// 왼쪽 아래로 진행중
							if (player[a].x == 0) {
								player[a].rlud = 3;
								player[a].rl = 0;
							}
							else {
								player[a].x--;
								player[a].rlud = 3;
							}
						}
					}
					else { player[a].y++; }	// 아래로 이동
					break;
				case 2:	// 왼쪽
					if (player[a].x == 0) {	// 왼쪽 벽
						if (player[a].ud == 0) {	// 아래로
							if (player[a].y == GRID - 1) {	// 마지막줄이면 오른쪽 위
								player[a].rlud = 0;
								player[a].ud = 1;
							}
							else {
								player[a].y++;
								player[a].rlud = 0;
							}

						}
						else {	// 위로
							if (player[a].y == 0) {	// 첫줄
								player[a].rlud = 0;
								player[a].ud = 0;
							}
							else {
								player[a].y--;
								player[a].rlud = 0;
							}
						}
					}
					else { player[a].x--; }
					break;
				case 3:	// 위로
					if (player[a].y == 0) {	// 윗 벽
						if (player[a].rl == 0) {	// 오른쪽
							if (player[a].x == GRID - 1) {	// 오른쪽 끝
								player[a].rlud = 1;
								player[a].rl = 1;
							}
							else {
								player[a].x++;
								player[a].rlud = 1;
							}
						}
						else {	// 왼쪽
							if (player[a].x == 0) {
								player[a].rlud = 1;
								player[a].rl = 0;
							}
							else {
								player[a].x--;
								player[a].rlud = 1;
							}
						}
					}
					else { player[a].y--; }
					break;
				}
			}
		}
		// 충돌체크
		for (int i = 0; i < brick_num; i++) {	// player와 brick의 충돌
			for (int j = 0; j < 21; j++) {
				if (brick[i].x == player[j].x && brick[i].y == player[j].y) {
					switch (wParam)
					{
					case 1:
						switch (player[j].rlud)
						{
						case 0:	// 오른쪽
							player[j].x--;
							player[j].rlud = 2;
							break;
						case 1:	// 아래
							player[j].y--;
							player[j].rlud = 3;
							break;
						case 2:	// 왼쪽
							player[j].x++;
							player[j].rlud = 0;
							break;
						case 3:	// 위로
							player[j].y++;
							player[j].rlud = 1;
							break;
						}
					}
				}
			}
		}
		// player[0]과 player충돌
		for (int i = 1; i < 21; i++) {
			if (player[i].x == player[0].x && player[i].y == player[0].y) {
				if (player[i].is_move == 0) {	// 정지 상태면
					player[i].is_move = 1;
				}
				else if (player[i].is_move == 1) {
					tail[tail_length] = i;
					tail_length++;
				}
			}
		}
		// player와 player의 충돌
		for (int i = 1; i < 21; i++) {
			for (int j = 1; j < 21; j++) {
				if(i == j){continue;}
				if (player[i].x == player[j].x && player[i].y == player[j].y) {
					
				}
			}
		}
		for (int b = 0; b < 21; b++) {
			player[b].x1 = player[b].x * grid;
			player[b].y1 = player[b].y * grid;
			player[b].x2 = player[b].x1 + grid;
			player[b].y2 = player[b].y1 + grid;
		}
		
		
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'q':
			PostQuitMessage(1);
			break;
		case '+':
		case '=':
			if (speed < 20) {
				break;
			}
			speed = speed - 10;
			SetTimer(hWnd, 1, speed, NULL);
			break;
		case '-':
		case '_':
			if (speed > 100) {
				break;
			}
			speed = speed + 10;
			SetTimer(hWnd, 1, speed, NULL);
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RIGHT:
			player[0].rlud = 0;
			player[0].rl = 0;
			break;
		case VK_DOWN:
			player[0].rlud = 1;
			player[0].ud = 0;
			break;
		case VK_LEFT:
			player[0].rlud = 2;
			player[0].rl = 1;
			break;
		case VK_UP:
			player[0].rlud = 3;
			player[0].ud = 1;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC, GRID);

		for (int i = 0; i < brick_num; i++) {
			SelectObject(hDC, brickBrush);
			DrawRectangle(hDC, brick[i].x1, brick[i].y1, brick[i].x2, brick[i].y2);
			DeleteObject(brickBrush);
		}

		for (int i = 0; i < 21; i++) {
			if (player[i].is_move == 0) {	// 가만히
				hBrush = CreateSolidBrush(player[i].color);
				SelectObject(hDC, hBrush);
				DrawRectangle(hDC, player[i].x1, player[i].y1, player[i].x2, player[i].y2);
				DeleteObject(hBrush);
			}
			else {
				if (smaller == 0) {
					hBrush = CreateSolidBrush(player[i].color);
					SelectObject(hDC, hBrush);
					DrawCircle(hDC, player[i].x1, player[i].y1, player[i].x2, player[i].y2);
					DeleteObject(hBrush);
				}
				else if(smaller >= 1){
					hBrush = CreateSolidBrush(player[i].color);
					SelectObject(hDC, hBrush);
					DrawCircle(hDC, player[i].x1, player[i].y1, player[i].x2 - 5, player[i].y2 - 5);
					DeleteObject(hBrush);
					smaller++;
					if (smaller > 50) {
						smaller = 0;
					}
				}
			}
		}
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}