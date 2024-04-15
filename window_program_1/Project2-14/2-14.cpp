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

typedef struct NUMBER {
	int x;
	int y;
	int number;
};

void ShowMessageBoxWIN(HWND hWnd) {
	MessageBox(hWnd, L"성공", L"성공", MB_OK);
}

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
	static ALP alpa[26];
	static NUMBER numbers[10];
	static int mode;
	srand((unsigned int)time(NULL));

	static SIZE size;
	static HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	static HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
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
		for (int i = 0; i < 10; i++) {
			numbers[i].number = i;
			numbers[i].x = rand() % GRID;
			numbers[i].y = rand() % GRID;
		}
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
		case '1':
			mode = 1;
			break;
		case '2':
			mode = 2;
			break;
		case '3':
			mode = 3;
			break;
		case 'p':
			for (int i = 0; i < 10; i++) {
				numbers[i].number = i;
				numbers[i].x = rand() % GRID;
				numbers[i].y = rand() % GRID;
			}
			for (int i = 0; i < 26; i++) {
				alpa[i].alp = 'a' + i;
				alpa[i].x = rand() % GRID;
				alpa[i].y = rand() % GRID;
			}
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

		switch (mode)
		{
		case 1:
			// 숫자 충돌체크
			for (int i = 0; i < 10; i++) {
				if (x == numbers[i].x && y == numbers[i].y) {
					switch (prev_keydown)
					{
					case 'w':
						if (numbers[i].y == 0) {
							numbers[i].y = GRID - 1;
						}
						else {
							numbers[i].y--;
						}
						break;
					case 'a':
						if (numbers[i].x == 0) {
							numbers[i].x = GRID - 1;
						}
						else {
							numbers[i].x--;
						}
						break;
					case 's':
						if (numbers[i].y == GRID - 1) {
							numbers[i].y = 0;
						}
						else {
							numbers[i].y++;
						}
						break;
					case 'd':
						if (numbers[i].x == GRID - 1) {
							numbers[i].x = 0;
						}
						else {
							numbers[i].x++;
						}
						break;
					}
				}
			}
			break;
		case 2:
			// 알파벳 충돌체크
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
			}
			break;
		case 3 :
			// 알파벳 충돌체크
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
			}
			// 숫자 충돌체크
			for (int i = 0; i < 10; i++) {
				if (x == numbers[i].x && y == numbers[i].y) {
					switch (prev_keydown)
					{
					case 'w':
						if (numbers[i].y == 0) {
							numbers[i].y = GRID - 1;
						}
						else {
							numbers[i].y--;
						}
						break;
					case 'a':
						if (numbers[i].x == 0) {
							numbers[i].x = GRID - 1;
						}
						else {
							numbers[i].x--;
						}
						break;
					case 's':
						if (numbers[i].y == GRID - 1) {
							numbers[i].y = 0;
						}
						else {
							numbers[i].y++;
						}
						break;
					case 'd':
						if (numbers[i].x == GRID - 1) {
							numbers[i].x = 0;
						}
						else {
							numbers[i].x++;
						}
						break;
					}
				}
			}
			break;
		}

		switch (mode)
		{
		case 1:
			// 숫자끼리 체크
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (i == j) { continue; }
					if (numbers[i].x == numbers[j].x && numbers[i].y == numbers[j].y) {
						numbers[i].number = numbers[i].number + numbers[j].number;
						for (int k = j; k < 9; k++) {
							numbers[k].x = numbers[k + 1].x;
							numbers[k].y = numbers[k + 1].y;
							numbers[k].number = numbers[k + 1].number;
						}
						// 마지막 숫자는 초기화
						numbers[9].x = -1;
						numbers[9].y = -1;
						numbers[9].number = -1;
						break;
					}
				}
			}
			break;
		case 2:
			// 영어끼리 체크
			for (int i = 0; i < 26; i++) {
				for (int j = 0; j < 26; j++) {
					if (i == j) { continue; }
					if (alpa[i].x == alpa[j].x && alpa[i].y == alpa[j].y) {
						// 각 알파벳을 1부터 26까지의 숫자로 매핑하여 더함
						int num_i = alpa[i].alp - L'a' + 1;
						int num_j = alpa[j].alp - L'a' + 1;
						int sum = num_i + num_j;
						// 더한 결과가 26을 넘어가면 26으로 나눈 나머지를 사용함
						if (sum > 26) {
							sum %= 26;
						}
						// 숫자를 다시 알파벳으로 변환하여 저장
						alpa[i].alp = (wchar_t)(L'a' + sum - 1);
						for (int k = j; k < 25; k++) {
							alpa[k].x = alpa[k + 1].x;
							alpa[k].y = alpa[k + 1].y;
							alpa[k].alp = alpa[k + 1].alp;
						}
						// 마지막 숫자는 초기화
						alpa[25].x = -1;
						alpa[25].y = -1;
						alpa[25].alp = 'a';
						break;
					}
				}
			}
			break;
		case 3:
			// 숫자끼리 체크
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (i == j) { continue; }
					if (numbers[i].x == numbers[j].x && numbers[i].y == numbers[j].y) {
						numbers[i].number = numbers[i].number + numbers[j].number;
						// 마지막 숫자는 초기화
						for (int k = j; k < 9; k++) {
							numbers[k].x = numbers[k + 1].x;
							numbers[k].y = numbers[k + 1].y;
							numbers[k].number = numbers[k + 1].number;
						}
						numbers[9].x = -1;
						numbers[9].y = -1;
						numbers[9].number = -1;
						break;
					}
				}
			}
			// 영어끼리 체크
			for (int i = 0; i < 26; i++) {
				for (int j = 0; j < 26; j++) {
					if (i == j) { continue; }
					if (alpa[i].x == alpa[j].x && alpa[i].y == alpa[j].y) {
						// 각 알파벳을 1부터 26까지의 숫자로 매핑하여 더함
						int num_i = alpa[i].alp - L'a' + 1;
						int num_j = alpa[j].alp - L'a' + 1;
						int sum = num_i + num_j;
						// 더한 결과가 26을 넘어가면 26으로 나눈 나머지를 사용함
						if (sum > 26) {
							sum %= 26;
						}
						// 숫자를 다시 알파벳으로 변환하여 저장
						alpa[i].alp = (wchar_t)(L'a' + sum - 1);
						// 마지막 영어는 초기화
						for (int k = j; k < 25; k++) {
							alpa[k].x = alpa[k + 1].x;
							alpa[k].y = alpa[k + 1].y;
							alpa[k].alp = alpa[k + 1].alp;
						}
						alpa[25].x = -1;
						alpa[25].y = -1;
						alpa[25].alp = 'a';
						break;
					}
				}
			}
			// 영어숫자 체크
			for (int i = 0; i < 26; i++) {
				for (int j = 0; j < 10; j++) {
					if (alpa[i].x == numbers[j].x && alpa[i].y == numbers[j].y) {
						// 각 알파벳을 1부터 26까지의 숫자로 매핑하여 더함
						int num_i = alpa[i].alp - L'a' + 1;
						int num_j = numbers[j].number;
						int sum = num_i + num_j;
						// 더한 결과가 26을 넘어가면 26으로 나눈 나머지를 사용함
						if (sum > 26) {
							sum %= 26;
						}
						// 숫자를 다시 알파벳으로 변환하여 저장
						alpa[i].alp = (wchar_t)(L'a' + sum - 1);
						// 마지막 숫자는 초기화
						for (int k = j; k < 9; k++) {
							numbers[k].x = numbers[k + 1].x;
							numbers[k].y = numbers[k + 1].y;
							numbers[k].number = numbers[k + 1].number;
						}
						numbers[9].x = -1;
						numbers[9].y = -1;
						numbers[9].number = -1;
						break;
					}
				}
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC);

		SelectObject(hDC, hBrush);
		DrawRectangle(hDC, player_x1, player_y1, player_x2, player_y2);

		for (int i = 0; i < 10; i++) {
			TCHAR singleChar[10];
			_stprintf_s(singleChar, _T("%d"), numbers[i].number);
			TextOut(hDC, numbers[i].x* grid + 10, numbers[i].y* grid + 5, singleChar, _tcslen(singleChar));
		}

		for (int i = 0; i < 26; i++) {
			TCHAR singleChar[2];
			singleChar[0] = alpa[i].alp;
			singleChar[1] = '\0';
			TextOut(hDC, alpa[i].x * grid + 10, alpa[i].y * grid + 5, singleChar, 1);
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