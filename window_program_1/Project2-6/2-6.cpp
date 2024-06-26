#include <windows.h>
#include <tchar.h>
#include <time.h>

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

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 100, 50, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
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
	int n = rand() % 8 + 2;
	int m = rand() % 8 + 2;
	int width = 800 / n;
	int height = 600 / m;
	TCHAR t_image[100];
	char image[6][100] = {"@   @\n @ @ \n  @  \n @ @ \n@   @",
						"  #  \n # # \n#####\n  #  \n # # \n#####",
						"*    *\n** **\n* * *\n** **\n*   *",
						"-----\n    |\n-----\n|    \n-----",
						"  `  \n ` ` \n`   `\n ` ` \n  `  ",
						"^ ^ ^\n^ ^ ^\n^ ^ ^\n^ ^ ^\n^ ^ ^"};
	int random;
	//--- 메세지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				rect.left = i * width;
				rect.top = j * height;
				rect.right = (i + 1) * width;
				rect.bottom = (j + 1) * height;

				COLORREF frame = RGB(0, 0, 255);
				HBRUSH hBrush = CreateSolidBrush(frame);
				FrameRect(hDC, &rect, hBrush);
				DeleteObject(hBrush);
				random = rand() % 6;
				swprintf(t_image, 100, L"%hs", image[random]);
				DrawText(hDC, t_image, lstrlen(t_image), &rect, DT_CENTER);
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