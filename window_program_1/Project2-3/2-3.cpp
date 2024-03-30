#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam) {
	PAINTSTRUCT ps;
	HDC hDC;
	RECT rect;
	srand((unsigned int)time(NULL));

	TCHAR str[500];
	int x = rand() % 601;
	int y = rand() % 401;
	int n = rand() % 10;
	int count = rand() % 16 + 5; 

	COLORREF background = RGB(rand() % 256, rand() % 256, rand() % 256);
	HBRUSH hBrush = CreateSolidBrush(background);
	
	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_CHAR:
		hDC = GetDC(hWnd);
		if (wParam == 'q') { PostQuitMessage(0); }
		else if (wParam == VK_RETURN) {
			int x = rand() % 601;
			int y = rand() % 401;
			int n = rand() % 10;
			int count = rand() % 16 + 5;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		rect.left = x;
		rect.top = y;
		rect.right = x + (count*19.5);
		rect.bottom = y + (count*20);
		SetTextColor(hDC, RGB(rand() % 256, rand() % 256, rand() % 256));
		FillRect(hDC, &rect, hBrush);
		SetBkColor(hDC, background);
		for (int i = 0; i < count; i++) {
			for (int j = 0; j < count; j++) {
				wsprintf(str, L"%d", n);
				TextOut(hDC, x + i * 20, y + j * 20, str, lstrlen(str));
			}
		}
		EndPaint(hWnd, &ps);
		break;
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