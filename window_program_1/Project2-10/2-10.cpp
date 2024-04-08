#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1000, 1000, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

#define M_PI 3.14

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

void DrawRectangle(HDC hdc, int x1, int y1, int x2, int y2) {
	Rectangle(hdc, x1, y1, x2, y2);
}

void DrawTriangle(HDC hdc, int x1, int y1, int x2, int y2) {
	POINT points[3] = { {(x1+x2)/2, y1}, {x1, y2}, {x2, y2} };
	Polygon(hdc, points, 3);
}

void DrawPentagon(HDC hdc, int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	double radius = sqrt(dx * dx + dy * dy);

	POINT points[5];
	for (int i = 0; i < 5; ++i) {
		points[i].x = x1 + radius * cos((72 * i - 90) * M_PI / 180);
		points[i].y = y1 + radius * sin((72 * i - 90) * M_PI / 180);
	}
	Polygon(hdc, points, 5);
}

void DrawCircle(HDC hdc, int x1, int y1, int x2, int y2) {
	Ellipse(hdc, x1, y1, x2, y2);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	static COLORREF random_pen[10];
	static COLORREF random_brush[10];
	static TCHAR str[100];
	static SIZE size;
	static HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	static HPEN hPen;
	static HBRUSH hBrush;
	
	srand((unsigned int)time(NULL));

	static int is_first = 0;
	static int parsed = 0;
	static int angle[20] = { 0 };
	static int x1[20];
	static int y1[20];
	static int x2[20];
	static int y2[20];
	static int nWidth[20];
	static int count = 0;
	static int a = 0;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		switch (wParam)
		{
		case 'r':	// 재시작
			break;
		case 'w':
			break;
		case 'a':
			break;
		case 's':
			break;
		case 'd':
			break;
		case 'q':
			PostQuitMessage(0);
			break;
		default:
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		linePen
		hPen = CreatePen(PS_SOLID, nWidth[parsed], random_pen[parsed]);
		hBrush = CreateSolidBrush(random_brush[parsed]);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER:
		hDC = GetDC(hWnd);
		ReleaseDC(hWnd, hDC);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}