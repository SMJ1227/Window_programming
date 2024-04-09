#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WINDOW_X 500
#define WINDOW_Y 500
#define grid 40
#define OBSTACLE 20

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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 495, 520, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

#define M_PI 3.14

typedef struct STONES {
	int x;
	int y;
	int x1;
	int y1;
	int x2;
	int y2;
	int type;
};

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

void DrawCircle(HDC hdc, int x1, int y1, int x2, int y2) {
	Ellipse(hdc, x1, y1, x2, y2);
}

void DrawGrid(HDC hdc) {
	for (double i = 0; i < grid; i++) {
		DrawLine(hdc, 0, i * (WINDOW_Y / grid), WINDOW_X, i * (WINDOW_Y / grid));
	}
	for (double j = 0; j < grid; j++) {
		DrawLine(hdc, j * (WINDOW_X / grid), 0, j * (WINDOW_X / grid), WINDOW_Y);
	}
}

void ShowMessageBoxWIN(HWND hWnd) {
	MessageBox(hWnd, L"성공", L"성공", MB_OK);
}

void ShowMessageBoxLOSEColor(HWND hWnd) {
	MessageBox(hWnd, L"색이 다름", L"실패", MB_OK);
}

void ShowMessageBoxLOSEShape(HWND hWnd) {
	MessageBox(hWnd, L"모양이 다름", L"실패", MB_OK);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	srand((unsigned int)time(NULL));

	static COLORREF random_brush[OBSTACLE];
	static COLORREF player_color = RGB(rand() % 256, rand() % 256, rand() % 256);
	static COLORREF end_color;
	static SIZE size;
	static HPEN linePen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	static HBRUSH playerBrush;
	static HBRUSH obstacleBrush;
		
	static int angle = 4;
	static int cellWidth = WINDOW_X / grid;
	static int cellHeight = WINDOW_Y / grid;
	static int player_width = WINDOW_X / grid;
	static int player_height = WINDOW_Y / grid;
	static int player_x = 0;
	static int player_y = 0;
	static int player_x1 = player_x * player_width;
	static int player_y1 = player_y * player_height;
	static int player_x2 = player_x1 + player_width;
	static int player_y2 = player_y1 + player_height;
	static char previous_char;
	
	static STONES stones[OBSTACLE];
	
	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		for (int i = 0; i < OBSTACLE; i++) {
			bool is_colliding = true;
			while (is_colliding) {
				stones[i].x = rand() % grid;
				stones[i].y = rand() % grid;
				is_colliding = false;
				if (stones[i].x == 0 && stones[i].y == 0) {
					is_colliding = true;
				}
				else if ((stones[i].x == (grid - 1) && stones[i].y == (grid - 1))) {
					is_colliding = true;
				}
				else {
					for (int j = 0; j < i; j++) {
						if (stones[i].x == stones[j].x && stones[i].y == stones[j].y) {
							is_colliding = true;
							break;
						}
					}
				}
			}
			stones[i].x1 = stones[i].x * cellWidth;
			stones[i].y1 = stones[i].y * cellHeight;
			stones[i].x2 = stones[i].x1 + cellWidth;
			stones[i].y2 = stones[i].y1 + cellHeight;
			stones[i].type = rand() % 5 + 1;
		}
		for (int i = 0; i < OBSTACLE; i++) {
			random_brush[i] = RGB(rand() % 255, rand() % 255, rand() % 255);
		}
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		switch (wParam)
		{
		case 'r':	// 재시작
			player_x = 0;
			player_y = 0;
			break;
		case 'w':
			if (player_y <= 0) {
				player_y = grid - 1;
			}
			else {
				player_y--;
			}
			previous_char = 'w';
			break;
		case 'a':
			if (player_x <= 0) {
				player_x = grid - 1;
			}
			else {
				player_x--;
			}
			previous_char = 'a';
			break;
		case 's':
			if (player_y >= grid - 1) {
				player_y = 0;
			}
			else {
				player_y++;
			}
			previous_char = 's';
			break;
		case 'd':
			if (player_x >= grid - 1) {
				player_x = 0;
			}
			else {
				player_x++;
			}
			previous_char = 'd';
			break;
		case 'q':
			PostQuitMessage(0);
			break;
		}

		// 장애물 충돌처리
		for (int i = 0; i < OBSTACLE; i++) {
			if (player_x == stones[i].x && player_y == stones[i].y) {
				switch (stones[i].type)
				{
				case 1:	// 장애물// 빨강
					switch (previous_char)
					{
					case 'w':
						if (player_y == grid - 1) {
							player_y = 0;
						}
						else {
							player_y++;
						}
						break;
					case 'a':
						if (player_x == grid - 1) {
							player_x = 0;
						}
						else {
							player_x++;
						}
						break;
					case 's':
						if (player_y == 0) {
							player_y = grid - 1;
						}
						else {
							player_y--;
						}
						break;
					case 'd':
						if (player_x == 0) {
							player_x = grid - 1;
						}
						else {
							player_x--;
						}
						break;
					}
					break;
				case 2:	// 색상 변경
					player_color = random_brush[i];
					break;
				case 3:	// 축소// 노랑
					if (player_width > 3) {
						player_width--;
						player_height--;
					}
					else {
						player_width++;
						player_height++;
					}
					break;
				case 4:	// 확대// 파랑
					player_width++;
					player_height++;
					if (player_width > 12) {
						player_width--;
						player_height--;
					}
					break;
				case 5:	// 모양 변경// 하늘
					angle = rand() % 4 + 3;
					break;
				}
			}
		}
		player_x1 = player_x * cellWidth;
		player_y1 = player_y * cellHeight;
		player_x2 = player_x1 + player_width;
		player_y2 = player_y1 + player_height;

		// 끝
		if (player_x == (grid - 1) && player_y == (grid - 1)) {
			if(player_color != end_color){ ShowMessageBoxLOSEColor(hWnd); }
			else if(angle != 4){ ShowMessageBoxLOSEShape(hWnd); }
			else { 
				ShowMessageBoxWIN(hWnd);
				DeleteObject(playerBrush);
				PostQuitMessage(0);
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC);

		// OBSTACLE
		for (int i = 0; i < OBSTACLE; i++) {
			switch (stones[i].type)
			{
			case 1:	// 장애물
				random_brush[i] = RGB(255, 0, 0);	// 빨강
				obstacleBrush = CreateSolidBrush(random_brush[i]);
				SelectObject(hDC, obstacleBrush);
				DrawRectangle(hDC, stones[i].x * cellWidth, stones[i].y * cellHeight, 
							(stones[i].x + 1) * cellWidth, (stones[i].y + 1) * cellHeight); 
				DeleteObject(obstacleBrush);
				break;
			case 2:	// 색상 변경
				end_color = random_brush[i];
				obstacleBrush = CreateSolidBrush(random_brush[i]);
				SelectObject(hDC, obstacleBrush);
				DrawRectangle(hDC, stones[i].x * cellWidth, stones[i].y * cellHeight,
					(stones[i].x + 1) * cellWidth, (stones[i].y + 1) * cellHeight);
				DrawRectangle(hDC, (grid - 1) * cellWidth, (grid - 1) * cellHeight, grid * cellWidth, grid * cellHeight); // goal
				DeleteObject(obstacleBrush);
				break;
			case 3:	// 축소
				random_brush[i] = RGB(255, 255, 0);	// 노랑
				obstacleBrush = CreateSolidBrush(random_brush[i]);
				SelectObject(hDC, obstacleBrush);
				DrawRectangle(hDC, stones[i].x * cellWidth, stones[i].y * cellHeight,
					(stones[i].x + 1) * cellWidth, (stones[i].y + 1) * cellHeight);
				DeleteObject(obstacleBrush);
				break;
			case 4:	// 확대
				random_brush[i] = RGB(0, 0, 255);	// 파랑
				obstacleBrush = CreateSolidBrush(random_brush[i]);
				SelectObject(hDC, obstacleBrush);
				DrawRectangle(hDC, stones[i].x * cellWidth, stones[i].y * cellHeight,
					(stones[i].x + 1) * cellWidth, (stones[i].y + 1) * cellHeight);
				DeleteObject(obstacleBrush);
				break;
			case 5:	// 모양 변경
				random_brush[i] = RGB(0, 255, 255);	// 하늘
				obstacleBrush = CreateSolidBrush(random_brush[i]);
				SelectObject(hDC, obstacleBrush);
				DrawRectangle(hDC, stones[i].x * cellWidth, stones[i].y * cellHeight,
					(stones[i].x + 1) * cellWidth, (stones[i].y + 1) * cellHeight);
				DeleteObject(obstacleBrush);
				break;
			}
		}
		
		// PLAYER
		playerBrush = CreateSolidBrush(player_color);
		SelectObject(hDC, playerBrush);
		switch (angle)
		{
		case 3:
			DrawTriangle(hDC, player_x1, player_y1, player_x2, player_y2);
			break;
		case 4:
			DrawRectangle(hDC, player_x1, player_y1, player_x2, player_y2);
			break;
		case 5:
			DrawPentagon(hDC, player_x1, player_y1, player_x2, player_y2);
			break;
		case 6:
			DrawCircle(hDC, player_x1, player_y1, player_x2, player_y2);
			break;
		}

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER:
		hDC = GetDC(hWnd);
		ReleaseDC(hWnd, hDC);
		break;
	case WM_DESTROY:
		DeleteObject(playerBrush);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}