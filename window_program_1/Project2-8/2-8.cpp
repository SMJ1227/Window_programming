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

void copy_str(TCHAR str[MAX_Y][MAX_X], TCHAR copy[MAX_Y][MAX_X]) {
	for (int i = 0; i < MAX_Y; i++) {
		for (int j = 0; j < MAX_X; j++) {
			copy[i][j] = str[i][j];
		}
	}
}

void copy_copy(TCHAR str[MAX_Y][MAX_X], TCHAR copy[MAX_Y][MAX_X]) {
	for (int i = 0; i < MAX_Y; i++) {
		for (int j = 0; j < MAX_X; j++) {
			str[i][j] = copy[i][j];
		}
	}
}

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
	static TCHAR copy[MAX_Y][MAX_X];
	static SIZE size;

	srand((unsigned int)time(NULL));

	static int y = 0;
	static int count = 0;
	static int insert = 0;
	static int upper = 0;
	static int spaceCount = 0;
	static int firstSpace = 0;
	static int nextSpace = 0;
	static int f2 = 0;
	static int pageUp = 0;
	static int pageDown = 0;

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
		case VK_OEM_MINUS:	// 아스키 이전 문자로
			for (int i = 0; i < MAX_Y; i++) {
				for (int j = 0; j < lstrlen(str[i]); j++) {
					if (str[i][j] == ' ' || str[i][j] == '\0') {
						break;
					}
					else {
						if (str[i][j] == 'a') {
							str[i][j] = 'z';
						}
						else if (str[i][j] == '0') {
							str[i][j] = '9';
						}
						else {
							str[i][j] = str[i][j] - 1;
						}
					}
				}
			}
			break;
		case VK_OEM_PLUS:	// 아스키 다음 문자로
			for (int i = 0; i < MAX_Y; i++) {
				for (int j = 0; j < lstrlen(str[i]); j++) {
					if (str[i][j] == ' ' || str[i][j] == '\0') {
						break;
					}
					else {
						if (str[i][j] == 'z') {
							str[i][j] = 'a';
						}
						else if (str[i][j] == '9') {
							str[i][j] = '0';
						}
						else {
							str[i][j] = str[i][j] + 1;
						}
					}
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
		case VK_F2:	// 모든 줄에서 앞에 4칸에 _를 넣는다. 다시 누르면 사라짐
			if (f2 == 0) {
				copy_str(str, copy);
				for (int k = 0; k < MAX_Y; k++) {
					for (int j = 0; j < 4; j++) {
						for (int i = MAX_X - 2; i >= 0; i--) {
							str[k][i + 1] = str[k][i];
						}
						str[k][0] = '_';	
					}
				}
				f2++;
			}
			else {
				copy_copy(str, copy);
				f2--;
			}
			break;
		case VK_F3:	// 모든 줄을 한 줄 아래로 출력한다. 모든 줄이 차 있다면 밀리는 한 줄은 삭제.
			for (int i = MAX_Y - 1; i > 0; i--) {
				for (int j = 0; j < MAX_X; j++) {
					str[i][j] = str[i - 1][j];
				}
			}
			for (int j = 0; j < MAX_X; j++) {
				str[0][j] = '\0';
			}
			if (count > lstrlen(str[y])) {
				count = 0;
			}
			break;
		case VK_F4:	// 모든 줄의 순서를 뒤바꾼다. 1234 -> 4321
			copy_str(str, copy);
			for (int i = 0; i < MAX_Y; i++) {
				for (int j = 0; j < MAX_X; j++) {
					str[i][j] = copy[MAX_Y - i - 1][j];
				}
			}
			break;
		case VK_F11:	// 원래 delete
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
		case VK_INSERT:		// 캐럿 위치에 문자 추가. 다시 누르면 원상 복귀
			if (insert == 0) {
				insert = 1;
			}
			else {
				insert = 0;
			}
			break;
		case VK_HOME:		// 캐럿이 그 줄 맨 앞으로
			count = 0;
			break;
		case VK_END:		//캐럿이 그 줄 맨뒤로대문자로 치환
			count = lstrlen(str[y]);
			break;
		case VK_PRIOR:	//공백자리에 )( 삽입, 
			if (pageUp == 0) {
				copy_str(str, copy);
				for (int k = 0; k < MAX_Y; k++) {
					if (str[k][0] != ' ' && str[k][0] != '\0') {
						for (int j = MAX_X - 2; j >= 0; j--) {
							str[k][j + 1] = str[k][j];
						}
						str[k][0] = '(';
					}
					for (int i = 0; i < lstrlen(str[k]); i++) {
						if (str[k][i] == ' ') {
							for (int j = MAX_X - 2; j > i; j--) {
								str[k][j + 1] = str[k][j];
							}
							str[k][i] = ')';
							str[k][i + 1] = '(';
						}
					}
					if (lstrlen(str[k]) < MAX_X - 1 && str[k][0] != '\0') {
						str[k][lstrlen(str[k])] = ')';
					}
				}
				pageUp++;
			}
			else {
				copy_copy(str, copy);
				pageUp--;
			}
			break;
		case VK_NEXT:	// 공백 삭제
			if (pageDown == 0) {
				copy_str(str, copy);
				for (int k = 0; k < MAX_Y; k++) {
					for (int i = 0; i < lstrlen(str[k]); i++) {
						if (str[k][i] == ' ') {
							for (int j = i; j < lstrlen(str[k]); j++) {
								str[k][j] = str[k][j + 1];
							}
							i--;
						}
					}
					str[k][lstrlen(str[k])] = '\0';
				}
				count = lstrlen(str[y]);
				pageDown++;
			}
			else {
				copy_copy(str, copy);
				count = lstrlen(str[y]);
				pageDown--;
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
		case '+':
		case '=':
		case '-':
		case '_':
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