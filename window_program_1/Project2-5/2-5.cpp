#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

# define MAX_X 31
# define MAX_Y 10

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;
	static TCHAR str[MAX_Y][MAX_X];
	static SIZE size;

	srand((unsigned int)time(NULL));

	static int y = 0;
	static int count = 0;
	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		if (wParam == VK_ESCAPE) { PostQuitMessage(0); }
		else if (wParam == VK_RETURN) {
			y = (y + 1) % MAX_Y;
			count = 0;
		}
		else if (wParam == VK_BACK) {
			if (count < 1 && y < 1) {
				count = 1;
			}
			if (count < 1 && y > 0) {
				y = (y - 1) % MAX_Y;
				count = lstrlen(str[y]);
				str[y][count++] = '\0';
			}
			else {
				str[y][--count] = '\0';
			}
		}
		else {
			if (count < 30) {
				str[y][count++] = wParam;
			}
			else {
				y = (y + 1) % MAX_Y;
				count = 0;
				str[y][count++] = wParam;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		for (int i = 0; i < MAX_Y; i++) {
			GetTextExtentPoint32(hDC, str[i], count, &size);
			TextOut(hDC, 0, i*20, str[i], _tcslen(str[i]));
			if (i == y) {
				SetCaretPos(size.cx, i * 20);
			}	
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		hDC = GetDC(hWnd);
		ReleaseDC(hWnd, hDC);
		break;
	case WM_DESTROY:
		HideCaret(hWnd);
		DestroyCaret();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}