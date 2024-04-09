#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_X 500
#define WINDOW_Y 500
#define GRID 20

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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 515, 538, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

typedef struct WORDS {
	char word[10];
};

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

void DrawRectangle(HDC hdc, int x1, int y1, int x2, int y2) {
	Rectangle(hdc, x1, y1, x2, y2);
}

void DrawCircle(HDC hdc, int x1, int y1, int x2, int y2) {
	Ellipse(hdc, x1, y1, x2, y2);
}

void DrawGrid(HDC hdc) {
	for (int i = 0; i < GRID; i++) {
		DrawLine(hdc, 0, i * (WINDOW_Y / GRID), WINDOW_X, i * (WINDOW_Y / GRID));
	}
	for (int j = 0; j < GRID; j++) {
		DrawLine(hdc, j * (WINDOW_X / GRID), 0, j * (WINDOW_X / GRID), WINDOW_Y);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	srand((unsigned int)time(NULL));

	static SIZE size;
	static HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	static HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0));
	static HPEN hPen;

	static int x = 0;
	static int y = 0;
	static int player_x1 = x * WINDOW_X / GRID;
	static int player_y1 = y * WINDOW_Y / GRID;
	static int player_x2 = player_x1 + WINDOW_X / GRID;
	static int player_y2 = player_y2 + WINDOW_Y / GRID;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		switch (wParam)
		{
		case 's':
			break;
		case 'q':
			PostQuitMessage(0);
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			if (x == 0) {
				x = GRID - 1;
			}
			else {
			x--;
			}
			break;
		case VK_RIGHT:
			if (x == GRID - 1) {
				x = 0;
			}
			else {
				x++;
			}
			break;
		case VK_UP:
			if (y == 0) {
				y = GRID - 1;
			}
			else {
				y--;
			}
			break;
		case VK_DOWN:
			if (y == GRID - 1) {
				y = 0;
			}
			else {
				y++;
			}
			break;
		}
		player_x1 = x * WINDOW_X / GRID;
		player_y1 = y * WINDOW_Y / GRID;
		player_x2 = player_x1 + WINDOW_X / GRID;
		player_y2 = player_y1 + WINDOW_Y / GRID;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC);

		SelectObject(hDC, hBrush);
		DrawRectangle(hDC, player_x1, player_y1, player_x2, player_y2);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER:
		hDC = GetDC(hWnd);
		ReleaseDC(hWnd, hDC);
		break;
	case WM_DESTROY:
		DeleteObject(hBrush);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}