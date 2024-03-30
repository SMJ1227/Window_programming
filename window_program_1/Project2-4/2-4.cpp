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

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;
	static TCHAR str[100];
	static SIZE size;

	srand((unsigned int)time(NULL));

	static int x;
	static int y;
	static int n;
	static int m;
	static int count = 0;
	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		if (wParam == 'q') { PostQuitMessage(0); }
		else if (wParam == VK_RETURN) {
			int parsed = _stscanf(str, _T("%d %d %d %d"), &x, &y, &n, &m);
			if (n < 0 || n > 19) {
				count = 0;
				memset(str, 0, sizeof(str));
			}
			else if (m < 1 || m > 30) {
				count = 0;
				memset(str, 0, sizeof(str));
			}
			if (parsed == 4) {
				count = 0;
				memset(str, 0, sizeof(str));
			}
		}
		else {
			str[count++] = wParam;
			str[count] = '\0';
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetTextExtentPoint32(hDC, str, lstrlen(str), &size);
		TextOut(hDC, 0, 0, str, lstrlen(str));
		SetCaretPos(size.cx, 0);
		for (int i = 1; i <= m; i++) {
			TCHAR multiplicationStr[100];
			wsprintf(multiplicationStr, L"%d * %d = %d", n, i, n * i);
			TextOut(hDC, x, y, multiplicationStr, lstrlen(multiplicationStr));
			y += 20;
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