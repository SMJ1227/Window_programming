#include <windows.h>
#include <tchar.h>
#include <time.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming 2";

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
	TCHAR str[100];
	int num;
	int x[11];
	int y[11];
	srand((unsigned int)time(NULL));

	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		num = _tcslen(str);
		TextOut(hDC, 400, 300, L"0: (400, 300)", strlen("0: (400, 300)"));
		for (int i = 1; i < 11; i++) {
			while (1) {
				int j;
				x[i] = rand() % 80 * 10;
				for (j = 0; j < i; j++) {
					if (x[j] - 10 < x[i] && x[i] < x[j] + 10) {
						x[i] = rand() % 80 * 10;
						break;
					}
				}
				if (j == i) {
					y[i] = rand() % 60 * 10;
					for (j = 0; j < i; j++) {
						if (x[j] - 10 < x[i] && x[i] < x[j] + 10) {
							x[i] = rand() % 60 * 10;
							break;
						}
					}
					if (j == i) {
						wsprintf(str, L"%d: (%d, %d)", i, x[i], y[i]);
						TextOut(hDC, x[i], y[i], str, lstrlen(str));
						break;
					}
				}
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