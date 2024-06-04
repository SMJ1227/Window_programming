#define _CRT_SECURE_NO_WARNINGS

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
#define WM_UPDATE_DRAW_MODE (WM_USER + 1)
#define WM_UPDATE_LINE_COLOR (WM_USER + 2)
#define WM_UPDATE_DRAW_CRICLE (WM_USER + 3)
#define WM_UPDATE_GO_TIMER (WM_USER + 4)
#define WM_UPDATE_X_Y (WM_USER + 5)
int drawMode = 0; // 0: Sin, 1: ZigZag, 2: Spring, 3: Pie
COLORREF lineColor = RGB(0, 255, 255); // �⺻ û�ϻ�
bool drawCircle = false;
bool goTimer = false;
int xy = 0;

LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlalog_Proc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;

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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, WINDOW_X + 15, WINDOW_Y + 58, NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

BOOL CALLBACK Dlalog_Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        CheckRadioButton(hDlg, IDC_SIN, IDC_PIE, NULL);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_SIN:
        case IDC_ZIGZAG:
        case IDC_SPRING:
        case IDC_PIE:
            if (IsDlgButtonChecked(hDlg, IDC_SIN)) drawMode = 0;
            else if (IsDlgButtonChecked(hDlg, IDC_ZIGZAG)) drawMode = 1;
            else if (IsDlgButtonChecked(hDlg, IDC_SPRING)) drawMode = 2;
            else if (IsDlgButtonChecked(hDlg, IDC_PIE)) drawMode = 3;
            // ���� �����쿡 �޽��� ������
            SendMessage(GetParent(hDlg), WM_UPDATE_DRAW_MODE, drawMode, 0);
            InvalidateRect(hDlg, NULL, TRUE);
            break;

        case IDC_CYAN:
        case IDC_MAGENTA:
        case IDC_YELLOW:
        case IDC_INVERSE:
        {
            int r = 0, g = 0, b = 0;
            if (IsDlgButtonChecked(hDlg, IDC_CYAN)) { r += 0; g += 255; b += 255; }
            if (IsDlgButtonChecked(hDlg, IDC_MAGENTA)) { r += 255; g += 0; b += 255; }
            if (IsDlgButtonChecked(hDlg, IDC_YELLOW)) { r += 255; g += 255; b += 0; }
            if (IsDlgButtonChecked(hDlg, IDC_INVERSE)) { r = 255 - r; g = 255 - g; b = 255 - b; }
            lineColor = RGB(r % 256, g % 256, b % 256);
            // ���� �����쿡 �޽��� ������
            SendMessage(GetParent(hDlg), WM_UPDATE_LINE_COLOR, 0, lineColor);
            InvalidateRect(hDlg, NULL, TRUE);
        }
        break;

        case IDC_MOVE_X:
            goTimer = true;
            xy = 0;
            SendMessage(GetParent(hDlg), WM_UPDATE_GO_TIMER, goTimer, true);
            SendMessage(GetParent(hDlg), WM_UPDATE_X_Y, xy, 0);
            InvalidateRect(hDlg, NULL, TRUE);
            break;

        case IDC_MOVE_Y:
            goTimer = true;
            xy = 1;
            SendMessage(GetParent(hDlg), WM_UPDATE_GO_TIMER, goTimer, true);
            SendMessage(GetParent(hDlg), WM_UPDATE_X_Y, xy, 1);
            InvalidateRect(hDlg, NULL, TRUE);
            break;

        case IDC_STOP:
            goTimer = false;
            SendMessage(GetParent(hDlg), WM_UPDATE_GO_TIMER, goTimer, false);
            InvalidateRect(hDlg, NULL, TRUE);
            break;

        case IDC_RESET:
            drawMode = 0; // �⺻ ���� �ʱ�ȭ
            SendMessage(GetParent(hDlg), WM_UPDATE_DRAW_MODE, drawMode, 0);
            lineColor = RGB(0, 255, 255); // �⺻ �������� �ʱ�ȭ
            SendMessage(GetParent(hDlg), WM_UPDATE_LINE_COLOR, 0, lineColor);
            InvalidateRect(hDlg, NULL, TRUE);
            break;

        case IDC_MOVE_CIRCLE:
            drawCircle = true;
            SendMessage(GetParent(hDlg), WM_UPDATE_DRAW_CRICLE, drawCircle, true);
            InvalidateRect(hDlg, NULL, TRUE);
            break;

        case IDOK:
            EndDialog(hDlg, IDOK);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;
    }
    return FALSE;
}

// ������
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HDC hDC, mDC;
    PAINTSTRUCT ps;
    HBITMAP hBitmap;
    RECT rt;

    static int offsetX = 0;
    static int offsetY = 0;

    //--- �޼��� ó���ϱ�
    switch (uMsg) {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)Dlalog_Proc);
            break;
        case VK_RIGHT:
            break;
        case VK_LEFT:
            break;
        case VK_DOWN:
            break;
        }
    case WM_TIMER:
        switch (wParam)
        {
        case TIMER_ID:
            switch (xy)
            {
            case 0:
                offsetX++;
                break;
            case 1:
                offsetY++;
                break;
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_UPDATE_DRAW_MODE:
        drawMode = (int)wParam;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_UPDATE_LINE_COLOR:
        lineColor = (COLORREF)lParam;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_UPDATE_DRAW_CRICLE:
        drawCircle = (bool)wParam;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_UPDATE_GO_TIMER:
        goTimer = (bool)wParam;
        if (goTimer) { SetTimer(hWnd, TIMER_ID, 1000 / 60, NULL); }
        else if (!goTimer) { KillTimer(hWnd, TIMER_ID); }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_UPDATE_X_Y:
        xy = (int)wParam;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_PAINT:
    {
        GetClientRect(hWnd, &rt);
        int width = rt.right - rt.left;
        int height = rt.bottom - rt.top;
        hDC = BeginPaint(hWnd, &ps);
        mDC = CreateCompatibleDC(hDC);
        hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
        SelectObject(mDC, (HBITMAP)hBitmap);

        // ���� ����
        HPEN hPen = CreatePen(PS_SOLID, 2, lineColor);
        SelectObject(mDC, hPen);

        //--- ��� �׸��⸦ �޸� DC���Ѵ�.
        switch (drawMode) {
        case 0: // ���� �
            MoveToEx(mDC, 0, height / 2, NULL);
            for (int x = 0; x < width; x++) {
                int y = (int)(height / 2 + 100 * sin(2 * 3.14 * (x + offsetX) / 100)) + offsetY;
                LineTo(mDC, x, y);
            }
            break;

        case 1: // �������
            MoveToEx(mDC, 0, height / 2, NULL);
            for (int x = 0; x < width; x += 20) {
                LineTo(mDC, x + offsetX, height / 2 - 50 - offsetY);
                x += 20;
                LineTo(mDC, x + offsetX, height / 2 + 50 + offsetY);
            }
            break;

        case 2: // ������
            for (int x = 0; x < width; x += 50) {
                Ellipse(mDC, x + offsetX, height / 2 - 25, x + 50 + offsetX, height / 2 + 25);
                // rcos+center rsin+center
            }
            break;

        case 3: // ���� �
            MoveToEx(mDC, 0, height / 2, NULL);
            int yDirection = 1; // 1�̸� -1��, -1�̸� 1�� �����ϴ� ����
            for (int x = 0; x < width; x += 20) {
                LineTo(mDC, x + offsetX, height / 2 + 50 * yDirection);
                LineTo(mDC, x + 20 + offsetX, height / 2 + 50 * yDirection);
                yDirection = -yDirection; // y ���� ����
            }
            break;

        }
        if (drawCircle) {
            int radius = 20; // ���� ������
            int centerX = width / 2; // ȭ�� ���� �߾�
            int centerY = height / 2; // ȭ�� ���� �߾�

            // switch ���� �� case�� �°� centerY�� �����մϴ�.
            switch (drawMode) {
            case 0: // ���� �
                centerY += (int)(100 * sin(2 * M_PI * (centerX + offsetX) / 100));
                break;
            case 1: // �������
                if ((offsetX / 20) % 2 == 0) // ¦�� ��° �̵� ��
                    centerY += 50;
                else // Ȧ�� ��° �̵� ��
                    centerY -= 50;
                break;
            case 2: // ������
                // ���� ���� y ��ǥ�� ��ȭ ����
                break;
            case 3: // ���� �
                centerY++; // y ��ǥ�� �������� �̵�
                break;  
            }

            HBRUSH hBrush = CreateSolidBrush(WHITE_BRUSH); // ���� ������ ���� ����� �����ϰ� ����
            SelectObject(mDC, hBrush);
            Ellipse(mDC, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
            DeleteObject(hBrush);
        }


        //--- �������� �޸� DC�� ������ȭ��DC�κ����Ѵ�.
        BitBlt(hDC, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
        DeleteDC(mDC);
        DeleteObject(hBitmap);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);        //---���� �� �޽��� ���� ������ �޼����� OS��
}
