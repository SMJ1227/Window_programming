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
    WndClass.lpszMenuName = MAKEINTRESOURCE(NULL);
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
    static int x;
    static int y;
    static DWORD dword = SRCCOPY;
    static bool shared = false;
    static int sharedX = WINDOW_X / 2;
    static int sharedY = WINDOW_Y / 2;
    static int r_mouse_x;
    static int r_mouse_y;
    static int rect_num;
    static bool clicked = false;
    static int selectedX;
    static int selectedY;
    static int offsetX = 0;
    static bool selectedP = false;
    static int jjig = 0;

    switch (uMsg) {
    case WM_CREATE:
        GetClientRect(hWnd, &rt);
        x = rt.right;
        y = rt.bottom;
        hBitmap = (HBITMAP)LoadImage(NULL, TEXT("bitmap1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (hBitmap == NULL) {
            MessageBox(hWnd, _T("Failed to load bitmap!"), _T("Error"), MB_OK);
        }
        else {
            GetObject(hBitmap, sizeof(BITMAP), &bmp);
        }
        break;
    case WM_RBUTTONDOWN:
        if (!shared)break;
        offsetX = 0;
        r_mouse_x = LOWORD(lParam);
        r_mouse_y = HIWORD(lParam);
        if (r_mouse_x < sharedX && r_mouse_y < sharedY) {
            rect_num = 1;
            clicked = true;
        }
        else if (r_mouse_x > sharedX && r_mouse_y < sharedY) {
            rect_num = 2;
            clicked = true;
        }
        else if (r_mouse_x < sharedX && r_mouse_y > sharedY) {
            rect_num = 3;
            clicked = true;
        }
        else if (r_mouse_x > sharedX && r_mouse_y > sharedY) {
            rect_num = 4;
            clicked = true;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_LBUTTONDOWN:
        clicked = false;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_RIGHT:
            offsetX += 10;
            if (offsetX > sharedX)offsetX = 0;
            break;
        case VK_LEFT:
            offsetX -= 10;
            if (offsetX < -sharedX)offsetX = 0;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_CHAR:
        switch (wParam) {
        case 'a':
            if (x == rt.right) {
                x = bmp.bmWidth;
                y = bmp.bmHeight;
            }
            else {
                x = rt.right;
                y = rt.bottom;
            }
            break;
        case 'r':
            if (dword == DSTINVERT) {
                dword = SRCCOPY;
            }
            else {
                dword = DSTINVERT;
            }
            break;
        case '+':
        case '=':
            if (x == rt.right) {
                x = bmp.bmWidth;
                y = bmp.bmHeight;
            }
            else {
                sharedX++;
                sharedY++;
                jjig += 2;
            }
            shared = true;
            break;
        case '-':
        case '_':
            shared = false;
            break;
        case 'p':
            selectedP = true;
            break;
        case 's':
            x = rt.right;
            y = rt.bottom;
            dword = SRCCOPY;
            jjig = 0;
            clicked = false;
            offsetX = 0;
            selectedP = true;
            break;
        case 'q':
            PostQuitMessage(0);
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
        if (selectedP) {
            // 왼쪽 위
            StretchBlt(hDC, 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            if (offsetX > 0) {
                StretchBlt(hDC, offsetX - sharedX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
            else {
                StretchBlt(hDC, sharedX + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
            // 오른쪽 위
            StretchBlt(hDC, sharedX + 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            if (offsetX > 0) {
                StretchBlt(hDC, 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
            else {
                StretchBlt(hDC, sharedX * 2 + 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
            // 왼쪽 아래
            StretchBlt(hDC, 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            if (offsetX > 0) {
                StretchBlt(hDC, offsetX - sharedX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
            else {
                StretchBlt(hDC, sharedX + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
            // 오른쪽 아래
            StretchBlt(hDC, sharedX + 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            if (offsetX > 0) {
                StretchBlt(hDC, 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
            else {
                StretchBlt(hDC, sharedX * 2 + 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
            }
        }
        if (!selectedP && shared) {
            if (clicked) {
                RECT rect;
                HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                SelectObject(hDC, hPen);
                switch (rect_num)
                {
                case 1:                    // 왼쪽 위
                    Rectangle(hDC, 0, 0, sharedX, sharedY);
                    StretchBlt(hDC, 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    if (offsetX > 0) {
                        StretchBlt(hDC, offsetX - sharedX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    else {
                        StretchBlt(hDC, sharedX + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    break;
                case 2:                    // 오른쪽 위
                    Rectangle(hDC, sharedX, 0, sharedX * 2, sharedY);
                    StretchBlt(hDC, sharedX + 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    if (offsetX > 0) {
                        StretchBlt(hDC, 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    else {
                        StretchBlt(hDC, sharedX * 2 + 1 + offsetX, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    break;
                case 3:                    // 왼쪽 아래
                    Rectangle(hDC, 0, sharedY, sharedX, sharedY * 2);
                    StretchBlt(hDC, 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    if (offsetX > 0) {
                        StretchBlt(hDC, offsetX - sharedX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    else {
                        StretchBlt(hDC, sharedX + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    break;
                case 4:                    // 오른쪽 아래
                    Rectangle(hDC, sharedX, sharedY, sharedX * 2, sharedY * 2);
                    StretchBlt(hDC, sharedX + 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    if (offsetX > 0) {
                        StretchBlt(hDC, 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    else {
                        StretchBlt(hDC, sharedX * 2 + 1 + offsetX, sharedY + 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                    }
                    break;
                }
                DeleteObject(hPen);
            }
            else {
                // 왼쪽 위
                StretchBlt(hDC, 1, 1, sharedX - 2, sharedY - 2, mDC, 0, 0, x, y, dword);
                // 오른쪽 위
                StretchBlt(hDC, sharedX + 1, 1, sharedX - 2 - jjig, sharedY - 2, mDC, 0, 0, x, y, dword);
                // 왼쪽 아래
                StretchBlt(hDC, 1, sharedY + 1, sharedX - 2, sharedY - 2 - jjig, mDC, 0, 0, x, y, dword);
                // 오른쪽 아래
                StretchBlt(hDC, sharedX + 1, sharedY + 1, sharedX - 2 - jjig, sharedY - 2 - jjig, mDC, 0, 0, x, y, dword);
            }
        }
        else if(!selectedP && !shared){
            StretchBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, x, y, dword);
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
