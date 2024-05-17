#include <windows.h>
#include <vector>
#include <time.h>

const int BOARD_WIDTH = 800;
const int BOARD_HEIGHT = 600;
const int GRID = 40;
const int MAP_WIDTH = BOARD_WIDTH / GRID;
const int MAP_HEIGHT = BOARD_HEIGHT / GRID;
const int PLAYER_SIZE = 20;
const int BULLET_SIZE = 6;
const int BULLET_RADIUS = 3;
const double M_PI = 3.141592;

bool g_explosionOccurred = false; // ���� ���θ� ��Ÿ���� ���� ����
int g_explosionX = 0; // ������ �߻��� ��ġ X ��ǥ
int g_explosionY = 0; // ������ �߻��� ��ġ Y ��ǥ

int map[MAP_HEIGHT][MAP_WIDTH];

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"InversusGame";

// WinMain �Լ�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
    srand((unsigned int)time(NULL));
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
    WndClass.lpszMenuName = MAKEINTRESOURCE(NULL);
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_QUESTION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, BOARD_WIDTH, BOARD_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

// ���� ����
struct Player {
    int x, y;
    int bullets;
} g_player;

struct Bullet {
    int x, y;
    int dx, dy;
};
vector<Bullet> g_bullets;

struct Enemy {
    int x, y;
    int dx, dy;
};
vector<Enemy> g_enemies;

void ProcessKeyboardInput(WPARAM wParam);
void InitializeMap(int mapType);
void DrawPlayer(HDC hDC);
void DrawMap(HDC hdc);
void ShootBullet(int dx, int dy);
void MoveBullets();
void DrawBullets(HDC hDC);
void GenerateEnemy();
void MoveEnemies();
void DrawEnemies(HDC hDC);
void CheckCollisions();
void CheckEnemyPlayerCollisions();
void CheckEnemyBulletCollisions();
void CheckExplosionBulletCollisions();
void DrawExplosion(HDC hDC, int x, int y);

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {    
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps); 
    HDC mDC;
    HBITMAP hBitmap;
    RECT rt;

    COLORREF COLOR_WHITE = RGB(255, 255, 255);
    COLORREF COLOR_BLACK = RGB(0, 0, 0);

    static int mapType = 1;
    static int enemy_gen_interval = 60;
    static int explosion_time = 10;
    static int shakeAmountX = 0;
    static int shakeAmountY = 0;
    switch (message) {
    case WM_CREATE:
        InitializeMap(mapType);
        // Ÿ�̸� ����
        g_player.x = BOARD_WIDTH / 2 - PLAYER_SIZE / 2;
        g_player.y = BOARD_HEIGHT / 2 - PLAYER_SIZE / 2;
        g_player.bullets = 6;
        SetTimer(hWnd, 1, 1000 / 60, NULL); // 60 FPS�� ����
        break;
    case WM_CHAR:
        switch (wParam)
        {
        case 'q':
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_TIMER:
        // ���� ���� ������Ʈ
        switch (wParam)
        {
        case 1:
            enemy_gen_interval--;
            MoveBullets();
            CheckCollisions();
            MoveEnemies();
            if (enemy_gen_interval <= 0) {
                GenerateEnemy();
                enemy_gen_interval = 60;
            }
            break;
        }
        // ȭ�� ���� ��û
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_PAINT:
    {
        GetClientRect(hWnd, &rt);
        mDC = CreateCompatibleDC(hDC);
        hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
        SelectObject(mDC, (HBITMAP)hBitmap);

        //--- ��� �׸��⸦ �޸� DC���Ѵ�.
        Rectangle(mDC, 0, 0, rt.right, rt.bottom);  //--- ȭ�鿡 ����ֱ� ������ȭ�鰡�����簢�����׷��������μ����ϱ�
        DrawMap(mDC);
        DrawPlayer(mDC);
        DrawBullets(mDC);
        DrawEnemies(mDC);
        if (g_explosionOccurred) {
            shakeAmountX = rand() % 10 - 10;
            shakeAmountY = rand() % 10 - 10;
            DrawExplosion(mDC, g_explosionX + shakeAmountX, g_explosionY + shakeAmountY);
            explosion_time--;
            if (explosion_time <= 0) {
                g_explosionOccurred = false;
                g_explosionX = 0;
                g_explosionY = 0;
                explosion_time = 10;
                shakeAmountX = 0;
                shakeAmountY = 0;
            }
        }

        // �޸� DC���� ȭ�� DC�� �׸��� ����
        if (shakeAmountX == 0 && shakeAmountY == 0) {
            BitBlt(hDC, 0, 0, BOARD_WIDTH, BOARD_HEIGHT, mDC, 0, 0, SRCCOPY);
        }
        else {
            BitBlt(hDC, shakeAmountX, shakeAmountY, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
        }
        
        DeleteDC(mDC);
        DeleteObject(hBitmap);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_KEYDOWN:
        ProcessKeyboardInput(wParam);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void ProcessKeyboardInput(WPARAM wParam) {
    switch (wParam) {
    case VK_UP:
        ShootBullet(0, -10); // ���� �������� �Ѿ� �߻�
        break;
    case VK_DOWN:
        ShootBullet(0, 10); // �Ʒ��� �������� �Ѿ� �߻�
        break;
    case VK_LEFT:
        ShootBullet(-10, 0); // ���� �������� �Ѿ� �߻�
        break;
    case VK_RIGHT:
        ShootBullet(10, 0); // ������ �������� �Ѿ� �߻�
        break;

    case 'W':
    case 'w':
        g_player.y -= 5;
        break;
    case 'S':
    case 's':
        g_player.y += 5;
        break;
    case 'A':
    case 'a':
        g_player.x -= 5;
        break;
    case 'D':
    case 'd':
        g_player.x += 5;
        break;
    case 'R':
    case 'r':
        g_player.bullets = 6;
        break;
    }
}

// ���� �ʱ�ȭ�ϴ� �Լ�
void InitializeMap(int mapType) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = 0;
        }
    }
    int centerX = MAP_WIDTH / 2 - 1;
    int centerY = MAP_HEIGHT / 2;

    switch (mapType)
    {
    case 0: //�׸��
        for (int y = centerY - 4; y <= centerY + 4; ++y) {
            for (int x = centerX - 4; x <= centerX + 4; ++x) {
                if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                    map[y][x] = 1;
                }
            }
        }
        break;
    case 1:
        for (int y = centerY - 4; y <= centerY + 4; ++y) {
            for (int x = centerX - 4; x <= centerX + 4; ++x) {
                if (abs(x - centerX) + abs(y - centerY) <= 4) {
                    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                        map[y][x] = 1;
                    }
                }
            }
        }
        break;
    }
}

void DrawMap(HDC hdc) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // ���� ���� ���� 1�̸� ���(�÷��̾� ����), 0�̸� ���������� �׸��ϴ�.
            if (map[y][x] == 1) {
                // ��� �簢�� �׸���
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
            else {
                // ������ �簢�� �׸���
                HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
        }
    }
}


void DrawPlayer(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hBrush);
    Rectangle(hdc, g_player.x - PLAYER_SIZE / 2, g_player.y - PLAYER_SIZE / 2, g_player.x + PLAYER_SIZE / 2, g_player.y + PLAYER_SIZE / 2);
    double angle = 2 * M_PI / g_player.bullets;
    for (int i = 0; i < g_player.bullets; i++) {
        int bulletX = g_player.x + PLAYER_SIZE / 3 * cos(i * angle);
        int bulletY = g_player.y + PLAYER_SIZE / 3 * sin(i * angle);

        Ellipse(hdc, bulletX - BULLET_RADIUS, bulletY - BULLET_RADIUS, bulletX + BULLET_RADIUS, bulletY + BULLET_RADIUS);
    }
    DeleteObject(hBrush);
}

void ShootBullet(int dx, int dy) {
    if (g_player.bullets > 0) {
        Bullet newBullet;
        newBullet.x = g_player.x;
        newBullet.y = g_player.y;
        newBullet.dx = dx;
        newBullet.dy = dy;
        g_bullets.push_back(newBullet);
        g_player.bullets--;
    }
}

void MoveBullets() {
    for (auto it = g_bullets.begin(); it != g_bullets.end();) {
        it->x += it->dx;
        it->y += it->dy;

        if (it->x < 0 || it->x > BOARD_WIDTH || it->y < 0 || it->y > BOARD_HEIGHT) {
            it = g_bullets.erase(it);
        }
        else {
            ++it;
        }
    }
}

void DrawBullets(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hBrush);
    for (const auto& bullet : g_bullets) {
        if (bullet.x >= 0 && bullet.x <= BOARD_WIDTH && bullet.y >= 0 && bullet.y <= BOARD_HEIGHT) {
            Ellipse(hdc, bullet.x - BULLET_SIZE / 2, bullet.y - BULLET_SIZE / 2, bullet.x + BULLET_SIZE / 2, bullet.y + BULLET_SIZE / 2);
        }
    }
    DeleteObject(hBrush);
}

void GenerateEnemy() {
    Enemy newEnemy;
    newEnemy.x = rand() % BOARD_WIDTH;
    newEnemy.y = rand() % BOARD_HEIGHT;
    newEnemy.dx = rand() % 2 - 1;
    newEnemy.dy = rand() % 2 - 1;
    g_enemies.push_back(newEnemy);
}

void DrawEnemies(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
    for (const auto& enemy : g_enemies) {
        RECT rect = { enemy.x - PLAYER_SIZE / 2, enemy.y - PLAYER_SIZE / 2, enemy.x + PLAYER_SIZE / 2, enemy.y + PLAYER_SIZE / 2 };
        FillRect(hdc, &rect, hBrush);
    }
    DeleteObject(hBrush);
}

void MoveEnemies() {
    for (auto it = g_enemies.begin(); it != g_enemies.end();) {
        it->x += it->dx;
        it->y += it->dy;

        if (it->x < 0 || it->x > BOARD_WIDTH || it->y < 0 || it->y > BOARD_HEIGHT) {
            it = g_enemies.erase(it);
        }
        else {
            ++it;
        }
    }
}

void DrawExplosion(HDC hdc, int x, int y) {
    // ���� ȿ���� ũ��� ������ �����մϴ�.
    int explosionSize = 50; // ���� ȿ���� ũ��
    COLORREF explosionColor = RGB(255, 165, 0); // �������� ���� ȿ��

    // ���� �ܰ����� ǥ���ϱ� ���� ���� �����մϴ�.
    HPEN hPen = CreatePen(PS_SOLID, 1, explosionColor);
    SelectObject(hdc, hPen);

    // ���� ���θ� ä��� ���� �귯�ø� �����մϴ�.
    HBRUSH hBrush = CreateSolidBrush(explosionColor);
    SelectObject(hdc, hBrush);

    // ���� �׸��ϴ�.
    Ellipse(hdc, x - explosionSize, y - explosionSize, x + explosionSize, y + explosionSize);

    // ������ ��� �귯�ø� �����մϴ�.
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

// �浹 Ȯ�� �Լ�
void CheckCollisions() {
    CheckEnemyPlayerCollisions();
    CheckEnemyBulletCollisions();
    CheckExplosionBulletCollisions();
}

// ���� �÷��̾� �浹 �˻� �Լ�
void CheckEnemyPlayerCollisions() {
    for (auto& enemy : g_enemies) {
        if (enemy.x >= g_player.x - PLAYER_SIZE / 2 && enemy.x <= g_player.x + PLAYER_SIZE / 2 &&
            enemy.y >= g_player.y - PLAYER_SIZE / 2 && enemy.y <= g_player.y + PLAYER_SIZE / 2) {
            // �浹 �� �÷��̾ �����ϰ� ���� ���� ó�� ���� ����
            // ���⼭�� �ϴ� �� ����
            enemy.x = -1; // ȭ�� ������ �̵��Ͽ� ��������� ����
            enemy.y = -1;
        }
    }
}

// ���� �Ѿ� �浹 �˻� �Լ�
void CheckEnemyBulletCollisions() {
    for (auto it = g_enemies.begin(); it != g_enemies.end(); ) {
        bool collisionDetected = false;
        for (auto& bullet : g_bullets) {
            if (bullet.x >= it->x - PLAYER_SIZE / 2 && bullet.x <= it->x + PLAYER_SIZE / 2 &&
                bullet.y >= it->y - PLAYER_SIZE / 2 && bullet.y <= it->y + PLAYER_SIZE / 2) {
                // �浹 �� ���� �����
                g_explosionX = it->x;
                g_explosionY = it->y;
                g_explosionOccurred = true;
                it = g_enemies.erase(it);
                collisionDetected = true;
                break;
            }
        }
        if (!collisionDetected) {
            ++it;
        }
    }
}

void CheckExplosionBulletCollisions() {
    for (auto it = g_bullets.begin(); it != g_bullets.end(); ) {
        if ((it->x - g_explosionX) * (it->x - g_explosionX) + (it->y - g_explosionY) * (it->y - g_explosionY) <= 50 * 50) {
            it = g_bullets.erase(it);
        }
        else {
            ++it;
        }
    }
}

