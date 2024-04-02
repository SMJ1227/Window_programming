#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

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

# define MAX_X 31
# define MAX_Y 10

void delete_char(TCHAR str[MAX_Y][MAX_X], int y, int spaceCount) {
	while (str[y][spaceCount] != '\0') {
		str[y][spaceCount] = str[y][spaceCount + 1];
		spaceCount++;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;
	static TCHAR str[MAX_Y][MAX_X];
	static SIZE size;

	srand((unsigned int)time(NULL));

	static int y = 0;
	static int count = 0;
	static int insert = 0;
	static int upper = 0;
	static int spaceCount = 0;
	static int firstSpace = 0;
	static int nextSpace = 0;

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		hDC = GetDC(hWnd);
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:		//캐럿이 이동
			if (count == 0 && y > 0) {
				y--;
				count = lstrlen(str[y]);
			}
			else if (count == 0 && y == 0) {
				y = MAX_Y - 1;
				count = lstrlen(str[y]);
			}
			else {
				count--;
			}
			break;
		case VK_UP:
			if (y == 0) {
				y = MAX_Y - 1;
				if (count > lstrlen(str[y])) {
					count = lstrlen(str[y]);
				}
			}
			else {
				y--;
				if (count > lstrlen(str[y])) {
					count = lstrlen(str[y]);
				}
			}
			break;
		case VK_RIGHT:
			if (count == lstrlen(str[y]) && y >= MAX_Y - 1) {
				y = 0;
				count = 0;
			}
			else if (count == lstrlen(str[y])) {
				y++;
				count = 0;
			}
			else {
				count++;
			}
			break;
		case VK_DOWN:
			if (y == MAX_Y - 1) {
				y = 0;
				if (count > lstrlen(str[y])) {
					count = lstrlen(str[y]);
				}
			}
			else {
				y++;
				if (count > lstrlen(str[y])) {
					count = lstrlen(str[y]);
				}
			}
			break;
		case VK_F1:			//입력하는 문자가 대문자. 다시 누르면 원상태
			if (upper == 0) {
				upper = 1;
			}
			else {
				upper = 0;
			}
			break;
		case VK_F11:
			firstSpace = count;
			while (str[y][firstSpace] != ' ') {
				firstSpace--;
				if (firstSpace == 0) {
					break;
				}
			}
			nextSpace = count;
			while (str[y][nextSpace] != '\0' && str[y][nextSpace] != ' ') {
				nextSpace++;
				if (lstrlen(str[y]) == nextSpace) {
					break;
				}
			}
			for (int i = 0; i < (nextSpace - firstSpace); i++) {
				delete_char(str, y, firstSpace);
			}
			if (count > lstrlen(str[y])) {
				count = lstrlen(str[y]);
			}
			break;
		case VK_F12:			//종료
			PostQuitMessage(0);
			break;
		case VK_DELETE:			// 캐럿 뒤 글자 삭제
			spaceCount = count;
			delete_char(str, y, spaceCount);
			break;
		case VK_HOME:		// 캐럿이 그 줄 맨 앞으로
			count = 0;
			break;
		case VK_END:		//캐럿이 그 줄 맨뒤로
			count = lstrlen(str[y]);
			break;
		case VK_INSERT:		// 캐럿 위치에 문자 추가. 다시 누르면 원상 복귀
			if (insert == 0) {
				insert = 1;
			}
			else {
				insert = 0;
			}
			break;
		default:
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CHAR:
		switch (wParam) {
		case VK_ESCAPE:		// 화면이 다 지워지고 캐럿은 맨 윗줄 앞에 있다.
			for (int i = 0; i < MAX_Y; i++) {
				for (int j = 0; j < MAX_X; j++) {
					str[i][j] = '\0';
				}
			}
			y = 0;
			count = 0;
			break;
		case VK_RETURN:
			y = (y + 1) % MAX_Y;
			count = 0;
			break;
		case VK_BACK:
			if (count < 1 && y < 1) {
				count = 1;
			}
			if (count < 1 && y > 0) {
				y = (y - 1) % MAX_Y;
				count = lstrlen(str[y]);
				str[y][count] = '\0';
			}
			else {
				for (int i = count - 1; i < lstrlen(str[y]); i++) {
					str[y][i] = str[y][i + 1];
				}
				count--;
			}
			break;
		case VK_TAB:		// 5개의 스페이스가 삽입 캐럿도
			for (int i = 0; i < 5; i++) {
				if (count < 30) {
					if (insert == 0) {
						str[y][count++] = ' ';
					}
					else {
						str[y][++count] = ' ';
					}
				}
				else {
					y = (y + 1) % MAX_Y;
					count = 0;
					str[y][count++] = ' ';
				}
			}
			break;
		default:
			if (count < 30) {
				if (insert == 0) {
					if (upper == 0) {
						str[y][count++] = wParam;
					}
					else {
						str[y][count++] = toupper(wParam);
					}	
				}
				else {
					if (upper == 0) {
						for (int i = lstrlen(str[y]); i >= count; i--) {
							str[y][i + 1] = str[y][i];
						}
						str[y][count++] = wParam;
					}
					else {
						for (int i = lstrlen(str[y]); i >= count; i--) {
							str[y][i + 1] = str[y][i];
						}
						str[y][count++] = toupper(wParam);
					}
				}
			}
			else {
				// 다음 줄로 넘어가기
				y = (y + 1) % MAX_Y;
				count = 0;
				str[y][count++] = wParam;
			}
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		for (int i = 0; i < MAX_Y; i++) {
			GetTextExtentPoint32(hDC, str[i], count, &size);
			if (lstrlen(str[i]) > 30) {
				TextOut(hDC, 0, i * 20, str[i], 30);
			}
			else {
				TextOut(hDC, 0, i * 20, str[i], lstrlen(str[i]));
			}
			if (i == y) {
				SetCaretPos(size.cx, i * 20);
			}
		}
		EndPaint(hWnd, &ps);
		break;
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