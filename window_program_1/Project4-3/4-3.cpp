#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "resource.h"

#define WINDOW_X 300
#define WINDOW_Y 600
#define GRID 50
#define GRID_X 6
#define GRID_Y 12
#define M_PI 3.14
#define TIMER_ID 1

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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 315, 658, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

typedef enum BLOCKTYPE{
	O_BLOCK,
	I_BLOCK,
	Z_BLOCK,
	J_BLOCK,
	U_BLOCK,
	T_BLOCK
};

typedef struct BLOCKS {
	int x;
	int y;
	BLOCKTYPE type;
	COLORREF colors[3][3]; // 각 칸의 색상을 나타내는 배열
	int shape[3][3]; // 블록의 모양을 나타내는 배열
} TetrisBlock;

int blocks[6][3][3] = {
	{	{1, 1, 0},	// O 블록
		{1, 1, 0},
		{0, 0, 0},
	},
	{	{0, 0, 0},	// I 블록
		{1, 1, 1},
		{0, 0, 0}
	},
	{	{1, 1, 0},	// Z 블록
		{0, 1, 1},
		{0, 0, 0}
	},
	{	{1, 0, 0},	// J 블록
		{1, 1, 1},
		{0, 0, 0}
	},
	{	{0, 0, 0},	// U 블록
		{1, 1, 1},
		{1, 0, 1}
	},
	{	{0, 0, 0},	// T 블록
		{1, 1, 1},
		{0, 1, 0}
	}
};

TetrisBlock createRandomBlock(int x, int y, int randoms, int color_num) {	
	int randomType = rand() % randoms + (6-randoms);
	TetrisBlock block;
	block.x = x;
	block.y = 0;
	block.type = (BLOCKTYPE)randomType;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			block.shape[i][j] = blocks[randomType][i][j];
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (block.shape[i][j] == 1) {
				switch (color_num)
				{
				case 0:
					block.colors[i][j] = RGB(rand() % 256, rand() % 256, rand() % 256);
					break;
				case 1:
					block.colors[i][j] = RGB(255, 0, 0);
					break;
				case 2:
					block.colors[i][j] = RGB(0, 255, 0);
					break;
				case 3:
					block.colors[i][j] = RGB(0, 0, 255);
					break;
				}
			}
		}
	}

	return block;
}

void drawBlock(HDC hdc, TetrisBlock block) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (block.shape[i][j] == 1) {
				RECT rect = { block.x + j * GRID, block.y + i * GRID, block.x + (j + 1) * GRID, block.y + (i + 1) * GRID };
				HBRUSH hBrush = CreateSolidBrush(block.colors[i][j]);
				FillRect(hdc, &rect, hBrush);
				DeleteObject(hBrush);
			}
		}
	}
}

void rotateBlock(TetrisBlock* block) {
	int tempShape[3][3];
	COLORREF tempColors[3][3];

	// 현재 블록의 모양을 임시 배열에 복사
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			tempShape[i][j] = block->shape[i][j];
			tempColors[i][j] = block->colors[i][j];
		}
	}

	// 블록을 시계 방향으로 90도 회전
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			block->shape[i][j] = tempShape[2 - j][i];
			block->colors[i][j]= tempColors[2 - j][i];
		}
	}
}

typedef struct GRIDS {
	COLORREF color;
	BOOL disable;
}Grids;

// 그리드를 초기화하는 함수
void createGrid(GRIDS grids[GRID_X][GRID_Y]) {
	for (int i = 0; i < GRID_X; i++) {
		for (int j = 0; j < GRID_Y; j++) {
			grids[i][j].disable = FALSE;
			grids[i][j].color = RGB(0, 255, 255);
		}
	}
}

void DrawGrid(HDC hdc, GRIDS grids[GRID_X][GRID_Y]) {
	for (int i = 0; i < GRID_X; i++) {
		for (int j = 0; j < GRID_Y; j++) {
			RECT rect = { i * GRID, j * GRID, (i + 1) * GRID, (j + 1) * GRID };
			HBRUSH hBrush = CreateSolidBrush(grids[i][j].color);
			FillRect(hdc, &rect, hBrush);
			DeleteObject(hBrush);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;
	HPEN hPen;
	srand((unsigned int)time(NULL));
	
	static int TIMER_INTERVAL = 500;
	static int randoms = 4;
	static int color_num = 0;

	static TetrisBlock randomBlock;
	static int collide_floor = 2;
	static int collide_right = 2;
	static int collide_left = 0;
	static BOOL bottomTouch = FALSE;
	static BOOL isCollide = FALSE;

	static GRIDS grids[GRID_X][GRID_Y];

		//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		createGrid(grids);
		break;
	case WM_COMMAND: //--- 메뉴를 선택했을 때
		switch (LOWORD(wParam)) {
		case ID_GAME_START:
			SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);
			randomBlock = createRandomBlock(0, 0, randoms, color_num);
			break;
		case ID_GAME_END:
			PostQuitMessage(0);
			break;
		case ID_BLOCK_4:
			randoms = 2;
			break;
		case ID_BLOCK_5:
			randoms = 3;
			break;
		case ID_BLOCK_6:
			randoms = 4;
			break;
		case ID_COLOR_RED:
			color_num = 1;
			break;
		case ID_COLOR_GREEN:
			color_num = 2;
			break;
		case ID_COLOR_BLUE:
			color_num = 3;
			break;
		case ID_SPEED_FAST:
			KillTimer(hWnd, TIMER_ID);
			TIMER_INTERVAL = 250;
			SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);
			break;
		case ID_SPEED_MEDIUM:
			KillTimer(hWnd, TIMER_ID);
			TIMER_INTERVAL = 500;
			SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);
			break;
		case ID_SPEED_SLOW:
			KillTimer(hWnd, TIMER_ID);
			TIMER_INTERVAL = 750;
			SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			rotateBlock(&randomBlock);
			break;
		case VK_RIGHT:
			for (int j = 2; j > 0; j--) {
				for (int i = 0; i < 3; i++) {
					if (randomBlock.shape[i][j] == 1) {
						collide_right = j;
						isCollide = TRUE;
						break;
					}
				}
				if (isCollide) break;
			}
			if (randomBlock.x + (collide_right + 1) * GRID >= 315 - GRID) {
				isCollide = FALSE;
				break;
			}
			randomBlock.x += GRID;
			break;
		case VK_LEFT:
			if (randomBlock.x <= 0) {
				break;
			}
			randomBlock.x -= GRID;
			break;
		case VK_DOWN:
			randomBlock.y += GRID;
			break;
		}
	case WM_TIMER:
		// 타이머 메시지 처리
		switch (wParam)
		{
		case TIMER_ID:
			randomBlock.y += GRID;
			int gridX = randomBlock.x / GRID;
			int gridY = randomBlock.y / GRID;
			if (randomBlock.y + 3 * GRID >= 658 - GRID) {
				// 블록의 한 줄을 순회하면서 맨 아래 줄에 닿았는지 확인
				for (int i = 0; i < 3; i++) {
					if (randomBlock.shape[collide_floor][i] == 1) {
						bottomTouch = TRUE;
						break;
					}
				}
				collide_floor--;
				if (bottomTouch) {
					// 바닥에 닿으면 새로운 블록 생성
					collide_floor = 2;
					bottomTouch = FALSE;
					// 색과 비활성화 상태를 적용
					for (int i = 0; i < 3; i++) {
						for (int j = 0; j < 3; j++) {
							if (randomBlock.shape[i][j] == 1) {
								grids[gridX+j][gridY+i-1].color = randomBlock.colors[i][j]; // 블록의 색을 그리드에 적용
								grids[gridX+j][gridY+i-1].disable = TRUE; // 그리드 비활성화
							}
						}
					}
					randomBlock = createRandomBlock(0, 0, randoms, color_num);
					break;
				}
			}
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					if (randomBlock.shape[i][j] == 1 && grids[gridX + j][gridY + i].disable == TRUE) {
						for (int i = 0; i < 3; i++) {
							for (int j = 0; j < 3; j++) {
								if (randomBlock.shape[i][j] == 1) {
									grids[gridX + j][gridY + i - 1].color = randomBlock.colors[i][j]; // 블록의 색을 그리드에 적용
									grids[gridX + j][gridY + i - 1].disable = TRUE; // 그리드 비활성화
								}
							}
						}
						randomBlock = createRandomBlock(0, 0, randoms, color_num);
						break;
					}
				}
			}
		}
		// 줄처리
		for (int i = 0; i < GRID_Y; i++) {
			int count = 0;
			COLORREF prevColor = RGB(0, 255, 255);
			// 같은 y축에 있는 모든 요소의 color 확인
			for (int j = 0; j < GRID_X; j++) {
				if (grids[j][i].color != prevColor) {
					count++;
					if (count >= 2) {
						for (int k = 0; k < GRID_X; k++) {
							grids[k][i].disable = FALSE;
							grids[k][i].color = RGB(0, 255, 255);
						}
					}
				}
				else { count = 0; }
			}
		}
		
		// 줄처리
		for (int i = 0; i < GRID_Y; i++) {
			// 같은 y축에 있는 모든 요소의 disable이 TRUE인지 확인
			BOOL allDisabled = TRUE;
			for (int j = 0; j < GRID_X; j++) {
				if (!grids[j][i].disable) {
					allDisabled = FALSE;
					break;
				}
			}
			// 같은 y축에 있는 모든 요소의 disable이 TRUE라면 해당 줄의 모든 요소의 disable을 FALSE로 설정
			if (allDisabled) {
				for (int j = 0; j < GRID_X; j++) {
					grids[j][i].disable = FALSE;
					grids[j][i].color = RGB(0, 255, 255);
				}
			}
		}
		InvalidateRect(hWnd, NULL, TRUE); // 화면 다시 그리기
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		DrawGrid(hDC, grids);

		drawBlock(hDC, randomBlock);
		
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, IParam);		//---위의 세 메시지 외의 나머지 메세지는 OS로
}