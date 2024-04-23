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

  static SIZE size;
  static TCHAR str[100];

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
1. 문자열 선언
TCHAR str_1[15] = _T(“나는 winple”);
TCHAR str_2[15] = L“나는 winple”;
TCHAR str_3[15] = TEXT(“나는 winple”);
2. 문자열 사용 함수
문자열 복사 strcpy _tcscpy
문자열을 c 만큼 복사 strncpy _tcsncpy
문자열 길이 strlen _tcslen
문자열 붙이기 strcat _tcscat
3. 문자열 만들기
str[count++] = wParam; //--- 문자저장 후 인덱스 증가
str[count] = '\0'; //--- 문자열은 null(‘\0’)로 끝남
wsprintf (str, L"%d", x);
str[y][--count] = '\0';  //--- 저장된 문자 지우기
```
----------------------------------------------------------------------------------
# 출력
``` C++
1. 윈도우에 출력하기
TextOut (hDC, 0, 0, L"Hello World", strlen("Hello World")); // 문자열 직접 사용
TextOut (hDC, 0, 100, str, _tcslen (str)); // 변수 사용

2. 박스 영역에 출력하기
RECT rect;
rect.left = ???; //--- 사각형 정의
rect.top = ???;
rect.right = ???;
rect.bottom = ???;
FillRect(hDC, &rect, hBrush);  // 사각형 채우기
FrameRect		       // 외각선 그리기 
InvertRect		       // 반전 사각형
DrawText (hDC, L"HelloWorld", 10, &rect, ???);
??? = DT_VCENTER(세로로 가운데) | DT_CENTER(가로로 가운데) | DT_SINGLELINE(한 줄로만 출력) | DT_WORDBREAK | DT_EDITCONTROL(줄바꿈)

3. 출력 옵션
SetBkColor(hDC, RGB(255,0,0));        // 백그라운드 컬러
SetTextColor (hDC, RGB (255, 0, 0));  // 텍스트 컬러
```
------------------------------------------------------------------------------
# 캐럿
```C++
* 캐럿 다루기
1. 캐럿 선언
CreateCaret(hWnd, NULL, 5, 15); //--- 캐럿 만들기
ShowCaret(hWnd);
GetTextExtentPoint32(hDC, str[i], count, &size); //--- 문자열 길이 알아내기
TextOut(hDC, 0, i*20, str[i], _tcslen(str[i]));	//--- 출력
SetCaretPos(size.cx, i * 20); //--- 캐럿 위치하기
```
--------------------------------------------------------------------------------
# 도형 그리기
```C++
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

void DrawGrid(HDC hdc) {
	for (double i = 0; i < grid; i++) {
		DrawLine(hdc, 0, i * (WINDOW_Y / grid), WINDOW_X, i * (WINDOW_Y / grid));
	}
	for (double j = 0; j < grid; j++) {
		DrawLine(hdc, j * (WINDOW_X / grid), 0, j * (WINDOW_X / grid), WINDOW_Y);
	}
}

void DrawTriangle(HDC hdc, int x1, int y1, int x2, int y2) {
	POINT points[3] = { {(x1+x2)/2, y1}, {x1, y2}, {x2, y2} };
	Polygon(hdc, points, 3);
}

void DrawRectangle(HDC hdc, int x1, int y1, int x2, int y2) {
	Rectangle(hdc, x1, y1, x2, y2);	// 사각형, RoundRect, PolyLine
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
	Ellipse(hdc, x1, y1, x2, y2);	// 원, Arc, Chord, Pie
}
```
# 실습때 사용
```C++
1. 엔터 눌렀을때
if (_stscanf(str, _T("%d %d %d %d %d %d"),	
	&angle[parsed], &x1[parsed], &y1[parsed], &x2[parsed], &y2[parsed], &nWidth[parsed]) != 6)	// str 분할해서 할당하기
{
	memset(str, 0, sizeof(str));	// str 비우기
}
2. 초기화 할 때 안보이게 하려면 -1로 보내버리기
3. 문자열 한 글자씩 출력
for (int i = 0; i < 26; i++) {
	TCHAR singleChar[2]; // 개별 문자를 저장할 배열
	singleChar[0] = alpa[i].alp; // 개별 문자 할당
	singleChar[1] = '\0'; // 문자열 종료 문자 추가
	TextOut(hDC, alpa[i].x * grid + 10, alpa[i].y * grid + 5, singleChar, 1); // 개별 문자 출력
}
4. 숫자의 알파벳, 알파벳의 숫자
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
```


