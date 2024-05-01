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
#define BRICKS (BRICK_X * BRICK_Y)
#define BRICK_X 7
#define BRICK_Y 3

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
	int x1;
	int y1;
	int x2;
	int y2;
	int is_disable;
};

void InitBRICK(BRICK* bricks, int grid, int length_x, int startX, int startY) {	
	for (int i = 0; i < BRICK_X * BRICK_Y; i++) {
		bricks[i].x1 = startX + length_x * (i % BRICK_X);
		bricks[i].y1 = startY + (i / BRICK_X) * grid;
		bricks[i].x2 = bricks[i].x1 + length_x;
		bricks[i].y2 = bricks[i].y1 + grid;
		bricks[i].is_disable = 0;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	static int is_start = 0;
	static int is_paused = 0;
	int startX = 150;
	int startY = 400;
	int grid = WINDOW_X / GRID;
	int length_x = (WINDOW_X - (startX * 2)) / BRICK_X;
	static int speed = 150;

	srand((unsigned int)time(NULL));

	COLORREF brick_color = RGB(255, 255, 0);
	HBRUSH brickBrush = CreateSolidBrush(brick_color);
	static BRICK bricks[BRICKS];
	static int brick_rl = 0;
	static int lost_bricks = 0;
	static TCHAR message[100];

	COLORREF player_color = RGB(0, 0, 255);
	HBRUSH playerBrush = CreateSolidBrush(player_color);
	static int player_x1 = 230;
	static int player_y1 = 0;
	static int player_x2 = player_x1 + grid;
	static int player_y2 = player_y1 + grid;
	static int player_speed;
	static int player_is_down = 1;
	static int player_rl;
	static int degree;

	static int l_mouse_x;
	static int l_mouse_y;

	static int r_mouse_x;
	static int r_mouse_y;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		InitBRICK(bricks, grid, length_x, startX, startY);
		break;

	case WM_LBUTTONDOWN:
		if (!is_start) { break; }
		if (bricks[0].x1 > 0) {
			for (int i = 0; i < BRICK_X * BRICK_Y; i++) {
				bricks[i].x1 = bricks[i].x1 - 15;
				bricks[i].x2 = bricks[i].x1 + length_x;
			}
		}
		break;

	case WM_RBUTTONDOWN:
		if (!is_start) { break; }
		if (bricks[6].x1 < 450) {
			for (int i = 0; i < BRICK_X * BRICK_Y; i++) {
				bricks[i].x1 = bricks[i].x1 + 15;
				bricks[i].x2 = bricks[i].x1 + length_x;
			}
		}
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			// 블럭 이동
			for (int i = 0; i < BRICK_X * BRICK_Y; i++) {
				if (brick_rl == 0) {
					bricks[i].x1 = bricks[i].x1 + 5;
					bricks[i].x2 = bricks[i].x1 + length_x;
				}
				else {
					bricks[i].x1 = bricks[i].x1 - 5;
					bricks[i].x2 = bricks[i].x1 + length_x;
				}
			}
			if (bricks[6].x2 > 490) {
				brick_rl = 1;
			}
			else if (bricks[0].x1 < 0) {
				brick_rl = 0;
			}

			// player 이동
			switch (player_is_down)
			{
			case 1:	// 하행
				switch (player_speed)
				{
				case 1:
					player_y1 = player_y1 + 10;
					player_y2 = player_y1 + grid;
					break;
				case 2:
					player_y1 = player_y1 + 10;
					player_y2 = player_y1 + grid;
					degree = 45;
					break;
				case 3:
					player_y1 = player_y1 + 10;
					player_y2 = player_y1 + grid;
					break;
				}
				break;
			case 0:	// 상행
				// player 반사
				switch (player_rl)
				{
				case 0:	// 오른쪽
					switch (player_speed)
					{
					case 1:
						player_x1 = player_x1 + (grid * 2);
						player_x2 = player_x1 + grid;
						player_y1 = player_y1 - grid;
						player_y2 = player_y1 + grid;
						break;
					case 2:
						player_x1 = player_x1 + grid;
						player_x2 = player_x1 + grid;
						player_y1 = player_y1 - grid;
						player_y2 = player_y1 + grid;
						break;
					case 3:
						player_x1 = player_x1 + grid;
						player_x2 = player_x1 + grid;
						player_y1 = player_y1 - (grid * 2);
						player_y2 = player_y1 + grid;
						break;
					}
					break;
				case 1:	// 왼쪽
					switch (player_speed)
					{
					case 1:
						player_x1 = player_x1 - (grid * 2);
						player_x2 = player_x1 + grid;
						player_y1 = player_y1 - grid;
						player_y2 = player_y1 + grid;
						break;
					case 2:
						player_x1 = player_x1 - grid;
						player_x2 = player_x1 + grid;
						player_y1 = player_y1 - grid;
						player_y2 = player_y1 + grid;
						break;
					case 3:
						player_x1 = player_x1 - grid;
						player_x2 = player_x1 + grid;
						player_y1 = player_y1 - (grid * 2);
						player_y2 = player_y1 + grid;
						break;
					}
					break;
				}
				break;
			}
			// 충돌체크
			// brick disable
			for (int i = 0; i < BRICK_X * BRICK_Y; i++) {
				if (bricks[i].is_disable == 1) { continue; }
				if (player_y2 >= bricks[i].y1 && player_y2 <= bricks[i].y2) {
					if ((player_x1 >= bricks[i].x1 && player_x1 <= bricks[i].x2) || (player_x2 >= bricks[i].x1 && player_x2 <= bricks[i].x2)) {
						bricks[i].is_disable = 1;
						player_is_down = 0;
						player_speed = rand() % 3 + 1;
						player_rl = rand() % 2;
						lost_bricks++;
					}
				}
			}
			// 테두리 체크
			if (player_y1 <= 0) {			// 맨 위
				player_is_down = 1;
			}
			if (player_y1 >= WINDOW_Y) {	// 맨 아래
				player_y1 = 0;
				player_y2 = player_y1 * grid;
			}
			if (player_x2 >= 490) {	// 오른쪽
				player_rl = 1;
			}
			if (player_x1 <= 0) {	// 왼쪽
				player_rl = 0;
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'q':
		case 'Q':
			PostQuitMessage(1);
			break;
		case 's':
		case 'S':
			if(!is_start){ 
				player_speed = rand() % 3 + 1;
				player_rl = rand() % 2;
				SetTimer(hWnd, 1, speed, NULL); 
			}
			is_start++;			
			break;
		case 'p':
		case 'P':
			if (!is_start) { break; }
			if (!is_paused) {
				KillTimer(hWnd, 1);
				is_paused++;
				wsprintf(message, L"사라진 블럭 : %d", lost_bricks);
				MessageBox(hWnd, message, L"정보", MB_OK);
			}
			else {
				SetTimer(hWnd, 1, speed, NULL);
				is_paused = 0;
			}
			break;
		case 'n':
		case 'N':
			if (!is_start) { break; }
			KillTimer(hWnd, 1);
			is_start = 0;
			is_paused = 0;
			speed = 150;
			player_x1 = 230;
			player_y1 = 0;
			player_x2 = player_x1 + grid;
			player_y2 = player_y1 + grid;
			brick_rl = 0;
			lost_bricks = 0;
			InitBRICK(bricks, grid, length_x, startX, startY);
			break;
		case '+':
		case '=':
			if (!is_start) { break; }
			if (speed < 100) {
				break;
			}
			speed = speed - 10;
			SetTimer(hWnd, 1, speed, NULL);
			break;
		case '-':
		case '_':
			if (!is_start) { break; }
			if (speed > 200) {
				break;
			}
			speed = speed + 10;
			SetTimer(hWnd, 1, speed, NULL);
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		
		SelectObject(hDC, playerBrush);
		Ellipse(hDC, player_x1, player_y1, player_x2, player_y2);

		SelectObject(hDC, brickBrush);
		for (int i = 0; i < BRICKS; i++) {
			if (!bricks[i].is_disable) {
				Rectangle(hDC, bricks[i].x1, bricks[i].y1, bricks[i].x2, bricks[i].y2);
			}
		}

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
		DeleteObject(brickBrush);
		DeleteObject(playerBrush);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}