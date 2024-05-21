#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <atlImage.h>
#include "resource.h"

#define WINDOW_X 800
#define WINDOW_Y 600
#define GRID 50
#define GRID_X 6
#define GRID_Y 12
#define M_PI 3.14

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 815, 658, NULL, (HMENU)NULL, hInstance, NULL);
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
    HBITMAP hBitmap;

    static CImage img;
    static int nWidth, nHeight;

    static bool Ldrag = false;
    static int l_mouse_x;
    static int l_mouse_y;
    static bool Rdrag = false;
    static int r_mouse_x;
    static int r_mouse_y;
    static int oldX;
    static int oldY;
    static int sizeX;
    static int sizeY;
    static int mouseX;
    static int mouseY;
    static int deltaX;
    static int deltaY;

    static int copy = 0;
    static int drawWidth;
    static int drawHeight;
    static int edge_startX;
    static int edge_startY;
    static int edge_endX;
    static int edge_endY;

    static bool flipH = false;
    static bool flipV = false;

    static int TIMER_ID;

    static bool zigzag = false;
    static int zigzagDirection = 1;

    static bool nul = false;
    static int goBig = 1;

    static DWORD dword = SRCCOPY;

    static bool drawFullImage = false;

    static int alpha = 0;

    switch (uMsg) {
    case WM_CREATE:
    {
        img.Load(L"image.png");
        nWidth = img.GetWidth();
        nHeight = img.GetHeight();
        break;
    }

    case WM_LBUTTONDOWN:
    {
        Ldrag = true;
        l_mouse_x = LOWORD(lParam);
        l_mouse_y = HIWORD(lParam);
        edge_startX = l_mouse_x;
        edge_startY = l_mouse_y;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_LBUTTONUP:
    {
        mouseX = 0;
        mouseY = 0;
        deltaX = 0;
        deltaY = 0;
        Ldrag = false;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        Rdrag = true;
        r_mouse_x = LOWORD(lParam);
        r_mouse_y = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_RBUTTONUP:
    {
        mouseX = 0;
        mouseY = 0;
        deltaX = 0;
        deltaY = 0;
        Rdrag = false;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (Ldrag) {
            mouseX = LOWORD(lParam);
            mouseY = HIWORD(lParam);
            deltaX = mouseX - l_mouse_x;
            deltaY = mouseY - l_mouse_y;
            edge_endX = edge_startX + deltaX;
            edge_endY = edge_startY + deltaY;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (Rdrag) {
            mouseX = LOWORD(lParam);
            mouseY = HIWORD(lParam);
            deltaX = mouseX - r_mouse_x;
            deltaY = mouseY - r_mouse_y;
            edge_endX = edge_endX + deltaX;
            edge_endY = edge_endY + deltaY;
            edge_startX = edge_startX + deltaX;
            edge_startY = edge_startY + deltaY;
            r_mouse_x = mouseX;
            r_mouse_y = mouseY;
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_CHAR:
        switch (wParam)
        {
        case '1':
            img.Destroy();
            img.Load(L"image.png");
            nWidth = img.GetWidth();
            nHeight = img.GetHeight();
            break;
        case '2':
            img.Destroy();
            img.Load(L"bitmap1.bmp");
            nWidth = img.GetWidth();
            nHeight = img.GetHeight();
            break;
        case 'e':
            alpha--;
            break;
        case 's':
            alpha++;
            break;
        case '0':
            alpha = 0;
            break;
        case 'f':
            drawFullImage = !drawFullImage;
            break;
        case 'r':
            edge_startX = 0;
            edge_startY = 0;
            edge_endX = 0;
            edge_endY = 0;
            break;
        case 'm':
            edge_endX += 5;
            edge_endY += 5;
            break;
        case 'n':
            edge_endX -= 5;
            edge_endY -= 5;
            break;
        case 'h':
            flipH = !flipH;
            break;
        case 'v':
            flipV = !flipV;
            break;
        case 'x':
            edge_endX += 5;
            break;
        case 'y':
            edge_endY += 5;
            break;
        case 'a':
            zigzag = !zigzag;
            if (zigzag) {
                SetTimer(hWnd, 1, 100, NULL);
            }
            else {
                KillTimer(hWnd, 1);
            }
            break;
        case 'b':
            nul = !nul;
            if (nul) {
                TIMER_ID = 2;
                SetTimer(hWnd, TIMER_ID, 100, NULL);
            }
            else {
                KillTimer(hWnd, TIMER_ID);
            }
            break;
        case 'i':
            if (dword == SRCCOPY) {
                dword = NOTSRCCOPY;
            }
            else {
                dword = SRCCOPY;
            }
            break;
        case 'k':
            if (copy > 20) {
                copy -= 5;
                copy -= 5;
            }
            else {
                copy += 5;
                copy += 5;
            }
            break;
        case 'q':
            PostQuitMessage(0);
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
            break;
        case VK_RIGHT:
            edge_startX += 10;
            edge_endX += 10;
            break;
        case VK_LEFT:
            edge_startX -= 10;
            edge_endX -= 10;
            break;
        case VK_UP:
            edge_startY -= 10;
            edge_endY -= 10;
            break;
        case VK_DOWN:
            edge_startY += 10;
            edge_endY += 10;
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            if (zigzagDirection == 1) {
                edge_startX += 10;
                edge_endX += 10;
            }
            else {
                edge_startX -= 10;
                edge_endX -= 10;
            }
            if (edge_endX > nWidth || edge_startX < 0) {
                zigzagDirection *= -1;
                edge_startY += 50;
                edge_endY += 50;
            }
            break;
        case 2:
            if (goBig) {
                edge_endX += 5;
                edge_endY += 5;
                if (edge_endX - edge_startX > 200)goBig = 0;
            }
            else {
                edge_endX -= 5;
                edge_endY -= 5;
                if (edge_endX - edge_startX < 50)goBig = 1;
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);
        mDC = CreateCompatibleDC(hDC);
        hBitmap = CreateCompatibleBitmap(hDC, nWidth, nHeight);
        SelectObject(mDC, (HBITMAP)hBitmap);
        img.Draw(mDC, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight);

        RECT rect = { edge_startX, edge_startY, edge_endX, edge_endY };
        drawWidth = edge_endX - edge_startX;
        drawHeight = edge_endY - edge_startY;

        if (drawFullImage) {
            StretchBlt(mDC, 0, 0, nWidth, nHeight,
                 mDC, edge_startX, edge_startY, drawWidth, drawHeight, SRCCOPY);
        }
        else {
            if (flipH && flipV) {
                img.StretchBlt(mDC, drawWidth, drawHeight, -drawWidth, -drawHeight,
                    edge_startX, edge_startY, drawWidth, drawHeight, dword);
            }
            else if (flipH) {
                img.StretchBlt(mDC, drawWidth, 0, -drawWidth, drawHeight,
                    edge_startX, edge_startY, drawWidth, drawHeight, dword);
            }
            else if (flipV) {
                img.StretchBlt(mDC, 0, drawHeight, drawWidth, -drawHeight,
                    edge_startX, edge_startY, drawWidth, drawHeight, dword);
            }
            else {
                img.StretchBlt(mDC, 0, 0, drawWidth + copy, drawHeight + copy,
                    edge_startX, edge_startY, drawWidth + copy, drawHeight + copy, dword);
            }
        }
        StretchBlt(mDC, edge_startX, edge_startY, drawWidth, drawHeight,
            mDC, edge_startX, edge_startY, drawWidth + alpha, drawHeight + alpha, SRCCOPY);

        DrawEdge(mDC, &rect, EDGE_RAISED, BF_RECT);


        BitBlt(hDC, 0, 0, nWidth, nHeight, mDC, 0, 0, SRCCOPY);
        DeleteDC(mDC);
        DeleteObject(hBitmap);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        img.Destroy();
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
