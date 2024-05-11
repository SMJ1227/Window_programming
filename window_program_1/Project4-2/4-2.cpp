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

typedef struct BRICK {
	int startX;
	int startY;
	int oldX;
	int oldY;
	int endX;
	int endY;
	BOOL selected;
	COLORREF color;
};

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
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

	HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	int grid = WINDOW_X / GRID;	

	srand((unsigned int)time(NULL));

	HBRUSH brickBrush[5];
	COLORREF brickColor[5];
	brickColor[0] = RGB(255, 0, 0);	// 빨
	brickColor[1] = RGB(0, 255, 0);	// 초
	brickColor[2] = RGB(0, 0, 255);	// 파
	brickColor[3] = RGB(0, 255, 255);	// 하늘
	brickColor[4] = RGB(255, 255, 0);	// 노

	static int count = 0; 
	static BRICK bricks[5];
	static BOOL Drag = FALSE;

	static COLORREF crossed;
	static HBRUSH crossed_brush;
	static int avgRed;
	static int avgGreen;
	static int avgBlue;

	static int l_mouse_x;
	static int l_mouse_y;
	static int r_mouse_x;
	static int r_mouse_y;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		for (int i = 0; i < 5; i++) {
			bricks[i].selected = FALSE;
		}
		break;

	case WM_LBUTTONDOWN:
		InvalidateRect(hWnd, NULL, TRUE);
		bricks[count].oldX = bricks[count].startX = LOWORD(IParam) / grid * grid;
		bricks[count].oldY = bricks[count].startY = HIWORD(IParam) / grid * grid;
		Drag = TRUE;
		break;
	case WM_LBUTTONUP:
		Drag = FALSE;
		count = (count + 1) % 5;
		if (count >= 5) {
			count = 0;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_RBUTTONDOWN:
		Drag = FALSE;
		r_mouse_x = LOWORD(IParam);
		r_mouse_y = HIWORD(IParam);
		break;
	case WM_MOUSEMOVE:
		hDC = GetDC(hWnd);
		if (Drag)
		{
			SetROP2(hDC, R2_COPYPEN);
			brickBrush[count] = CreateSolidBrush(brickColor[count]);
			SelectObject(hDC, brickBrush[count]);
			bricks[count].endX = LOWORD(IParam) / grid * grid + 1;
			bricks[count].endY = HIWORD(IParam) / grid * grid + 1;
			Rectangle(hDC, bricks[count].startX, bricks[count].startY, bricks[count].oldX, bricks[count].oldY); // 지우기
			Rectangle(hDC, bricks[count].startX, bricks[count].startY, bricks[count].endX, bricks[count].endY);	// 그리기
			bricks[count].oldX = bricks[count].endX; bricks[count].oldY = bricks[count].endY;
			DeleteObject(brickBrush[count]);
		}
		ReleaseDC(hWnd, hDC);
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'q':
			PostQuitMessage(0);
			break;
		case '0':
			if (bricks[0].selected == TRUE) {
				bricks[0].selected = FALSE;
			}
			else {
				bricks[0].selected = TRUE;
			}
			break;
		case '1':
			if (bricks[1].selected == TRUE) {
				bricks[1].selected = FALSE;
			}
			else {
				bricks[1].selected = TRUE;
			}
			break;
		case '2':
			if (bricks[2].selected == TRUE) {
				bricks[2].selected = FALSE;
			}
			else {
				bricks[2].selected = TRUE;
			}
			break;
		case '3':
			if (bricks[3].selected == TRUE) {
				bricks[3].selected = FALSE;
			}
			else {
				bricks[3].selected = TRUE;
			}
			break;
		case '4':
			if (bricks[4].selected == TRUE) {
				bricks[4].selected = FALSE;
			}
			else {
				bricks[4].selected = TRUE;
			}
			break;	
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RIGHT:
			for (int i = 0; i < 5; i++) {
				if (bricks[i].selected) {
					bricks[i].startX = bricks[i].startX + grid;
					bricks[i].oldX = bricks[i].oldX + grid;
				}
			}
			break;
		case VK_LEFT:
			for (int i = 0; i < 5; i++) {
				if (bricks[i].selected) {
					bricks[i].startX = bricks[i].startX - grid;
					bricks[i].oldX = bricks[i].oldX - grid;
				}
			}
			break;
		case VK_UP:
			for (int i = 0; i < 5; i++) {
				if (bricks[i].selected) {
					bricks[i].startY = bricks[i].startY - grid;
					bricks[i].oldY = bricks[i].oldY - grid;
				}
			}
			break;
		case VK_DOWN:
			for (int i = 0; i < 5; i++) {
				if (bricks[i].selected) {
					bricks[i].startY = bricks[i].startY + grid;
					bricks[i].oldY = bricks[i].oldY + grid;
				}
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC, GRID);
		DeleteObject(linePen);

		for (int i = 0; i < 5; i++) {
			// 현재 사각형이 윈도우의 영역을 벗어난 만큼 추가적인 도형을 그립니다.
			int offsetY = 0;
			int offsetX = 0;
			if (bricks[i].oldX > WINDOW_X) {
				offsetX = (bricks[i].oldX - WINDOW_X) / grid * grid;
			}
			if (bricks[i].oldY > WINDOW_Y) {
				offsetY = (bricks[i].oldY - WINDOW_Y);
			}
			// 원래의 사각형 그리기
			brickBrush[i] = CreateSolidBrush(brickColor[i]);
			SelectObject(hDC, brickBrush[i]);
			Rectangle(hDC, bricks[i].startX, bricks[i].startY, bricks[i].oldX, bricks[i].oldY);
			DeleteObject(brickBrush[i]);
			if (bricks[i].selected) {
				SelectObject(hDC, (HPEN)GetStockObject(WHITE_PEN));
				Rectangle(hDC, bricks[i].startX, bricks[i].startY, bricks[i].oldX, bricks[i].oldY);
			}

			brickBrush[i] = CreateSolidBrush(brickColor[i]);
			SelectObject(hDC, brickBrush[i]);
			Rectangle(hDC, 0, bricks[i].startY, offsetX, bricks[i].oldY);
			DeleteObject(brickBrush[i]);
			if (bricks[i].selected) {
				SelectObject(hDC, (HPEN)GetStockObject(WHITE_PEN));
				Rectangle(hDC, 0, bricks[i].startY, offsetX, bricks[i].oldY);
			}

			
		}

		for (int i = 0; i < 5; i++) {
			for (int j = i + 1; j < 5; j++) {
				int left = max(bricks[i].startX, bricks[j].startX);
				int top = max(bricks[i].startY, bricks[j].startY);
				int right = min(bricks[i].oldX, bricks[j].oldX);
				int bottom = min(bricks[i].oldY, bricks[j].oldY);

				if (left < right && top < bottom) {
					avgRed = (GetRValue(brickColor[i]) + GetRValue(brickColor[j])) % 256;
					avgGreen = (GetGValue(brickColor[i]) + GetGValue(brickColor[j])) % 256;
					avgBlue = (GetBValue(brickColor[i]) + GetBValue(brickColor[j])) % 256;

					crossed = RGB(avgRed, avgGreen, avgBlue);
					crossed_brush = CreateSolidBrush(crossed);
					SelectObject(hDC, crossed_brush);
					Rectangle(hDC, left, top, right, bottom);
					DeleteObject(crossed_brush);
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
