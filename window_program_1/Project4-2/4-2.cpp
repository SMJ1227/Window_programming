#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "resource.h"

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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
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

	static HPEN hPen;
	static COLORREF hpen_color = RGB(0,0,0);
	static BOOL isSolid = TRUE;
	int grid = WINDOW_X / GRID;	

	srand((unsigned int)time(NULL));
		 
	HBRUSH brickBrush[5];
	COLORREF brickColor[5];
	brickColor[0] = RGB(255, 0, 0);		// 빨
	brickColor[1] = RGB(0, 255, 0);		// 초
	brickColor[2] = RGB(0, 0, 255);		// 파
	brickColor[3] = RGB(0, 255, 255);	// 하늘
	brickColor[4] = RGB(255, 255, 0);	// 노

	static int count = 0; 
	static BRICK bricks[5];
	static BOOL LDrag = FALSE;
	static BOOL RDrag = FALSE;
	static int rSelected;
	static BOOL border = FALSE;

	static int l_mouse_x;
	static int l_mouse_y;
	static int r_mouse_x;
	static int r_mouse_y;
	static BOOL rdisable = FALSE;

	static BOOL informing = FALSE;

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
		LDrag = TRUE;
		break;
	case WM_LBUTTONUP:
		LDrag = FALSE;
		count = (count + 1) % 5;
		if (count >= 5) {
			count = 0;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_RBUTTONDOWN:
		if (rdisable) {
			break;
		}
		RDrag = TRUE;
		r_mouse_x = LOWORD(IParam);
		r_mouse_y = HIWORD(IParam);
		for (int i = 0; i < 5; i++) {
			if (r_mouse_x > bricks[i].startX && r_mouse_x < bricks[i].oldX &&
				r_mouse_y > bricks[i].startY && r_mouse_y < bricks[i].oldY) {
				bricks[i].selected = TRUE;
				rSelected = i;
			}
		}
		break;
	case WM_RBUTTONUP:
		if (rdisable) {
			break;
		}
		RDrag = FALSE;
		bricks[rSelected].startX = bricks[rSelected].startX / grid * grid;
		bricks[rSelected].startY = bricks[rSelected].startY / grid * grid;
		bricks[rSelected].oldX = bricks[rSelected].oldX / grid * grid + 1;
		bricks[rSelected].oldY = bricks[rSelected].oldY / grid * grid + 1;
		bricks[rSelected].selected = FALSE;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_MOUSEMOVE:
		hDC = GetDC(hWnd);
		if (LDrag)
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
		if (RDrag) {
			int mouseX = LOWORD(IParam);
			int mouseY = HIWORD(IParam);
			int deltaX = mouseX - r_mouse_x;
			int deltaY = mouseY - r_mouse_y;
			bricks[rSelected].startX += deltaX;
			bricks[rSelected].startY += deltaY;
			bricks[rSelected].oldX += deltaX;
			bricks[rSelected].oldY += deltaY;
			r_mouse_x = mouseX;
			r_mouse_y = mouseY;
			InvalidateRect(hWnd, NULL, TRUE);
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
				if (bricks[i].startX > WINDOW_X) {
					bricks[i].startX = bricks[i].startX - WINDOW_X;
					bricks[i].oldX = bricks[i].oldX - WINDOW_X;
				}
			}
			break;
		case VK_LEFT:
			for (int i = 0; i < 5; i++) {
				if (bricks[i].selected) {
					bricks[i].startX = bricks[i].startX - grid;
					bricks[i].oldX = bricks[i].oldX - grid;
				}
				if (bricks[i].oldX < 0) {
					bricks[i].startX = bricks[i].startX + WINDOW_X;
					bricks[i].oldX = bricks[i].oldX + WINDOW_X;
				}
			}
			break;
		case VK_UP:
			for (int i = 0; i < 5; i++) {
				if (bricks[i].selected) {
					bricks[i].startY = bricks[i].startY - grid;
					bricks[i].oldY = bricks[i].oldY - grid;
				}
				if (bricks[i].oldY < 0) {
					bricks[i].startY = bricks[i].startY + WINDOW_Y;
					bricks[i].oldY = bricks[i].oldY + WINDOW_Y;
				}
			}
			break;
		case VK_DOWN:
			for (int i = 0; i < 5; i++) {
				if (bricks[i].selected) {
					bricks[i].startY = bricks[i].startY + grid;
					bricks[i].oldY = bricks[i].oldY + grid;
				}
				if (bricks[i].startY > WINDOW_Y) {
					bricks[i].startY = bricks[i].startY - WINDOW_Y;
					bricks[i].oldY = bricks[i].oldY - WINDOW_Y;
				}
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_COMMAND: //--- 메뉴를 선택했을 때
		switch (LOWORD(wParam)) {
		case ID_GRID_SOLID:
			isSolid = TRUE;
			break;
		case ID_GRID_DOT:
			isSolid = FALSE;
			break;
		case ID_COLOR_RED:
			hpen_color = RGB(255,0,0);
			break;
		case ID_COLOR_GREEN:
			hpen_color = RGB(0,255,0);
			break;
		case ID_COLOR_BLUE:
			hpen_color = RGB(0,0,255);
			break;
		case ID_COLOR_SKY:
			hpen_color = RGB(0,255,255);
			break;
		case ID_COLOR_YELLOW:
			hpen_color = RGB(255,255,0);
			break;
		case ID_COLOR_BLACK:
			hpen_color = RGB(0, 0, 0);
			break;
		case ID_BORDER_OFF:
			border = FALSE;
			break;
		case ID_BORDER_ON:
			border = TRUE;
			break;
		case ID_MOVE_OFF:
			rdisable = TRUE;
			break;
		case ID_MOVE_ON:
			rdisable = FALSE;
			break;
		case ID_INFORM_OFF:
			informing = FALSE;
			break;
		case ID_INFORM_ON:
			informing = TRUE;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		if (isSolid) {
			hPen = CreatePen(PS_SOLID, 1, hpen_color);
			SelectObject(hDC, hPen);
			DrawGrid(hDC, GRID);
			DeleteObject(hPen);
		}
		if (!isSolid) {
			hPen = CreatePen(PS_DOT, 1, hpen_color);
			SelectObject(hDC, hPen);
			DrawGrid(hDC, GRID);
			DeleteObject(hPen);
		}
		

		for (int i = 0; i < 5; i++) {
			// 원래의 사각형 그리기
			brickBrush[i] = CreateSolidBrush(brickColor[i]);
			SelectObject(hDC, brickBrush[i]);
			Rectangle(hDC, bricks[i].startX, bricks[i].startY, bricks[i].oldX, bricks[i].oldY);
			DeleteObject(brickBrush[i]);
			if (bricks[i].selected || border) {
				SelectObject(hDC, (HPEN)GetStockObject(WHITE_PEN));
				Rectangle(hDC, bricks[i].startX, bricks[i].startY, bricks[i].oldX, bricks[i].oldY);
			}

			// 현재 사각형이 윈도우의 영역을 벗어난 만큼 추가적인 도형을 그립니다.
			int offsetY = 0;
			int offsetX = 0;
			if (bricks[i].oldX > WINDOW_X) {
				offsetX = (bricks[i].oldX - WINDOW_X) / grid * grid;
				brickBrush[i] = CreateSolidBrush(brickColor[i]);
				SelectObject(hDC, brickBrush[i]);
				Rectangle(hDC, 0, bricks[i].startY, offsetX, bricks[i].oldY);
				DeleteObject(brickBrush[i]);
			}
			if (bricks[i].oldY > WINDOW_Y) {
				offsetY = (bricks[i].oldY - WINDOW_Y) / grid * grid;
				brickBrush[i] = CreateSolidBrush(brickColor[i]);
				SelectObject(hDC, brickBrush[i]);
				Rectangle(hDC, bricks[i].startX, 0, bricks[i].oldX, offsetY);
				DeleteObject(brickBrush[i]);
			}
		}

		for (int i = 0; i < 5; i++) {
			for (int j = i + 1; j < 5; j++) {
				int left = max(bricks[i].startX, bricks[j].startX);
				int top = max(bricks[i].startY, bricks[j].startY);
				int right = min(bricks[i].oldX, bricks[j].oldX);
				int bottom = min(bricks[i].oldY, bricks[j].oldY);

				if (left < right && top < bottom) {
					int avgRed = (GetRValue(brickColor[i]) + GetRValue(brickColor[j])) % 256;
					int avgGreen = (GetGValue(brickColor[i]) + GetGValue(brickColor[j])) % 256;
					int avgBlue = (GetBValue(brickColor[i]) + GetBValue(brickColor[j])) % 256;

					COLORREF crossed = RGB(avgRed, avgGreen, avgBlue);
					HBRUSH crossed_brush = CreateSolidBrush(crossed);
					SelectObject(hDC, crossed_brush);
					Rectangle(hDC, left, top, right, bottom);
					DeleteObject(crossed_brush);
				}
			}
		}

		if (informing) {
			TCHAR infoText[200];

			wsprintf(infoText, TEXT("도형의 개수: %d\n"), count); // 도형의 개수를 문자열에 추가
			TextOut(hDC, 10, 10, infoText, lstrlen(infoText)); // 화면에 텍스트 출력

			// 각 도형의 가로세로 크기와 색상 출력
			for (int i = 0; i < 5; i++) {
				wsprintf(infoText, TEXT("도형 %d: 가로 %d, 세로 %d, 색상(RGB): (%d, %d, %d)\n"),
					i + 1, bricks[i].oldX - bricks[i].startX, bricks[i].oldY - bricks[i].startY,
					GetRValue(brickColor[i]), GetGValue(brickColor[i]), GetBValue(brickColor[i]));
				TextOut(hDC, 10, 30 + i * 20, infoText, lstrlen(infoText)); // 화면에 텍스트 출력
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
