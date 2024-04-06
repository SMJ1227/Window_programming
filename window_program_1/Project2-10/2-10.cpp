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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
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
	static HPEN hPen;
	static HBRUSH hBrush;
	
	srand((unsigned int)time(NULL));

	static int is_first = 0;
	static int parsed = 0;
	static int angle[10] = { 0 };
	static int x1[10];
	static int y1[10];
	static int x2[10];
	static int y2[10];
	static int nWidth[10];
	static int count = 0;
	static int a = 0;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		for (int i = 0; i < 10; i++) {
			random_pen[i] = RGB(rand() % 256, rand() % 256, rand() % 256);
			random_brush[i] = RGB(rand() % 256, rand() % 256, rand() % 256);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			x1[parsed] = x1[parsed] - 10;
			x2[parsed] = x2[parsed] - 10;
			break;
		case VK_UP:
			y1[parsed] = y1[parsed] - 10;
			y2[parsed] = y2[parsed] - 10;
			break;
		case VK_RIGHT:	
			x1[parsed] = x1[parsed] + 10;
			x2[parsed] = x2[parsed] + 10;
			break;
		case VK_DOWN:
			y1[parsed] = y1[parsed] + 10;
			y2[parsed] = y2[parsed] + 10;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		switch (wParam)
		{
		case '+':
		case '=':
			if (nWidth[parsed] > 10) {
				x2[parsed] = x2[parsed] + 10;
				y2[parsed] = y2[parsed] + 10;
			}
			else {
				nWidth[parsed]++;
			}
			break;
		case '-':
		case '_':
			if (nWidth[parsed] < 1) {
				x2[parsed] = x2[parsed] - 10;
				y2[parsed] = y2[parsed] - 10;
			}
			else {
				nWidth[parsed]--;
			}
			break;
		case 'r':
			random_pen[parsed] = RGB(rand() % 255, rand() % 255, rand() % 255);
			break;
		case 'b':
			random_brush[parsed] = RGB(rand() % 255, rand() % 255, rand() % 255);
			break;
		case 'p':
			parsed = (parsed - 1) % 10;
			break;
		case 'n':
			parsed = (parsed + 1) % 10;
			break;
		case 'a':
			a = 1;
			break;
		case 'q':
			PostQuitMessage(0);
			break;
		case VK_RETURN:
			parsed = (parsed + 1) % 10;	// prased는 0부터 시작.
			if (_stscanf(str, _T("%d %d %d %d %d %d"),
				&angle[parsed], &x1[parsed], &y1[parsed], &x2[parsed], &y2[parsed], &nWidth[parsed]) != 6) 
			{ 
				count = 0;
				memset(str, 0, sizeof(str));
			}
			else {
				if (angle[parsed] > 6 || angle[parsed] < 1) {
					count = 0;
					parsed = parsed - 1;
					memset(str, 0, sizeof(str));
				}
				else if (nWidth[parsed] < 1 || nWidth[parsed] > 10) {
					count = 0;
					parsed = parsed - 1;
					memset(str, 0, sizeof(str));
				}
				else if (x2[parsed] > 800 || y2[parsed] > 600) {
					count = 0;
					parsed = parsed - 1;
					memset(str, 0, sizeof(str));
				}
				count = 0;
				memset(str, 0, sizeof(str));
			}
			break;
		case VK_BACK:
			if (count < 1) {
				count = 0;
			}
			else {
				str[--count] = '\0';
			}
			break;
		default:
			str[count++] = wParam;
			str[count] = '\0';
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		hPen = CreatePen(PS_SOLID, nWidth[parsed], random_pen[parsed]);
		hBrush = CreateSolidBrush(random_brush[parsed]);

		GetTextExtentPoint32(hDC, str, lstrlen(str), &size);
		TextOut(hDC, 0, 0, str, lstrlen(str));
		SetCaretPos(size.cx, 0);

		switch (angle[parsed])
		{
		case 1:
			SelectObject(hDC, hPen);
			SetPixelV(hDC, x1[parsed], y1[parsed], random_pen[parsed]);
			DeleteObject(hPen);
			break;
		case 2:
			SelectObject(hDC, hBrush);
			SelectObject(hDC, hPen);
			DrawLine(hDC, x1[parsed], y1[parsed], x2[parsed], y2[parsed]);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			break;
		case 3:
			SelectObject(hDC, hBrush);
			SelectObject(hDC, hPen);
			DrawTriangle(hDC, x1[parsed], y1[parsed], x2[parsed], y2[parsed]);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			break;
		case 4:
			SelectObject(hDC, hBrush);
			SelectObject(hDC, hPen);
			DrawRectangle(hDC, x1[parsed], y1[parsed], x2[parsed], y2[parsed]);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			break;
		case 5:
			SelectObject(hDC, hBrush);
			SelectObject(hDC, hPen);
			DrawPentagon(hDC, x1[parsed], y1[parsed], x2[parsed], y2[parsed]);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			break;
		case 6:
			SelectObject(hDC, hBrush);
			SelectObject(hDC, hPen);
			DrawCircle(hDC, x1[parsed], y1[parsed], x2[parsed], y2[parsed]);
			DeleteObject(hBrush);
			DeleteObject(hPen);
			break;
		}
		if (a != 0) {
			for (int i = 0; i < 10; i++) {
				hPen = CreatePen(PS_SOLID, nWidth[parsed], random_pen[i]);
				hBrush = CreateSolidBrush(random_brush[i]);
				switch (angle[i]) {
				case 1:
					SelectObject(hDC, hPen);
					SetPixelV(hDC, x1[i], y1[i], random_pen[i]);
					DeleteObject(hPen);
					break;
				case 2:
					SelectObject(hDC, hBrush);
					SelectObject(hDC, hPen);
					DrawLine(hDC, x1[i], y1[i], x2[i], y2[i]);
					DeleteObject(hBrush);
					DeleteObject(hPen);
					break;
				case 3:
					SelectObject(hDC, hBrush);
					SelectObject(hDC, hPen);
					DrawTriangle(hDC, x1[i], y1[i], x2[i], y2[i]);
					DeleteObject(hBrush);
					DeleteObject(hPen);
					break;
				case 4:
					SelectObject(hDC, hBrush);
					SelectObject(hDC, hPen);
					DrawRectangle(hDC, x1[i], y1[i], x2[i], y2[i]);
					DeleteObject(hBrush);
					DeleteObject(hPen);
					break;
				case 5:
					SelectObject(hDC, hBrush);
					SelectObject(hDC, hPen);
					DrawPentagon(hDC, x1[i], y1[i], x2[i], y2[i]);
					DeleteObject(hBrush);
					DeleteObject(hPen);
					break;
				case 6:
					SelectObject(hDC, hBrush);
					SelectObject(hDC, hPen);
					DrawCircle(hDC, x1[i], y1[i], x2[i], y2[i]);
					DeleteObject(hBrush);
					DeleteObject(hPen);
					break;
				}
			}
			a -= 1;
		}
		EndPaint(hWnd, &ps);
		break;
	}
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