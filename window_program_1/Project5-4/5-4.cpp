#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <atlImage.h>
//#include "resource.h"

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
    static RECT rect;

    static CImage img, imgSprite[5];
    static int image_num = 0;
    static int nWidth, nHeight;
    static int spriteWidth = 100, spriteHeight = 100; // 각 스프라이트의 크기

    static int xPos[5], yPos[5];
    static int xPosCopy[5], yPosCopy[5];
    static int xMove = 0, yMove = 0;
    static int rl, ud;

    static int l_mouse_x, l_mouse_y;

    static bool zigzag = false;
    static int zigzagDirection = 1;

    static int goBig = 1;
    static int frameIndex = 0; // 현재 스프라이트 프레임 인덱스
    static int maxFrameIndex = 9; // 첫 줄의 스프라이트 개수
    static int frames = 10; // 스프라이트의 이미지 수
    static int currentRow = 0; // 현재 스프라이트 줄

    static bool changing = false;
    static double scale = 2;

    static bool selected[5];
    static bool l_clicked = false;

    switch (uMsg) {
    case WM_CREATE:
    {
        xMove = 5;
        yMove = 5;
        GetClientRect(hWnd, &rect);
        img.Load(L"image.png");
        nWidth = img.GetWidth();
        nHeight = img.GetHeight();
        imgSprite[0].Load(L"kurby.png");
        image_num++;
        for (int i = 0; i < 5; i++) {
            selected[i] = true;
        }
        SetTimer(hWnd, 1, 50, NULL);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        l_mouse_x = LOWORD(lParam);
        l_mouse_y = HIWORD(lParam);
        for (int i = 0; i < image_num; i++) {
            if (xPos[i] <= l_mouse_x && l_mouse_x <= xPos[i] + spriteWidth &&
                yPos[i] <= l_mouse_y && l_mouse_y <= yPos[i] + spriteHeight) {
                l_clicked = true;
                for (int j = 0; j < image_num; j++) {
                    selected[j] = false;
                }
                selected[i] = true;
                break;  // 스프라이트를 찾으면 더 이상 반복할 필요 없음
            }
        }

        if (l_clicked) {
            currentRow = 1;
            frames = 8;
        }
        else {
            SetTimer(hWnd, 7, 50, NULL);
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        KillTimer(hWnd, 7);
        ud = 0;
        rl = 0;
        SetTimer(hWnd, 1, 50, NULL);
        break;
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
        case '0':
            for (int i = 0; i < image_num; i++) {
                selected[i] = false;
            }
            selected[0] = true;
            break;
        case '1':
            if (image_num <= 0) break;
            for (int i = 0; i < image_num; i++) {
                selected[i] = false;
            }
            selected[1] = true;
            break;
        case '2':
            if (image_num <= 1) break;
            for (int i = 0; i < image_num; i++) {
                selected[i] = false;
            }
            selected[2] = true;
            break;
        case '3':
            if (image_num <= 2) break;
            for (int i = 0; i < image_num; i++) {
                selected[i] = false;
            }
            selected[3] = true;
            break;
        case '4':
            if (image_num <= 3) break;
            for (int i = 0; i < image_num; i++) {
                selected[i] = false;
            }
            selected[4] = true;
            break;
        case '5':
            for (int i = 0; i < 5; i++) {
                selected[i] = true;
            }
            break;
        case 'j':
            KillTimer(hWnd, 1);
            currentRow = 2;
            SetTimer(hWnd, 2, 50, NULL);
            break;
        case 'e':
            KillTimer(hWnd, 1);
            changing = true;
            SetTimer(hWnd, 3, 100, NULL);
            break;
        case 's':
            KillTimer(hWnd, 1);
            changing = true;
            SetTimer(hWnd, 4, 100, NULL);
            break;
        case 't':
            if (image_num >= 5)break;
            imgSprite[image_num].Load(L"kurby.png");
            image_num++;
            break;
        case 'a':
            zigzag = !zigzag;
            if (zigzag) {
                for (int i = 0; i < 5; i++) {
                    xPosCopy[i] = xPos[i];
                    yPosCopy[i] = yPos[i];
                }
                SetTimer(hWnd, 5, 50, NULL);
            }
            else {
                for (int i = 0; i < 5; i++) {
                    xPos[i] = xPosCopy[i];
                    yPos[i] = yPosCopy[i];
                }
                KillTimer(hWnd, 5);
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
        case VK_RIGHT:
            rl = 1;
            break;
        case VK_LEFT:
            rl = -1;
            break;
        case VK_UP:
            ud = -1;
            break;
        case VK_DOWN:
            ud = 1;
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_KEYUP:
        switch (wParam)
        {
        case VK_RIGHT:
            rl = 0;
            break;
        case VK_LEFT:
            rl = 0;
            break;
        case VK_UP:
            ud = 0;
            break;
        case VK_DOWN:
            ud = 0;
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            for (int i = 0; i < image_num; i++) {
                if (selected[i] == false)continue;
                xPos[i] += xMove * rl;
                yPos[i] += yMove * ud;
            }
            frameIndex = (frameIndex + 1) % (maxFrameIndex + 1); // 다음 프레임으로 이동
            InvalidateRect(hWnd, NULL, false);
            break;
        case 2:
            ud--;
            rl = 1;
            if (ud < -5) {
                KillTimer(hWnd, 2);
                ud = 0;
                rl = 0;
                currentRow = 0;
                SetTimer(hWnd, 1, 50, NULL);
            }
            for (int i = 0; i < image_num; i++) {
                if (selected[i] == false)continue;
                xPos[i] += xMove * rl;
                yPos[i] += yMove * ud;
            }
            frameIndex = (frameIndex + 1) % (maxFrameIndex + 1); // 다음 프레임으로 이동
            InvalidateRect(hWnd, NULL, false);
            break;
        case 3:
            if (changing) {
                scale+=0.1;
                if (scale >= 3) {
                    changing = false;
                }
            }
            else {
                scale-=0.1;
                if (scale <= 2) {
                    KillTimer(hWnd, 3);
                    scale = 2;
                    SetTimer(hWnd, 1, 50, NULL);
                }
            }
            frameIndex = (frameIndex + 1) % (maxFrameIndex + 1); // 다음 프레임으로 이동
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case 4:
            if (changing) {
                scale-=0.1;
                if (scale <= 1) {
                    changing = false;
                }
            }
            else {
                scale+=0.1;
                if (scale >= 2) {
                    KillTimer(hWnd, 4);
                    scale = 2;
                    SetTimer(hWnd, 1, 50, NULL);
                }
            }
            frameIndex = (frameIndex + 1) % (maxFrameIndex + 1); // 다음 프레임으로 이동
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case 5:
            if (zigzagDirection == 1) {
                for (int i = 0; i < image_num; i++) {
                    if (selected[i] == false)continue;
                    xPos[i] += xMove * 1;
                }
            }
            else {
                for (int i = 0; i < image_num; i++) {
                    if (selected[i] == false)continue;
                    xPos[i] -= xMove * 1;
                }
            }
            if (xPos[0] > nWidth || xPos[0] < 0) {
                for (int i = 0; i < image_num; i++) {
                    if (selected[i] == false)continue;
                    yPos[i] += 50;
                }
                zigzagDirection *= -1;
            }
            break;
        case 6:
            break;
        case 7:
            if (xPos[0] > l_mouse_x) { rl = -1; }
            if (xPos[0] < l_mouse_x) { rl = 1; }
            if (xPos[0] == l_mouse_x) { rl = 0; }
            if (yPos[0] > l_mouse_y) { ud = -1; }
            if (yPos[0] < l_mouse_y) { ud = 1; }
            if (yPos[0] == l_mouse_y) { ud = 0; }
            xPos[0] += xMove * rl;
            yPos[0] += yMove * ud;
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);
        mDC = CreateCompatibleDC(hDC);
        hBitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
        SelectObject(mDC, (HBITMAP)hBitmap);
        // 배경 그리기
        img.Draw(mDC, 0, 0, rect.right, rect.bottom, 0, 0, nWidth, nHeight);

        // 현재 프레임을 계산하여 그리기
        int spriteX = (frameIndex % frames) * spriteWidth;
        int spriteY = currentRow * spriteHeight;
        int drawWidth = spriteWidth * scale;
        int drawHeight = spriteHeight * scale;
        
        for (int i = 0; i < image_num; i++) {
            if (selected[i] == false)continue;
            if (rl == 1 || rl == 0) {
                imgSprite[i].Draw(mDC, xPos[i], yPos[i], drawWidth, drawHeight, spriteX, spriteY, spriteWidth, spriteHeight);
            }
            else if (rl == -1) {
                imgSprite[i].Draw(mDC, xPos[i], yPos[i], drawWidth, drawHeight, spriteX, spriteY, spriteWidth, spriteHeight);
            }
        }
        
        // 복사 후 제거
        BitBlt(hDC, 0, 0, rect.right, rect.bottom, mDC, 0, 0, SRCCOPY);
        DeleteDC(mDC);
        DeleteObject(hBitmap);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        img.Destroy();
        for (int i = 0; i < image_num; i++) {
            imgSprite[i].Destroy();
        }
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
