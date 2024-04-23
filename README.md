# ProtoType
```C++
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);                            // 본 구조체의 크기
	WndClass.style = CS_HREDRAW | CS_VREDRAW;                      // 출력 스타일
	WndClass.lpfnWndProc = (WNDPROC)WndProc;                       // 프로시저 함수
	WndClass.cbClsExtra = 0;                                       // 클래스 여분 메모리 - 사용안함
	WndClass.cbWndExtra = 0;                                       // 윈도우 여분 메모리 - 사용안함
	WndClass.hInstance = hInstance;                                // 윈도우 인스턴스
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);              // 아이콘
	WndClass.hCursor = LoadCursor(NULL, IDC_HAND);                 // 커서
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // 배경색
	WndClass.lpszMenuName = NULL;                                  // 메뉴 이름
	WndClass.lpszClassName = lpszClass;                            // 클래스 이름
	WndClass.hIconSm = LoadIcon(NULL, IDI_QUESTION);               // 작은 아이콘
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

  COLORREF color;
  HPEN hPen = CreatePen(PS_SOLID, 1, color);
  HBRUSH hBrush = CreateSolidBrush(color);

  switch(uMsg){
  case WM_CREATE:
    SetTimer(hWnd, 1, 150, NULL);
    break;
  case WM_LBUTTONDOWN:
		l_mouse_x = LOWORD(IParam);
		l_mouse_y = HIWORD(IParam);
    break;
  case WM_RBUTTONDOWN:
		r_mouse_x = LOWORD(IParam);
		r_mouse_y = HIWORD(IParam);
    break;
  case WM_TIMER:
    switch(wParam)
    {
    case 1:
      break;
    }
    InvalidateRect(hWnd, NULL, TRUE);
    break;
  case WM_CHAR:
    switch(wParam)
    {
    case 'q':
      PostQuitMessage(1);
      break;
    }
    InvalidateRect(hWnd, NULL, TRUE);
    break;
  case WM_KEYDOWN:  // KEYUP
		case VK_???:
			break;
    InvalidateRect(hWnd, NULL, TRUE);
    break;
  case WM_PAINT:
  {
    hDC = BeginPaint(hWnd, &ps);
    SelectObject(hDC, hBrush);
    // 그림 그리는 코드
    DeleteObject(hBrush);
    EndPaint(hWnd, &ps);
    break;
  }
  case WM_DESTROY:
    KillTimer(hWnd, 1);
    PostQuitMessage(0);
    break;
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
```
------------------------------------------------------------------------------
# 메세지 처리하기
```C++
* 문자열 다루기
|* 문자열 선언
TCHAR str_1[15] = _T(“나는 winple”);
TCHAR str_2[15] = L“나는 winple”;
TCHAR str_3[15] = TEXT(“나는 winple”);
|* 문자열 사용 함수
문자열 복사 strcpy _tcscpy
문자열을 c 만큼 복사 strncpy _tcsncpy
문자열 길이 strlen _tcslen
문자열 붙이기 strcat _tcscat
|* 문자열 만들기
str[count++] = wParam; //--- 문자저장 후 인덱스 증가
str[count] = '\0'; //--- 문자열은 null(‘\0’)로 끝남
wsprintf (str, L"%d", x);
str[y][--count] = '\0';  //--- 저장된 문자 지우기
```
----------------------------------------------------------------------------------
``` C++
* 1. 윈도우에 출력하기
TextOut (hDC, 0, 0, L"Hello World", strlen("Hello World")); // 문자열 직접 사용
TextOut (hDC, 0, 100, str, _tcslen (str)); // 변수 사용

* 2. 박스 영역에 출력하기
RECT rect;
rect.left = ???; //--- 사각형 정의
rect.top = ???;
rect.right = ???;
rect.bottom = ???;
FillRect(hDC, &rect, hBrush);  // 사각형 채우기
DrawText (hDC, L"HelloWorld", 10, &rect, ???);
??? = DT_VCENTER(세로로 가운데) | DT_CENTER(가로로 가운데) | DT_SINGLELINE(한 줄로만 출력) | DT_WORDBREAK | DT_EDITCONTROL(줄바꿈)

* 3. 출력 옵션
SetBkColor(hDC, RGB(255,0,0));        // 백그라운드 컬러
SetTextColor (hDC, RGB (255, 0, 0));  // 텍스트 컬러
```
