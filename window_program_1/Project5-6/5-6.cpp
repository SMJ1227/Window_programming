#include <windows.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <atlImage.h>

const int BOARD_WIDTH = 400;
const int BOARD_HEIGHT = 600;
const int GRID = 40;
const int MAP_WIDTH = BOARD_WIDTH / GRID;
const int MAP_HEIGHT = BOARD_HEIGHT / GRID;
const int PLAYER_SIZE = 60;
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
    WndClass.lpszMenuName = MAKEINTRESOURCE(NULL);
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
    bool is_invincible;
    bool playerChange;
} g_player;

struct Bullet {
    int x, y;
    int dy;
};
vector<Bullet> g_bullets;

struct Enemy {
    int x, y;
    int dy;
    int spriteWidth, spriteHeight;
    int frameIndex;
};
vector<Enemy> g_enemies;

void DrawImage(HDC hdc, int x, int y, DWORD dword);
void DrawSpriteImage(HDC hdc, const Enemy& enemy);
void ProcessKeyboardDown(WPARAM wParam);
void ProcessKeyboardUp(WPARAM wParam);
void InitPlayer();
void MovePlayer();
void DrawPlayer(HDC hDC);
void ShootBullet();
void MoveBullets();
void DrawBullets(HDC hDC);
void GenerateEnemy();
void MoveEnemies();
void DrawEnemies(HDC hDC, CImage imgSprite);
void DeleteEnemies();
void CheckCollisions();
void CheckEnemyPlayerCollisions();
void CheckEnemyBulletCollisions();

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);
    HDC mDC;
    HBITMAP hBitmap;
    RECT rt;
    static CImage enemySprite;
    static CImage enemyDieSprite;
    static CImage playerSprite;
    static CImage mapSprite;
    static CImage cloudSprite;

    COLORREF COLOR_WHITE = RGB(255, 255, 255);
    COLORREF COLOR_BLACK = RGB(0, 0, 0);

    static double scale = 2;
    static double enemyscale = 2;
    static int mapType = 3;
    static float enemy_gen_interval = 60;
    static float enemy_gen_interval_minus = 1;
    static int enemy_gen_count = 0;
    static int explosion_time = 10;
    static int dieframeIndex = 0;
    static int dieSpriteWidth;
    static int dieSpriteHeight;
    static int playerFrameIndex = 0;
    static int bgSpriteWidth;
    static int bgSpriteHeight;
    static int cloudSpriteWidth;
    static int cloudSpriteHeight;
    static int offsetY = 0;
    static int cloudoffsetY1 = 0;
    static int cloudoffsetY2 = 0;

    switch (message) {
    case WM_CREATE:
        enemySprite.Load(L"bee.png");
        enemyDieSprite.Load(L"bee_die.png");
        dieSpriteWidth = enemyDieSprite.GetWidth() / 4;
        dieSpriteHeight = enemyDieSprite.GetHeight();
        playerSprite.Load(L"kurby.png");
        mapSprite.Load(L"bg_city.png");
        cloudSprite.Load(L"clouds.png");
        bgSpriteWidth = mapSprite.GetWidth();
        bgSpriteHeight = mapSprite.GetHeight();
        cloudSpriteWidth = cloudSprite.GetWidth();
        cloudSpriteHeight = cloudSprite.GetHeight();
        InitPlayer();
        SetTimer(hWnd, 1, 1000 / 60, NULL); // 60 FPS로 설정
        break;
    case WM_CHAR:
        switch (wParam)
        {
        case 'Q':
        case 'q':
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            offsetY++;
            cloudoffsetY1 += 2;
            cloudoffsetY2 += 2;
            if (offsetY >= BOARD_HEIGHT / 2) {
                offsetY = 1;
            }
            if (cloudoffsetY1 > BOARD_HEIGHT / 2) {
                cloudoffsetY1 = 1;
            }
            if (cloudoffsetY2 > BOARD_HEIGHT) {
                cloudoffsetY2 = 1;
            }
            MovePlayer();
            MoveBullets();
            CheckCollisions();
            MoveEnemies();

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
            if (g_player.playerChange) {
                scale += 0.1;
                if (scale >= 4) {
                    g_player.playerChange = false;
                }
            }
            else {
                if (scale <= 2) {
                    scale = 2;
                    g_player.is_invincible = false;
                    break;
                }
                scale -= 0.1;
            }
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_PAINT:
    {
        GetClientRect(hWnd, &rt);
        mDC = CreateCompatibleDC(hDC);
        hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
        SelectObject(mDC, (HBITMAP)hBitmap);

        //--- 모든 그리기를 메모리 DC에한다.
        mapSprite.Draw(mDC, 0, 0, BOARD_WIDTH, offsetY, 0, bgSpriteHeight - offsetY, bgSpriteWidth, offsetY);
        mapSprite.Draw(mDC, 0, offsetY, BOARD_WIDTH, BOARD_HEIGHT, 0, 0, bgSpriteWidth, bgSpriteHeight);
       
        cloudSprite.Draw(mDC, 0, cloudoffsetY1 * 2, 100, 100, 0, 0, bgSpriteWidth, bgSpriteHeight);
        cloudSprite.Draw(mDC, 200, cloudoffsetY2, 100, 100, 0, 0, bgSpriteWidth, bgSpriteHeight);

        int playerSpriteX = (playerFrameIndex % 10) * 100;
        playerSprite.Draw(mDC, g_player.x, g_player.y, PLAYER_SIZE * scale / 2, PLAYER_SIZE * scale / 2,
            playerSpriteX + 25, 50, 50, 50);
        playerFrameIndex++;

        DrawBullets(mDC);
        //DrawEnemies(mDC, imgSprite);
        for (const auto& enemy : g_enemies) {
            int spriteX = (enemy.frameIndex % 3) * enemy.spriteWidth;
            enemySprite.Draw(mDC, enemy.x, enemy.y, GRID * 1.5, GRID,
                spriteX, 0, enemy.spriteWidth, enemy.spriteHeight);
        }
        if (g_explosionOccurred) {
            int spriteX = (dieframeIndex % 4) * dieSpriteWidth;
            enemyDieSprite.Draw(mDC, g_explosionX, g_explosionY, GRID * 1.5, GRID,
                spriteX, 0, dieSpriteWidth, dieSpriteHeight);
            dieframeIndex = (dieframeIndex + 1);
            explosion_time--;
            if (explosion_time <= 0) {
                g_explosionOccurred = false;
                g_explosionX = 0;
                g_explosionY = 0;
                explosion_time = 10;
            }
        }

        // 메모리 DC에서 화면 DC로 그림을 복사
        BitBlt(hDC, 0, 0, BOARD_WIDTH, BOARD_HEIGHT, mDC, 0, 0, SRCCOPY);


        DeleteDC(mDC);
        DeleteObject(hBitmap);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_KEYDOWN:
        ProcessKeyboardDown(wParam);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_KEYUP:
        ProcessKeyboardUp(wParam);
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_DESTROY:
        enemySprite.Destroy();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void DrawImage(HDC hdc, int x, int y, DWORD dword) {
    CImage image;
    HRESULT hr = image.Load(_T("disturb.png"));
    if (SUCCEEDED(hr)) {
        StretchBlt(hdc, x * GRID, y * GRID, GRID, GRID, image.GetDC(),
            0, 0, image.GetWidth(), image.GetHeight(), dword);

        // 이미지의 HDC 해제
        image.ReleaseDC();
    }
    else {
        // 이미지 로드 실패 시 처리
        MessageBox(NULL, _T("이미지를 로드할 수 없습니다."), _T("오류"), MB_ICONERROR);
    }
}
// 키입력
void ProcessKeyboardDown(WPARAM wParam) {
    switch (wParam) {
    case VK_LEFT:
        g_player.dx = -1;
        break;
    case VK_RIGHT:
        g_player.dx = 1;
        break;
    case VK_SPACE:
        ShootBullet();
        break;
    case VK_RETURN:
        g_player.playerChange = true;
        g_player.is_invincible = true;
        break;  
    }
}

void ProcessKeyboardUp(WPARAM wParam) {
    switch (wParam) {
    case VK_LEFT:
        g_player.dx = 0;
        break;
    case VK_RIGHT:
        g_player.dx = 0;
        break;
    }
}

// 플레이어
void InitPlayer() {
    g_player.x = BOARD_WIDTH / 2 - 20;
    g_player.y = BOARD_HEIGHT / 2 + 200;
    g_player.dx = 0;
    g_player.dy = 0;
    g_player.playerChange = false;
}

void MovePlayer() {
    int currentX = g_player.x;
    int currentY = g_player.y;
    int newX = currentX + g_player.dx * 3;
    int newY = currentY + g_player.dy * 3;

    g_player.x = newX;
    g_player.y = newY;
}

void ShootBullet() {
    Bullet newBullet;
    newBullet.x = g_player.x + GRID;
    newBullet.y = g_player.y;
    newBullet.dy = -15;
    g_bullets.push_back(newBullet);
}

void MoveBullets() {
    for (auto it = g_bullets.begin(); it != g_bullets.end();) {
        it->y += it->dy;
        int gridY = it->y / GRID;
        // 맵밖으로 나가면 사라짐
        if (it->y < 0 || it->y > BOARD_HEIGHT) {
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
    int num = rand() % 4 + 1;
    Enemy newEnemy;
    newEnemy.x = 100 * num;
    newEnemy.y = 100;
    newEnemy.dy = 1;
    newEnemy.spriteWidth = 150;
    newEnemy.spriteHeight = 100;
    newEnemy.frameIndex = 0;
    g_enemies.push_back(newEnemy);
}

void DrawEnemies(HDC hdc, CImage enemySprite) {
    for (const auto& enemy : g_enemies) {
        int spriteX = (enemy.frameIndex % 3) * enemy.spriteWidth;
        enemySprite.Draw(hdc, enemy.x, enemy.y, GRID * 1.5, GRID,
            spriteX, 0, enemy.spriteWidth, enemy.spriteHeight);
    }
}

void MoveEnemies() {
    for (auto it = g_enemies.begin(); it != g_enemies.end();) {
        int currentY = it->y;
        int newY = currentY + it->dy * 3;
        
        it->y = newY;
        it->frameIndex++;
        it++;
    }
}

void DeleteEnemies() {
    for (auto it = g_enemies.begin(); it != g_enemies.end();) {
        it = g_enemies.erase(it);
    }
}

// 충돌 확인 함수
void CheckCollisions() {
    if (!g_player.is_invincible) {
        CheckEnemyPlayerCollisions();
    }
    CheckEnemyBulletCollisions();
}

void CheckEnemyPlayerCollisions() {
    for (auto& enemy : g_enemies) {
        if (enemy.x >= g_player.x - PLAYER_SIZE / 2 && enemy.x <= g_player.x + PLAYER_SIZE / 2 &&
            enemy.y >= g_player.y - PLAYER_SIZE / 2 && enemy.y <= g_player.y + PLAYER_SIZE / 2) {
            // 충돌 시 플레이어가 폭발하고 게임 오버 처리 등을 수행
            g_explosionX = g_player.x;
            g_explosionY = g_player.y;
            g_explosionOccurred = true;
        }
    }
}

void CheckEnemyBulletCollisions() {
    for (auto it = g_enemies.begin(); it != g_enemies.end(); ) {
        bool collisionDetected = false;
        for (auto& bullet : g_bullets) {
            if (bullet.x >= it->x - GRID / 2 && bullet.x <= it->x + GRID / 2 &&
                bullet.y >= it->y - GRID / 2 && bullet.y <= it->y + GRID / 2) {
                // 충돌 시 적이 사라짐
                g_explosionX = it->x;
                g_explosionY = it->y;
                g_explosionOccurred = true;
                int num = rand() % 3 + 1;
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

