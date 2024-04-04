#include <windows.h>
#include <cmath>

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainWindow";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(L"MainWindow", L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

#define M_PI 3.14

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static COLORREF triangleColor = RGB(255, 0, 0); // Red
    static COLORREF hourglassColor = RGB(0, 255, 0); // Green
    static COLORREF pentagonColor = RGB(0, 0, 255); // Blue
    static COLORREF pieColor = RGB(255, 255, 0); // Yellow
    static COLORREF randomColor = RGB(rand() % 255, rand() % 255, rand() % 255);

    static int selectedShape = -1;
    static int endShape = -1;
    static int default_shape = 3;
    static int is_end = 0;

    switch (uMsg) {
    case WM_CREATE:
        break;
    case WM_KEYUP:
        is_end += 1;
        switch (wParam) {
        case 'T':
        case 't':
            endShape = 0; // 삼각
            break;
        case 'S':
        case 's':
            endShape = 1; // 모래시계
            break;
        case 'P':
        case 'p':
            endShape = 2; // 오각
            break;
        case 'E':
        case 'e':
            endShape = 3; // 파이
            break;
        case 'Q':
        case 'q':
            PostQuitMessage(0);
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_KEYDOWN:
        is_end = 0;
        switch (wParam) {
        case 'T':
        case 't':
            selectedShape = 0; // 삼각
            break;
        case 'S':
        case 's':
            selectedShape = 1; // 모래시계
            break;
        case 'P':
        case 'p':
            selectedShape = 2; // 오각
            break;
        case 'E':
        case 'e':
            selectedShape = 3; // 파이
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));

        int centerX = (rect.right - rect.left) / 2;
        int centerY = (rect.bottom - rect.top) / 2;

        int squareSize = 200;
        RECT centralRect = { centerX - squareSize / 2, centerY - squareSize / 2, centerX + squareSize / 2, centerY + squareSize / 2 };
        HBRUSH hCentralBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &centralRect, hBrush);
        FrameRect(hdc, &centralRect, hCentralBrush);

        HBRUSH triangle_brush = CreateSolidBrush(triangleColor);
        SelectObject(hdc, triangle_brush);
        POINT triangle[3];
        triangle[0].x = centerX - 250;
        triangle[0].y = centerY - 100;
        triangle[1].x = centerX - 336;
        triangle[1].y = centerY + 50;
        triangle[2].x = centerX - 164;
        triangle[2].y = centerY + 50;
        Polygon(hdc, triangle, 3);

        HBRUSH hourglass_brush = CreateSolidBrush(hourglassColor);
        SelectObject(hdc, hourglass_brush);
        POINT hourglass[3];
        hourglass[0].x = centerX - 50;
        hourglass[0].y = centerY + 250;
        hourglass[1].x = centerX + 50;
        hourglass[1].y = centerY + 250;
        hourglass[2].x = centerX;
        hourglass[2].y = centerY + 200;
        Polygon(hdc, hourglass, 3);
        hourglass[0].x = centerX - 50;
        hourglass[0].y = centerY + 150;
        hourglass[1].x = centerX + 50;
        hourglass[1].y = centerY + 150;
        hourglass[2].x = centerX;
        hourglass[2].y = centerY + 200;
        Polygon(hdc, hourglass, 3);

        HBRUSH pentagon_brush = CreateSolidBrush(pentagonColor);
        SelectObject(hdc, pentagon_brush);
        POINT pentagon[5];
        for (int i = 0; i < 5; ++i) {
            pentagon[i].x = centerX + 80 * cos(2 * M_PI / 5 * i) + 250;
            pentagon[i].y = centerY - 80 * sin(2 * M_PI / 5 * i);
        }
        Polygon(hdc, pentagon, 5);

        HBRUSH pie_brush = CreateSolidBrush(pieColor);
        SelectObject(hdc, pie_brush);
        Pie(hdc, centerX - 50, centerY - 250, centerX + 50, centerY - 150, centerX, centerY - 250, centerX, centerY - 200);

        HBRUSH random_brush = CreateSolidBrush(randomColor);

        switch (selectedShape) {
        case 0: // 3
            randomColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            SelectObject(hdc, random_brush);
            triangle[0].x = centerX;
            triangle[0].y = centerY - 100;
            triangle[1].x = centerX - 100;
            triangle[1].y = centerY + 100;
            triangle[2].x = centerX + 100;
            triangle[2].y = centerY + 100;
            Polygon(hdc, triangle, 3);

            triangle[0].x = centerX - 250;
            triangle[0].y = centerY - 100;
            triangle[1].x = centerX - 336;
            triangle[1].y = centerY + 50;
            triangle[2].x = centerX - 164;
            triangle[2].y = centerY + 50;
            Polygon(hdc, triangle, 3);
            break;
        case 1: // ㅁ
            randomColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            SelectObject(hdc, random_brush);
            hourglass[0].x = centerX - 100;
            hourglass[0].y = centerY + 100;
            hourglass[1].x = centerX + 100;
            hourglass[1].y = centerY + 100;
            hourglass[2].x = centerX;
            hourglass[2].y = centerY;
            Polygon(hdc, hourglass, 3);
            hourglass[0].x = centerX - 100;
            hourglass[0].y = centerY - 100;
            hourglass[1].x = centerX + 100;
            hourglass[1].y = centerY - 100;
            hourglass[2].x = centerX;
            hourglass[2].y = centerY;
            Polygon(hdc, hourglass, 3);

            hourglass[0].x = centerX - 50;
            hourglass[0].y = centerY + 250;
            hourglass[1].x = centerX + 50;
            hourglass[1].y = centerY + 250;
            hourglass[2].x = centerX;
            hourglass[2].y = centerY + 200;
            Polygon(hdc, hourglass, 3);
            hourglass[0].x = centerX - 50;
            hourglass[0].y = centerY + 150;
            hourglass[1].x = centerX + 50;
            hourglass[1].y = centerY + 150;
            hourglass[2].x = centerX;
            hourglass[2].y = centerY + 200;
            Polygon(hdc, hourglass, 3);
            break;
        case 2: // 5
            randomColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            SelectObject(hdc, random_brush);
            for (int i = 0; i < 5; ++i) {
                pentagon[i].x = centerX + 110 * cos(2 * M_PI / 5 * i) - 10;
                pentagon[i].y = centerY - 105 * sin(2 * M_PI / 5 * i);
            }
            Polygon(hdc, pentagon, 5);
            for (int i = 0; i < 5; ++i) {
                pentagon[i].x = centerX + 80 * cos(2 * M_PI / 5 * i) + 250;
                pentagon[i].y = centerY - 80 * sin(2 * M_PI / 5 * i);
            }
            Polygon(hdc, pentagon, 5);
            break;
        case 3: // ㅠ
            randomColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            SelectObject(hdc, random_brush);
            Pie(hdc, centerX - 100, centerY - 100, centerX + 100, centerY + 100, centerX, centerY - 100, centerX, centerY);
            Pie(hdc, centerX - 50, centerY - 250, centerX + 50, centerY - 150, centerX, centerY - 250, centerX, centerY - 200);
            break;
        }
        switch (endShape) {
        case 0: // 3
            SelectObject(hdc, triangle_brush);
            triangle[0].x = centerX - 250;
            triangle[0].y = centerY - 100;
            triangle[1].x = centerX - 336;
            triangle[1].y = centerY + 50;
            triangle[2].x = centerX - 164;
            triangle[2].y = centerY + 50;
            Polygon(hdc, triangle, 3);
            break;
        case 1: // ㅁ
            SelectObject(hdc, hourglass_brush);
            hourglass[0].x = centerX - 50;
            hourglass[0].y = centerY + 250;
            hourglass[1].x = centerX + 50;
            hourglass[1].y = centerY + 250;
            hourglass[2].x = centerX;
            hourglass[2].y = centerY + 200;
            Polygon(hdc, hourglass, 3);
            hourglass[0].x = centerX - 50;
            hourglass[0].y = centerY + 150;
            hourglass[1].x = centerX + 50;
            hourglass[1].y = centerY + 150;
            hourglass[2].x = centerX;
            hourglass[2].y = centerY + 200;
            Polygon(hdc, hourglass, 3);
            break;
        case 2: // 5
            SelectObject(hdc, pentagon_brush);
            for (int i = 0; i < 5; ++i) {
                pentagon[i].x = centerX + 110 * cos(2 * M_PI / 5 * i) - 10;
                pentagon[i].y = centerY - 105 * sin(2 * M_PI / 5 * i);
            }
            Polygon(hdc, pentagon, 5);
            for (int i = 0; i < 5; ++i) {
                pentagon[i].x = centerX + 80 * cos(2 * M_PI / 5 * i) + 250;
                pentagon[i].y = centerY - 80 * sin(2 * M_PI / 5 * i);
            }
            Polygon(hdc, pentagon, 5);
            break;
        case 3: // ㅠ
            SelectObject(hdc, pie_brush);
            Pie(hdc, centerX - 50, centerY - 250, centerX + 50, centerY - 150, centerX, centerY - 250, centerX, centerY - 200);
            break;
        }
        endShape = -1;
        if (is_end != 0) {
            switch (default_shape)
            {
            case 0:
                FillRect(hdc, &centralRect, hBrush);
                FrameRect(hdc, &centralRect, hCentralBrush);
                SelectObject(hdc, triangle_brush);
                triangle[0].x = centerX;
                triangle[0].y = centerY - 100;
                triangle[1].x = centerX - 100;
                triangle[1].y = centerY + 100;
                triangle[2].x = centerX + 100;
                triangle[2].y = centerY + 100;
                Polygon(hdc, triangle, 3);
                break;
            case 1:
                FillRect(hdc, &centralRect, hBrush);
                FrameRect(hdc, &centralRect, hCentralBrush);
                SelectObject(hdc, triangle_brush);
                triangle[0].x = centerX;
                triangle[0].y = centerY - 100;
                triangle[1].x = centerX - 100;
                triangle[1].y = centerY + 100;
                triangle[2].x = centerX + 100;
                triangle[2].y = centerY + 100;
                Polygon(hdc, triangle, 3);
                break;
            case 2:
                FillRect(hdc, &centralRect, hBrush);
                FrameRect(hdc, &centralRect, hCentralBrush);
                SelectObject(hdc, hourglass_brush);
                hourglass[0].x = centerX - 100;
                hourglass[0].y = centerY + 100;
                hourglass[1].x = centerX + 100;
                hourglass[1].y = centerY + 100;
                hourglass[2].x = centerX;
                hourglass[2].y = centerY;
                Polygon(hdc, hourglass, 3);
                hourglass[0].x = centerX - 100;
                hourglass[0].y = centerY - 100;
                hourglass[1].x = centerX + 100;
                hourglass[1].y = centerY - 100;
                hourglass[2].x = centerX;
                hourglass[2].y = centerY;
                Polygon(hdc, hourglass, 3);
                break;
            case 3:
                FillRect(hdc, &centralRect, hBrush);
                FrameRect(hdc, &centralRect, hCentralBrush);
                SelectObject(hdc, pie_brush);
                Pie(hdc, centerX - 100, centerY - 100, centerX + 100, centerY + 100, centerX, centerY - 100, centerX, centerY);
                break;
            }
        }
        DeleteObject(hBrush);
        DeleteObject(hCentralBrush);
        DeleteObject(random_brush);
        DeleteObject(triangle_brush);
        DeleteObject(hourglass_brush);
        DeleteObject(pentagon_brush);
        DeleteObject(pie_brush);
        EndPaint(hWnd, &ps);

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}