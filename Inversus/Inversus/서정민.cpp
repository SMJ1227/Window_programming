#include <windows.h>
#include <vector>
#include <time.h>
#include <math.h>
#include "resource.h"

const int BOARD_WIDTH = 800;
const int BOARD_HEIGHT = 600;
const int GRID = 40;
const int MAP_WIDTH = BOARD_WIDTH / GRID;
const int MAP_HEIGHT = BOARD_HEIGHT / GRID;
const int PLAYER_SIZE = 20;
const int BULLET_SIZE = 15;
const int BULLET_RADIUS = 3;
const double M_PI = 3.141592;

bool g_explosionOccurred = false; // 폭발 여부를 나타내는 전역 변수
int g_explosionX = 0; // 폭발이 발생한 위치 X 좌표
int g_explosionY = 0; // 폭발이 발생한 위치 Y 좌표

int map[MAP_HEIGHT][MAP_WIDTH];

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"InversusGame";

// WinMain 함수
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
    WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_QUESTION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, BOARD_WIDTH + 16, BOARD_HEIGHT + 59, NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

// 전역 변수
struct Player {
    int x, y;
    int dx, dy;
    int bullets;
    int special_bullets;
    double angle = 0;
    bool invincibility = false;
    bool alive = true;
} g_player;

struct Bullet {
    int x, y;
    int dx, dy;
    int type;
};
vector<Bullet> g_bullets;

struct Item {
    int x, y, num;
};
vector<Item> g_items;

struct Enemy {
    int x, y;
    int dx, dy;
    int type;
};
vector<Enemy> g_enemies;

void ProcessKeyboardUp(WPARAM wParam);
void ProcessKeyboardDown(WPARAM wParam);
void InitializeMap(int mapType);
void DrawMap(HDC hdc);
void InitPlayer();
void MovePlayer();
void DrawPlayer(HDC hDC);
void GenerateItem(int x, int y, int num);
void DrawItems(HDC hdc);
void ShootBullet(int dx, int dy);
void MoveBullets();
void DrawBullets(HDC hDC);
void GenerateEnemy();
void MoveEnemies();
void DrawEnemies(HDC hDC);
void DeleteEnemies();
void DrawExplosion(HDC hDC, int x, int y);
void DrawScore(HDC hDC, int score, int combo, int combo_interval);
void CheckCollisions();
void CheckEnemyPlayerCollisions();
void CheckItemPlayerCollisions();
void CheckEnemyBulletCollisions();
void CheckExplosionBulletCollisions();
void CheckExplosionEnemyCollisions();

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {    
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps); 
    HDC mDC;
    HBITMAP hBitmap;
    RECT rt;

    COLORREF COLOR_WHITE = RGB(255, 255, 255);
    COLORREF COLOR_BLACK = RGB(0, 0, 0);

    static int mapType = 1;
    static float enemy_gen_interval = 60;
    static float enemy_gen_interval_minus = 1;
    static int enemy_gen_count = 0;
    static int explosion_time = 10;
    static int shakeAmountX = 0;
    static int shakeAmountY = 0;
    static int score = 0;
    static int combo = 0;
    static int combo_interval = 0;
    static int die_timer = 5;
    static bool ing = false;
    static bool start = false;

    switch (message) {
    case WM_CREATE:
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_GAME_START:
            start = true;
            ing = true;
            InitializeMap(mapType);
            InitPlayer();
            SetTimer(hWnd, 1, 1000 / 60, NULL); // 60 FPS로 설정
            break;
        case ID_GAME_QUIT:
            PostQuitMessage(0);
            break;
        case ID_GAME_PAUSE:
            if (start == false)break;
            if (ing == true) {
                KillTimer(hWnd, 1);
                ing = false;
            }
            else {
                SetTimer(hWnd, 1, 1000 / 60, NULL);
                ing = true;
            } // 60 FPS로 설정
            break;
        case ID_GAME_RESTART:
            if (start == false)break;
            InitializeMap(mapType);
            InitPlayer();
            DeleteEnemies();
            break;
        case ID_LEVEL_1:
            mapType = 0;
            InitializeMap(mapType);
            break;
        case ID_LEVEL_2:
            mapType = 1;
            InitializeMap(mapType);
            break;
        case ID_LEVEL_3:
            mapType = 2;
            InitializeMap(mapType);
            break;
        case ID_LEVEL_INVINCIBILITY:
            mapType = 3;
            InitializeMap(mapType);
            break;
        }
        break;
    case WM_CHAR:
        switch (wParam)
        {
        case 'Q':
        case 'q':
            PostQuitMessage(0);
            break;
        case 'i':
        case 'I':
            start = true;
            ing = true;
            InitializeMap(mapType);
            InitPlayer();
            SetTimer(hWnd, 1, 1000 / 60, NULL); // 60 FPS로 설정
            break;
        case 'P':
        case 'p':
            if (start == false)break;
            if(ing == true){
                KillTimer(hWnd, 1);
                ing = false;
            }
            else{ 
                SetTimer(hWnd, 1, 1000 / 60, NULL);
                ing = true;
            } // 60 FPS로 설정
            break;
        case 'o':
        case 'O':
            if (start == false)break;
            InitializeMap(mapType);
            InitPlayer();
            DeleteEnemies();
            break;
        }
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            if (start == false)break;
            if (g_player.alive == false) {
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 2, 1000 / 1, NULL); // 60 FPS로 설정
            }
            g_player.angle = g_player.angle + 0.1;
            MovePlayer();
            MoveBullets();
            CheckCollisions();
            MoveEnemies();

            combo_interval--;
            if (combo_interval <= 0) { combo = 0; }

            enemy_gen_interval -= enemy_gen_interval_minus;
            if (enemy_gen_interval <= 0) {
                GenerateEnemy();
                enemy_gen_interval = 60;
                enemy_gen_count++;
                if (enemy_gen_count >= 10 && enemy_gen_interval_minus < 5) {
                    enemy_gen_interval_minus += 0.5;
                    enemy_gen_count = 0;
                }
            }
            break;
        case 2:
            if (start == false)break;
            die_timer--;
            if (die_timer <= 0) {
                g_player.alive = true;
                die_timer = 5;
                KillTimer(hWnd, 2);
                SetTimer(hWnd, 1, 1000 / 60, NULL); // 60 FPS로 설정
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_PAINT:
    {
        if (start == false)break;
        GetClientRect(hWnd, &rt);
        mDC = CreateCompatibleDC(hDC);
        hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
        SelectObject(mDC, (HBITMAP)hBitmap);

        //--- 모든 그리기를 메모리 DC에한다.
        DrawMap(mDC);
        DrawPlayer(mDC);
        DrawBullets(mDC);
        DrawEnemies(mDC);
        DrawItems(mDC);
        DrawScore(mDC, score, combo, combo_interval);
        if (g_explosionOccurred) {
            combo++;
            combo_interval = 100;
            score = score + 10 + ((100 - enemy_gen_interval) * enemy_gen_interval_minus);
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

        // 메모리 DC에서 화면 DC로 그림을 복사
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
        if (start == false || g_player.alive == false)break;
        ProcessKeyboardDown(wParam);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_KEYUP:
        if (start == false || g_player.alive == false)break;
        ProcessKeyboardUp(wParam);
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

// 키입력
void ProcessKeyboardDown(WPARAM wParam) {
    switch (wParam) {
    case VK_UP:
        ShootBullet(0, -10); // 위쪽 방향으로 총알 발사
        break;
    case VK_DOWN:
        ShootBullet(0, 10); // 아래쪽 방향으로 총알 발사
        break;
    case VK_LEFT:
        ShootBullet(-10, 0); // 왼쪽 방향으로 총알 발사
        break;
    case VK_RIGHT:
        ShootBullet(10, 0); // 오른쪽 방향으로 총알 발사
        break;

    case 'W':
    case 'w':
        g_player.dy = -1;
        break;
    case 'S':
    case 's':
        g_player.dy = 1;
        break;
    case 'A':
    case 'a':
        g_player.dx = -1;
        break;
    case 'D':
    case 'd':
        g_player.dx = 1;
        break;
    case 'R':
    case 'r':
        g_player.bullets = 6;
        break;
    }
}

void ProcessKeyboardUp(WPARAM wParam) {
    switch (wParam) {
    case 'W':
    case 'w':
        g_player.dy = 0;
        break;
    case 'S':
    case 's':
        g_player.dy = 0;
        break;
    case 'A':
    case 'a':
        g_player.dx = 0;
        break;
    case 'D':
    case 'd':
        g_player.dx = 0;
        break;
    }
}

// 맵
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
    case 0: //네모맵
        for (int y = centerY - 4; y <= centerY + 4; ++y) {
            for (int x = centerX - 4; x <= centerX + 4; ++x) {
                if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                    map[y][x] = 1;
                }
            }
        }
        break;
    case 1: // 마름모맵
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
    case 2: // 마름모맵
        for (int y = centerY - 4; y <= centerY + 4; ++y) {
            for (int x = centerX - 4; x <= centerX + 4; ++x) {
                if (abs(x - centerX) + abs(y - centerY) <= 4) {
                    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                        map[y][x] = 1;
                    }
                }
            }
        }
        // 랜덤한 위치 10곳에 2를 할당
        for (int i = 0; i < 10; ++i) {
            int randX = rand() % MAP_WIDTH;  // 맵 전체에서 무작위 x 위치
            int randY = rand() % MAP_HEIGHT; // 맵 전체에서 무작위 y 위치
            map[randY][randX] = 3;
        }
        break;
    case 3: // 무적맵
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                map[y][x] = 1;
            }
        }
        g_player.invincibility = true;
        break;
    }
}

void DrawMap(HDC hdc) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // 현재 맵의 값이 1이면 흰색(플레이어 영역), 0이면 검은색으로 그립니다.
            if(map[y][x] == 0) {
                // 검은색
                HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
            else if (map[y][x] == 1) {
                // 흰색
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
            else if (map[y][x] == 2) {
                // 빗금
                HBRUSH hBrush = CreateHatchBrush(HS_BDIAGONAL ,RGB(0, 0, 0));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
            else if (map[y][x] == 3) {
                // 빨강
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
        }
    }
}

// 플레이어
void InitPlayer() {
    g_player.x = BOARD_WIDTH / 2 - 20;
    g_player.y = BOARD_HEIGHT / 2;
    g_player.dx = 0;
    g_player.dy = 0;
    g_player.bullets = 6;
    g_player.special_bullets = 0;    
}

void MovePlayer() {
    int currentX = g_player.x;
    int currentY = g_player.y;
    int newX = currentX + g_player.dx * 3;
    int newY = currentY + g_player.dy * 3;

    int leftX = (newX - PLAYER_SIZE / 2) / GRID;
    int rightX = (newX + PLAYER_SIZE / 2 - 1) / GRID;
    int topY = (newY - PLAYER_SIZE / 2) / GRID;
    int bottomY = (newY + PLAYER_SIZE / 2 - 1) / GRID;

    bool canMove = true;

    // 위쪽으로 이동할 때
    if (g_player.dy < 0) {
        if (topY < 0 || map[topY][leftX] == 0 || map[topY][rightX] == 0 
            || map[topY][leftX] == 3 || map[topY][rightX] == 3) {
            canMove = false;
        }
    }
    // 아래쪽으로 이동할 때
    if (g_player.dy > 0) {
        if (bottomY >= MAP_HEIGHT || map[bottomY][leftX] == 0 || map[bottomY][rightX] == 0 
            || map[bottomY][leftX] == 3 || map[bottomY][rightX] == 3) {
            canMove = false;
        }
    }
    // 왼쪽으로 이동할 때
    if (g_player.dx < 0) {
        if (leftX < 0 || map[topY][leftX] == 0 || map[bottomY][leftX] == 0
            || map[topY][leftX] == 3 || map[bottomY][leftX] == 3) {
            canMove = false;
        }
    }
    // 오른쪽으로 이동할 때
    if (g_player.dx > 0) {
        if (rightX >= MAP_WIDTH || map[topY][rightX] == 0 || map[bottomY][rightX] == 0
            || map[topY][rightX] == 3 || map[bottomY][rightX] == 3) {
            canMove = false;
        }
    }
    if (canMove) {
        g_player.x = newX;
        g_player.y = newY;
    }
}

void DrawPlayer(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hBrush);
    Rectangle(hdc, g_player.x - PLAYER_SIZE / 2, g_player.y - PLAYER_SIZE / 2, g_player.x + PLAYER_SIZE / 2, g_player.y + PLAYER_SIZE / 2);
    double angle = 2 * M_PI / g_player.bullets;
    for (int i = 0; i < g_player.bullets; i++) {
        int bulletX = g_player.x + PLAYER_SIZE / 3 * cos(i * angle + g_player.angle);
        int bulletY = g_player.y + PLAYER_SIZE / 3 * sin(i * angle + g_player.angle);

        if (g_player.special_bullets == 0) {
            HBRUSH hBrush1 = CreateSolidBrush(RGB(255, 255, 255));
            SelectObject(hdc, hBrush1);
            DeleteObject(hBrush1);
            Ellipse(hdc, bulletX - BULLET_RADIUS, bulletY - BULLET_RADIUS, bulletX + BULLET_RADIUS, bulletY + BULLET_RADIUS);
        }
        else if (g_player.special_bullets >= 1) {
            HBRUSH hBrush2 = CreateSolidBrush(RGB(0, 255, 255));
            SelectObject(hdc, hBrush2);
            Ellipse(hdc, bulletX - BULLET_RADIUS, bulletY - BULLET_RADIUS, bulletX + BULLET_RADIUS, bulletY + BULLET_RADIUS);
            DeleteObject(hBrush2);
        }
    }
    DeleteObject(hBrush);
}

// 아이템
void GenerateItem(int x, int y, int num) {
    Item newItem;
    newItem.x = x;
    newItem.y = y;
    newItem.num = num;
    g_items.push_back(newItem);
}

void DrawItems(HDC hdc) {
    for (const auto& item : g_items) {
        HBRUSH hBrush1 = CreateSolidBrush(RGB(255, 255, 255));
        SelectObject(hdc, hBrush1);
        Ellipse(hdc, item.x - BULLET_SIZE, item.y - BULLET_SIZE, item.x + BULLET_SIZE, item.y + BULLET_SIZE);
        DeleteObject(hBrush1);
        double angle = 2 * M_PI / item.num;
        for (int i = 0; i < item.num; i++) {
            int bulletX = item.x + BULLET_SIZE / 3 * cos(i * angle);
            int bulletY = item.y + BULLET_SIZE / 3 * sin(i * angle);
            HBRUSH hBrush2 = CreateSolidBrush(RGB(0, 255, 255));
            SelectObject(hdc, hBrush2);
            Ellipse(hdc, bulletX - BULLET_RADIUS, bulletY - BULLET_RADIUS, bulletX + BULLET_RADIUS, bulletY + BULLET_RADIUS);   
            DeleteObject(hBrush2);
        }
    }
}

// 총알
void ShootBullet(int dx, int dy) {
    if (g_player.bullets > 0) {
        if (g_player.special_bullets > 0) { // 스페셜
            for (int i = 0; i < 3; i++) {
                Bullet newBullet;
                if (dx == 0) {
                    newBullet.x = g_player.x + ((i - 1) * GRID);
                    newBullet.y = g_player.y;
                    newBullet.dx = dx * 2;
                    newBullet.dy = dy * 2;
                    newBullet.type = 1;
                    g_bullets.push_back(newBullet);

                }
                else if (dy == 0) {
                    newBullet.x = g_player.x;
                    newBullet.y = g_player.y + ((i - 1) * GRID);
                    newBullet.dx = dx * 2;
                    newBullet.dy = dy * 2;
                    newBullet.type = 1;
                    g_bullets.push_back(newBullet);

                }
            }
            g_player.bullets--;
            g_player.special_bullets--;
        }
        else {
            Bullet newBullet;
            newBullet.x = g_player.x;
            newBullet.y = g_player.y;
            newBullet.dx = dx * 2;
            newBullet.dy = dy * 2;
            newBullet.type = 0;
            g_bullets.push_back(newBullet);
            g_player.bullets--;
        }
    }
}

void MoveBullets() {
    for (auto it = g_bullets.begin(); it != g_bullets.end();) {
        it->x += it->dx;
        it->y += it->dy;
        // 맵의 값을 업데이트 흰색으로
        int gridX = it->x / GRID;
        int gridY = it->y / GRID;
        if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
            if (it->type == 0) {
                if (map[gridY][gridX] != 3) {
                    map[gridY][gridX] = 1;
                }
                else if (map[gridY][gridX] == 3) {
                    it = g_bullets.erase(it);
                    return;
                }
            }
            else {  // 특수
                map[gridY][gridX] = 1;
            }
        }
        // 맵밖으로 나가면 사라짐
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

// 적
void GenerateEnemy() {
    Enemy newEnemy;
    const int range = 100;
    do {
        newEnemy.x = rand() % (BOARD_WIDTH - 100) + 50;
    } while (newEnemy.x > g_player.x + range && newEnemy.x < g_player.x - range);

    do {
        newEnemy.y = rand() % (BOARD_HEIGHT - 100) + 50;
    } while (newEnemy.y > g_player.y + range && newEnemy.y < g_player.y - range);

    newEnemy.dx = rand() % 2 - 1;
    newEnemy.dy = rand() % 2 - 1;
    newEnemy.type = rand() % 2;
    g_enemies.push_back(newEnemy);
}

void DrawEnemies(HDC hdc) {
    for (const auto& enemy : g_enemies) {
        if (enemy.type == 0) {
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 0));
            RECT rect = { enemy.x - PLAYER_SIZE / 2, enemy.y - PLAYER_SIZE / 2, enemy.x + PLAYER_SIZE / 2, enemy.y + PLAYER_SIZE / 2 };
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
        }
        else if (enemy.type == 1) {
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
            RECT rect = { enemy.x - PLAYER_SIZE / 2, enemy.y - PLAYER_SIZE / 2, enemy.x + PLAYER_SIZE / 2, enemy.y + PLAYER_SIZE / 2 };
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
        }
    }
}

void MoveEnemies() {
    for (auto it = g_enemies.begin(); it != g_enemies.end();) {
        if (it->type == 0) {
            it->x += it->dx * 3;
            it->y += it->dy * 3;
        }
        else if (it->type == 1) {
            it->x += cos(atan2(g_player.y - it->y, g_player.x - it->x)) * 3;
            it->y += sin(atan2(g_player.y - it->y, g_player.x - it->x)) * 3;
        }
        
        // 맵의 값을 업데이트 검은색으로
        int gridX = it->x / GRID;
        int gridY = it->y / GRID;
        if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
            map[gridY][gridX] = 0;
        }
        // 주변은 2로
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int newX = gridX + dx;
                int newY = gridY + dy;

                if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT) {
                    if (map[newY][newX] == 1) {
                        map[newY][newX] = 2;
                    }
                }
            }
        }
        // 맵밖으로 나가면 사라짐
        if (it->x < 0 || it->x > BOARD_WIDTH || it->y < 0 || it->y > BOARD_HEIGHT) {
            it = g_enemies.erase(it);
        }
        else {
            ++it;
        }
    }
}

void DeleteEnemies() {
    for (auto it = g_enemies.begin(); it != g_enemies.end();) {
        it = g_enemies.erase(it);
    }
}
// 맵 효과
void DrawExplosion(HDC hdc, int x, int y) {
    int explosionSize = 50;
    COLORREF explosionColor = RGB(255, 165, 0);

    HPEN hPen = CreatePen(PS_SOLID, 1, explosionColor);
    SelectObject(hdc, hPen);

    HBRUSH hBrush = CreateSolidBrush(explosionColor);
    SelectObject(hdc, hBrush);

    Ellipse(hdc, x - explosionSize, y - explosionSize, x + explosionSize, y + explosionSize);

    DeleteObject(hPen);
    DeleteObject(hBrush);
    // 폭발이 맵에 영향을 끼침
    int gridX = x / GRID;
    int gridY = y / GRID;
    for (int y = gridY - 2; y <= gridY + 2; ++y) {
        for (int x = gridX - 2; x <= gridX + 2; ++x) {
            if (abs(x - gridX) + abs(y - gridY) <= 2) {
                if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                    map[y][x] = 1;
                }
            }
        }
    }
}

void DrawScore(HDC hdc, int score, int combo, int combo_interval) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 0, 0));

    wchar_t scoreText[50];
    swprintf(scoreText, 50, L"Score: %d", score);
    TextOut(hdc, 10, 10, scoreText, lstrlen(scoreText));
    wchar_t comboText[50];
    if (combo_interval > 0) {
        swprintf(comboText, 50, L"Combo: %d / Interval: %d", combo, combo_interval);
    }
    else{
        swprintf(comboText, 50, L"Combo: %d / Interval: 0", combo); // 수정
    }
    TextOut(hdc, 10, 20, comboText, lstrlen(comboText));
}

// 충돌 확인 함수
void CheckCollisions() {
    CheckEnemyPlayerCollisions();
    CheckItemPlayerCollisions();
    CheckEnemyBulletCollisions();
    CheckExplosionBulletCollisions();
    CheckExplosionEnemyCollisions();
}

void CheckEnemyPlayerCollisions() {
    for (auto& enemy : g_enemies) {
        if (enemy.x >= g_player.x - PLAYER_SIZE / 2 && enemy.x <= g_player.x + PLAYER_SIZE / 2 &&
            enemy.y >= g_player.y - PLAYER_SIZE / 2 && enemy.y <= g_player.y + PLAYER_SIZE / 2) {
            // 충돌 시 플레이어가 폭발하고 게임 오버 처리 등을 수행
            if (g_player.invincibility) {   // 무적모드
                g_explosionX = g_player.x;
                g_explosionY = g_player.y;
                g_explosionOccurred = true;
                return;
            }
            else {
                g_explosionX = g_player.x;
                g_explosionY = g_player.y;
                g_explosionOccurred = true;
                g_player.alive = false;
            }
        }
    }
}

void CheckItemPlayerCollisions() {
    for (auto it = g_items.begin(); it != g_items.end(); ) {
        if (it->x >= g_player.x - PLAYER_SIZE / 2 && it->x <= g_player.x + PLAYER_SIZE / 2 &&
            it->y >= g_player.y - PLAYER_SIZE / 2 && it->y <= g_player.y + PLAYER_SIZE / 2) {
            if (g_player.bullets + it->num > 6) {
                g_player.bullets = 6;
            }
            else {
                g_player.bullets += it->num;
            }
            g_player.special_bullets += it->num;
            it = g_items.erase(it);
        }
        else {
            ++it;
        }
    }
}

void CheckEnemyBulletCollisions() {
    for (auto it = g_enemies.begin(); it != g_enemies.end(); ) {
        bool collisionDetected = false;
        for (auto& bullet : g_bullets) {
            if (bullet.x >= it->x - PLAYER_SIZE / 2 && bullet.x <= it->x + PLAYER_SIZE / 2 &&
                bullet.y >= it->y - PLAYER_SIZE / 2 && bullet.y <= it->y + PLAYER_SIZE / 2) {
                // 충돌 시 적이 사라짐
                g_explosionX = it->x;
                g_explosionY = it->y;
                g_explosionOccurred = true;
                int num = rand() % 3 + 1;
                GenerateItem(it->x, it->y, num);
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

void CheckExplosionEnemyCollisions() {
    for (auto it = g_enemies.begin(); it != g_enemies.end(); ) {
        if ((it->x - g_explosionX) * (it->x - g_explosionX) + (it->y - g_explosionY) * (it->y - g_explosionY) <= 50 * 50) {
            it = g_enemies.erase(it);
        }
        else {
            ++it;
        }
    }
}
