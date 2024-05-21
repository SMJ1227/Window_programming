#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "resource.h"

#define WINDOW_X 800
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
    srand((unsigned int)time(NULL));
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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 830, 658, NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HDC hDC, mDC;
    PAINTSTRUCT ps;
    static RECT rt;
    static HBITMAP hBitmap;
    static BITMAP bmp;
    static DWORD dword = SRCCOPY;
    static bool clicked = false;
    static int offsetX = 0;
    static int offsetY = 0;
    static int deltaX[9] = { 0 };
    static int deltaY[9] = { 0 };
    static int rect_num = -1;
    static bool game_started = false;
    static bool shared = false;
    static int shared_num = 1;
    static int sharedX = WINDOW_X / shared_num;
    static int sharedY = WINDOW_Y / shared_num;
    static int l_mouse_x;
    static int l_mouse_y;

    static int x;
    static int y;
    static int selectedX;
    static int selectedY;
    static bool selectedP = false;
    static int jjig = 0;

    switch (uMsg) {
    case WM_CREATE:
        GetClientRect(hWnd, &rt);
        hBitmap = (HBITMAP)LoadImage(NULL, TEXT("bitmap1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (hBitmap == NULL) {
            MessageBox(hWnd, _T("Failed to load bitmap!"), _T("Error"), MB_OK);
        }
        else {
            GetObject(hBitmap, sizeof(BITMAP), &bmp);
        }
        x = bmp.bmWidth;
        y = bmp.bmHeight;
        break;
    case WM_LBUTTONDOWN:
        clicked = true;
        l_mouse_x = LOWORD(lParam);
        l_mouse_y = HIWORD(lParam);
        if (game_started) {
            if (l_mouse_y > 0 && l_mouse_y < 200) {
                if (l_mouse_x > 0 && l_mouse_x < 280) {
                    rect_num = 0;
                }
                else if (l_mouse_x > 280 && l_mouse_x < 560) {
                    rect_num = 1;
                }
                else if (l_mouse_x > 560 && l_mouse_x < 840) {
                    rect_num = 2;
                }
            }
            else if (l_mouse_y > 200 && l_mouse_y < 400) {
                if (l_mouse_x > 0 && l_mouse_x < 280) {
                    rect_num = 3;
                }
                else if (l_mouse_x > 280 && l_mouse_x < 560) {
                    rect_num = 4;
                }
                else if (l_mouse_x > 560 && l_mouse_x < 840) {
                    rect_num = 5;
                }
            }
            else if (l_mouse_y > 400 && l_mouse_y < 600) {
                if (l_mouse_x > 0 && l_mouse_x < 280) {
                    rect_num = 6;
                }
                else if (l_mouse_x > 280 && l_mouse_x < 560) {
                    rect_num = 7;
                }
                else if (l_mouse_x > 560 && l_mouse_x < 840) {
                    rect_num = 8;
                }
            }
        }
        else if (shared) {
            if (l_mouse_x > 0 && l_mouse_x < 280) {
                rect_num = 0;
            }
            else if (l_mouse_x > 280 && l_mouse_x < 560) {
                rect_num = 1;
            }
            else if (l_mouse_x > 560 && l_mouse_x < 840) {
                rect_num = 2;
            }
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_LBUTTONUP:
        clicked = false;
        offsetX = 0;
        offsetY = 0;
        deltaX[rect_num] = 0;
        deltaY[rect_num] = 0;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_MOUSEMOVE:
        if (clicked && rect_num >= 0)
        {
            offsetX = LOWORD(lParam);
            offsetY = HIWORD(lParam);
            deltaX[rect_num] = offsetX - l_mouse_x;
            deltaY[rect_num] = offsetY - l_mouse_y;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_PICTURE_1:
            break;
        case ID_PICTURE_2:
            break;
        case ID_CUT_3:
            shared_num = 3;
            break;
        case ID_CUT_4:
            shared_num = 4;
            break;
        case ID_CUT_5:
            shared_num = 5;
            break;
        case ID_GAME_START:
            game_started = true;
            break;
        case ID_GAME_VIEW:
            game_started = false;
            break;
        case ID_GAME_DSTINVERT:
            dword = DSTINVERT;
            break;
        case ID_GAME_QUIT:
            PostQuitMessage(0);
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_CHAR:
        switch (wParam) {
        case 's':
            game_started = true;
            break;
        case 'f':
            break;
        case 'q':
            PostQuitMessage(0);
            break;
        case 'v':
            break;
        case 'h':
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_TIMER:
        switch (wParam) {
        case TIMER_ID:
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);
        mDC = CreateCompatibleDC(hDC);
        SelectObject(mDC, hBitmap);
        StretchBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, x, y, dword);
        if (game_started) {
            int rectWidth = rt.right / 3;
            int rectHeight = rt.bottom / 3;

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    int srcX = (bmp.bmWidth / 3) * j;
                    int srcY = (bmp.bmHeight / 3) * i;
                    int destX = rectWidth * j;
                    int destY = rectHeight * i;
                    int rectIndex = i * 3 + j;
                    StretchBlt(hDC, destX + deltaX[rectIndex], destY + deltaY[rectIndex], rectWidth, rectHeight, mDC, srcX, srcY, bmp.bmWidth / 3, bmp.bmHeight / 3, dword);
                }
            }
        }
        if (shared) {
            int rectWidth = rt.right / 3;
            int rectHeight = rt.bottom / 3;
            for (int j = 0; j < 3; ++j) {
                int srcX = (bmp.bmWidth / 3) * j;
                int destX = rectWidth * j;
                int rectIndex = j;
                StretchBlt(hDC, destX + deltaX[rectIndex], rt.bottom, rectWidth, rectHeight, mDC, srcX, bmp.bmHeight - (bmp.bmHeight / 3), bmp.bmWidth / 3, bmp.bmHeight / 3, dword);
            }
        }
        

        DeleteDC(mDC);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        DeleteObject(hBitmap);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
