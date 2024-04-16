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
	COLORREF color;
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
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	srand((unsigned int)time(NULL));

	static COLORREF random_pen;
	static COLORREF random_brush;
	static SIZE size;
	static HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	static HBRUSH hBrush;
	static HPEN hPen;

	static PLAYER player[21];
	static int prev_move[21];
	int grid = WINDOW_X / GRID;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 50, NULL);
		InitPlayer(&player[0], grid);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			switch (player[0].rlud)
			{
			case 0:
				if (player[0].x == GRID - 1) {
					if (player[0].ud == 0) {
						if (player[0].y == GRID - 1) {
						player[0].rlud = 2;
						player[0].ud = 1;
						}
						else {
							player[0].y++;
							player[0].rlud = 2;
						}
					}
					else {
						if (player[0].y == 0) {
							player[0].rlud = 2;
							player[0].ud = 0;
						}
						else {
							player[0].y--;
							player[0].rlud = 2;
						}
					}
				}
				else { player[0].x++; }
				break;
			case 1:
				if (player[0].y == GRID - 1) {
					if (player[0].rl == 0) {
						player[0].x++;
						player[0].rlud = 3;
					}
					else {
						player[0].x--;
						player[0].rlud = 3;
					}
				}
				else { player[0].y++; }
				break;
			case 2:
				if (player[0].x == 0) {
					if (player[0].ud == 0) {
						player[0].y++;
						player[0].rlud = 0;
					}
					else {
						player[0].y--;
						player[0].rlud = 0;
					}
				}
				else { player[0].x--; }
				break;
			case 3:
				if (player[0].y == 0) {
					if (player[0].rl == 0) {
						player[0].x++;
						player[0].rlud = 1;
					}
					else {
						player[0].x--;
						player[0].rlud = 1;
					}
				}
				else { player[0].y--; }
				break;
			}
		}
		player[0].x1 = player[0].x * grid;
		player[0].y1 = player[0].y * grid;
		player[0].x2 = player[0].x1 + grid;
		player[0].y2 = player[0].y1 + grid;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'q':
			PostQuitMessage(1);
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

		hBrush = CreateSolidBrush(player[0].color);
		SelectObject(hDC, hBrush);
		DrawRectangle(hDC, player[0].x1, player[0].y1, player[0].x2, player[0].y2);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}