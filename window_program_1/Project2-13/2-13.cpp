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

typedef struct ALP {
	int x;
	int y;
	char alp;
};

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

void DrawRectangle(HDC hdc, int x1, int y1, int x2, int y2) {
	Rectangle(hdc, x1, y1, x2, y2);
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
	TCHAR str1[10] = L"apple";
	static ALP alpa[26];

	static int rand1;
	srand((unsigned int)time(NULL));

	static SIZE size;
	static HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	static HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0));
	static HPEN hPen;

	static int grid = WINDOW_X / GRID;
	static int x = 0;
	static int y = 0;
	static int player_x1 = x * grid;
	static int player_y1 = y * grid;
	static int player_x2 = player_x1 + grid;
	static int player_y2 = player_y1 + grid;
	static char prev_keydown;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		rand1 = rand() % _tcslen(str1);
		for (int i = 0; i < 26; i++) {
			alpa[i].alp = 'a' + i;
			alpa[i].x = rand() % GRID;
			alpa[i].y = rand() % GRID;
		}
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
			prev_keydown = 'a';
			break;
		case VK_RIGHT:
			if (x == GRID - 1) {
				x = 0;
			}
			else {
				x++;
			}
			prev_keydown = 'd';
			break;
		case VK_UP:
			if (y == 0) {
				y = GRID - 1;
			}
			else {
				y--;
			}
			prev_keydown = 'w';
			break;
		case VK_DOWN:
			if (y == GRID - 1) {
				y = 0;
			}
			else {
				y++;
			}
			prev_keydown = 's';
			break;
		}
		player_x1 = x * grid;
		player_y1 = y * grid;
		player_x2 = player_x1 + grid;
		player_y2 = player_y1 + grid;

		// 충돌체크
		for (int i = 0; i < 26; i++) {
			if (x == alpa[i].x && y == alpa[i].y) {
				switch (prev_keydown)
				{
				case 'w':
					if (alpa[i].y == 0) {
						alpa[i].y = GRID - 1;
					}
					else {
						alpa[i].y--;
					}
					break;
				case 'a':
					if (alpa[i].x == 0) {
						alpa[i].x = GRID - 1;
					}
					else {
						alpa[i].x--;
					}
					break;
				case 's':
					if (alpa[i].y == GRID - 1) {
						alpa[i].y = 0;
					}
					else {
						alpa[i].y++;
					}
					break;
				case 'd':
					if (alpa[i].x == GRID - 1) {
						alpa[i].x = 0;
					}
					else {
						alpa[i].x++;
					}
					break;
				}
			}
			for (int j = 0; j < 26; j++) {
				if (j == i) { continue; }
				if (alpa[i].x == alpa[j].x && alpa[i].y == alpa[j].y) {
					switch (prev_keydown)
					{
					case 'w':
						if (alpa[j].y == 0) {
							alpa[j].y = GRID - 1;
						}
						else {
							alpa[j].y--;
						}
						break;
					case 'a':
						if (alpa[j].x == 0) {
							alpa[j].x = GRID - 1;
						}
						else {
							alpa[j].x--;
						}
						break;
					case 's':
						if (alpa[j].y == GRID - 1) {
							alpa[j].y = 0;
						}
						else {
							alpa[j].y++;
						}
						break;
					case 'd':
						if (alpa[j].x == GRID - 1) {
							alpa[j].x = 0;
						}
						else {
							alpa[j].x++;
						}
						break;
					}
				}
			}
		}

		// 정답 체크
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC);

		SelectObject(hDC, hBrush);
		DrawRectangle(hDC, player_x1, player_y1, player_x2, player_y2);

		for (int i = 0; i < _tcslen(str1); i++) {
			if (i == rand1) { continue; }
			TCHAR singleChar[2]; // 개별 문자를 저장할 배열
			singleChar[0] = str1[i]; // 개별 문자 할당
			singleChar[1] = '\0'; // 문자열 종료 문자 추가

			TextOut(hDC, i * grid + 10, grid + 5, singleChar, 1); // 개별 문자 출력
		}

		for (int i = 0; i < 26; i++) {
			TCHAR singleChar[2]; // 개별 문자를 저장할 배열
			singleChar[0] = alpa[i].alp; // 개별 문자 할당
			singleChar[1] = '\0'; // 문자열 종료 문자 추가
			TextOut(hDC, alpa[i].x * grid + 10, alpa[i].y * grid + 5, singleChar, 1); // 개별 문자 출력
		}
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