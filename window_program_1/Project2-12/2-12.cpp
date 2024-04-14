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

typedef struct STONES {
	int x;
	int y;
	int x1;
	int y1;
	int x2;
	int y2;
	int color;
	int angle;
	int previous_angle;
	int cellWidth;
	int cellHeight;
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

void DrawPentagon(HDC hdc, int x1, int y1, int x2, int y2) {
	int centerX = (x1 + x2) / 2;
	int centerY = (y1 + y2) / 2;

	int dx = (x2 - x1) / 3;
	int dy = (y2 - y1) / 3;
	double radius = sqrt(dx * dx + dy * dy);

	POINT points[5];
	for (int i = 0; i < 5; i++) {
		points[i].x = centerX + radius * cos((72 * i - 90) * M_PI / 180);
		points[i].y = centerY + radius * sin((72 * i - 90) * M_PI / 180);
	}
	Polygon(hdc, points, 5);
}

void DrawGrid(HDC hdc, int grid) {
	for (double i = 0; i < grid; i++) {
		DrawLine(hdc, 0, i * (WINDOW_Y / grid), WINDOW_X, i * (WINDOW_Y / grid));
	}
	for (double j = 0; j < grid; j++) {
		DrawLine(hdc, j * (WINDOW_X / grid), 0, j * (WINDOW_X / grid), WINDOW_Y);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	static int grid = 30;

	srand((unsigned int)time(NULL));

	static COLORREF random_pen;
	static COLORREF random_brush;
	static SIZE size;
	static HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	static HBRUSH hBrush;
	static HPEN hPen;

	static STONES stones[MAX];
	static int now = 0;
	static int selected = 0;
	static int c = 0;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		switch (wParam)
		{
		case 's':
			grid = 30;
			break;
		case 'm':
			grid = 40;
			break;
		case 'l':
			grid = 50;
			break;	
		case 'e':	// 원
			stones[now].cellWidth = WINDOW_X / grid;
			stones[now].cellHeight = WINDOW_Y / grid;
			stones[now].x = rand() % grid;
			stones[now].y = rand() % grid;
			stones[now].x1 = stones[now].x * stones[now].cellWidth;
			stones[now].y1 = stones[now].y * stones[now].cellHeight;
			stones[now].x2 = stones[now].x1 + stones[now].cellWidth;
			stones[now].y2 = stones[now].y1 + stones[now].cellHeight;
			stones[now].color = rand() % COLORS;
			stones[now].angle = 6;
			now = (now + 1) % MAX;
			break;
		case 't':	// 삼각형
			stones[now].cellWidth = WINDOW_X / grid;
			stones[now].cellHeight = WINDOW_Y / grid;
			stones[now].x = rand() % grid;
			stones[now].y = rand() % grid;
			stones[now].x1 = stones[now].x * stones[now].cellWidth;
			stones[now].y1 = stones[now].y * stones[now].cellHeight;
			stones[now].x2 = stones[now].x1 + stones[now].cellWidth;
			stones[now].y2 = stones[now].y1 + stones[now].cellHeight;
			stones[now].color = rand() % COLORS;
			stones[now].angle = 3;
			now = (now + 1) % MAX;
			break;	
		case 'r':	// 사각형
			stones[now].cellWidth = WINDOW_X / grid;
			stones[now].cellHeight = WINDOW_Y / grid;
			stones[now].x = rand() % grid;
			stones[now].y = rand() % grid;
			stones[now].x1 = stones[now].x * stones[now].cellWidth;
			stones[now].y1 = stones[now].y * stones[now].cellHeight;
			stones[now].x2 = stones[now].x1 + stones[now].cellWidth;
			stones[now].y2 = stones[now].y1 + stones[now].cellHeight;
			stones[now].color = rand() % COLORS;
			stones[now].angle = 4;
			now = (now + 1) % MAX;
			break;
		case '0':
			selected = 0;
			break;
		case '1':
			selected = 1;
			break;
		case '2':
			selected = 2;
			break;
		case '3':
			selected = 3;
			break;
		case '4':
			selected = 4;
			break;
		case '5':
			selected = 5;
			break;
		case '6':
			selected = 6;
			break;
		case '7':
			selected = 7;
			break;
		case '8':
			selected = 8;
			break;
		case '9':
			selected = 9;
			break;
		case '+':	// 확대
		case '=':
			if (stones[selected].cellWidth < WINDOW_X / grid) {
				stones[selected].cellWidth = stones[selected].cellWidth + 1;
				stones[selected].cellHeight = stones[selected].cellHeight + 1;
				stones[selected].x2 = stones[selected].x1 + stones[selected].cellWidth;
				stones[selected].y2 = stones[selected].y1 + stones[selected].cellHeight;
			}
			break;
		case '-':	// 축소
		case '_':
			if (stones[selected].cellWidth > 10) {
				stones[selected].cellWidth = stones[selected].cellWidth - 1;
				stones[selected].cellHeight = stones[selected].cellHeight - 1;
				stones[selected].x2 = stones[selected].x1 + stones[selected].cellWidth;
				stones[selected].y2 = stones[selected].y1 + stones[selected].cellHeight;
			}
			break;
		case 'c':	// 같은 색이면 같은 도형
			if (c == 0) {
				for (int i = 0; i < MAX; i++) {
					if (stones[i].color == stones[selected].color) {
						stones[i].previous_angle = stones[i].angle;
						stones[i].angle = 5;
					}
				}
				c++;
			}
			else {
				for (int i = 0; i < MAX; i++) {
					if (stones[i].color == stones[selected].color) {
						stones[i].angle = stones[i].previous_angle;
					}
				}
				c--;
			}
			break;	
		case 'd':	// 도형 삭제
			for (int i = 0; i < MAX - 1; i++) {
				stones[i].cellWidth = stones[i + 1].cellWidth;
				stones[i].cellHeight = stones[i + 1].cellHeight;
				stones[i].x = stones[i + 1].x;
				stones[i].y = stones[i + 1].y;
				stones[i].x1 = stones[i + 1].x1;
				stones[i].y1 = stones[i + 1].y1;
				stones[i].x2 = stones[i + 1].x2;
				stones[i].y2 = stones[i + 1].y2;
				stones[i].color = stones[i + 1].color;
				stones[i].angle = stones[i + 1].angle;
			}
			break;
		case 'p':	// 삭제 초기화
			for (int i = 0; i < MAX; ++i) {
				stones[i].cellWidth = 0;
				stones[i].cellHeight = 0;
				stones[i].x = 0;
				stones[i].y = 0;
				stones[i].x1 = 0;
				stones[i].y1 = 0;
				stones[i].x2 = 0;
				stones[i].y2 = 0;
				stones[i].color = 0;
				stones[i].angle = 0;
			}
			selected = 0;
			now = 0;
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
			if (stones[selected].x <= 0) {
				stones[selected].x = grid - 1;
				stones[selected].x1 = stones[selected].x * stones[selected].cellWidth;
				stones[selected].x2 = stones[selected].x1 + stones[selected].cellWidth;
			}
			else {
				stones[selected].x = stones[selected].x - 1;
				stones[selected].x1 = stones[selected].x * stones[selected].cellWidth;
				stones[selected].x2 = stones[selected].x1 + stones[selected].cellWidth;
			}
			break;
		case VK_RIGHT:
			if (stones[selected].x >= grid - 1) {
				stones[selected].x = 0;
				stones[selected].x1 = stones[selected].x * stones[selected].cellWidth;
				stones[selected].x2 = stones[selected].x1 + stones[selected].cellWidth;
			}
			else {
				stones[selected].x = stones[selected].x + 1;
				stones[selected].x1 = stones[selected].x * stones[selected].cellWidth;
				stones[selected].x2 = stones[selected].x1 + stones[selected].cellWidth;
			}
			break;
		case VK_UP:
			if (stones[selected].y <= 0) {
				stones[selected].y = grid - 1;
				stones[selected].y1 = stones[selected].y * stones[selected].cellHeight;
				stones[selected].y2 = stones[selected].y1 + stones[selected].cellHeight;
			}
			else {
				stones[selected].y = stones[selected].y - 1;
				stones[selected].y1 = stones[selected].y * stones[selected].cellHeight;
				stones[selected].y2 = stones[selected].y1 + stones[selected].cellHeight;
			}
			stones[selected].y = stones[selected].y - 1;
			stones[selected].y1 = stones[selected].y * stones[selected].cellHeight;
			stones[selected].y2 = stones[selected].y1 + stones[selected].cellHeight;
			break;
		case VK_DOWN:
			if (stones[selected].y >= grid - 1) {
				stones[selected].y = 0;
				stones[selected].y1 = stones[selected].y * stones[selected].cellHeight;
				stones[selected].y2 = stones[selected].y1 + stones[selected].cellHeight;
			}
			else {
				stones[selected].y = stones[selected].y + 1;
				stones[selected].y1 = stones[selected].y * stones[selected].cellHeight;
				stones[selected].y2 = stones[selected].y1 + stones[selected].cellHeight;
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC, grid);

		for (int i = 0; i < MAX; i++) {	//selected 제외 그리기
			if (i == selected) { continue; }
			switch (stones[i].color)	// 색정하기
			{
			case 0:
				random_brush = RGB(0, 0, 0);	// 검정
				hBrush = CreateSolidBrush(random_brush);
				break;
			case 1:
				random_brush = RGB(255, 0, 0);	// 빨강
				hBrush = CreateSolidBrush(random_brush);
				break;
			case 2:
				random_brush = RGB(255, 255, 0);// 노랑
				hBrush = CreateSolidBrush(random_brush);
				break;
			case 3:		
				random_brush = RGB(0, 0, 255);	// 검정
				hBrush = CreateSolidBrush(random_brush);
				break;
			}
			SelectObject(hDC, hBrush);
			switch (stones[i].angle)	// 그리기
			{
			case 3:
				DrawTriangle(hDC, stones[i].x1, stones[i].y1, stones[i].x2, stones[i].y2);
				break;
			case 4:
				DrawRectangle(hDC, stones[i].x1, stones[i].y1, stones[i].x2, stones[i].y2);
				break;
			case 5:
				DrawPentagon(hDC, stones[i].x1, stones[i].y1, stones[i].x2, stones[i].y2);
				break;
			case 6:
				DrawCircle(hDC, stones[i].x1, stones[i].y1, stones[i].x2, stones[i].y2);
				break;
			}
		}

		// selected 그리기
		switch (stones[selected].color)	// 색정하기
		{
		case 0:
			random_brush = RGB(0, 0, 0);	// 검정
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
			hBrush = CreateSolidBrush(random_brush);
			break;
		case 1:
			random_brush = RGB(255, 0, 0);	// 빨강
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
			hBrush = CreateSolidBrush(random_brush);
			break;
		case 2:
			random_brush = RGB(255, 255, 0);// 노랑
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
			hBrush = CreateSolidBrush(random_brush);
			break;
		case 3:
			random_brush = RGB(0, 0, 255);	// 검정
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
			hBrush = CreateSolidBrush(random_brush);
			break;
		}
		SelectObject(hDC, hPen);
		SelectObject(hDC, hBrush);
		switch (stones[selected].angle)	// 그리기
		{
		case 3:
			DrawTriangle(hDC, stones[selected].x1, stones[selected].y1, stones[selected].x2, stones[selected].y2);
			break;
		case 4:
			DrawRectangle(hDC, stones[selected].x1, stones[selected].y1, stones[selected].x2, stones[selected].y2);
			break;
		case 5:
			DrawPentagon(hDC, stones[selected].x1, stones[selected].y1, stones[selected].x2, stones[selected].y2);
			break;
		case 6:
			DrawCircle(hDC, stones[selected].x1, stones[selected].y1, stones[selected].x2, stones[selected].y2);
			break;
		}
		DeleteObject(hBrush);
		DeleteObject(hPen);

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