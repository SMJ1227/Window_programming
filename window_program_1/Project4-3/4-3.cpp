#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_X 300
#define WINDOW_Y 600
#define GRID 50
#define GRID_X 6
#define GRID_Y 12
#define M_PI 3.14

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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 315, 640, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

typedef struct STONES {
	int x;
	int y;
	int x1;
	int y1;
	int x2;
	int y2;
	COLORREF color;
};

typedef struct GRIDS {
	int x;
	int y;
	COLORREF color;
	BOOL disable = FALSE;
};
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

void DrawGrid(HDC hdc) {
	for (double i = 0; i < GRID_Y; i++) {
		DrawLine(hdc, 0, i * GRID, WINDOW_X, i * GRID);
	}
	for (double j = 0; j < GRID_X; j++) {
		DrawLine(hdc, j * GRID, 0, j * GRID, WINDOW_Y);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	srand((unsigned int)time(NULL));

		//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_COMMAND: //--- 메뉴를 선택했을 때
		switch (LOWORD(wParam)) {
		case ID_GAME_START:
			break;
		case ID_GAME_END:
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}