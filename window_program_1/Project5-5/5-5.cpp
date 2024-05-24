#include <windows.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <atlImage.h>

const int BOARD_WIDTH = 800;
const int BOARD_HEIGHT = 600;
const int GRID = 40;
const int MAP_WIDTH = BOARD_WIDTH / GRID;
const int MAP_HEIGHT = BOARD_HEIGHT / GRID;
const int PLAYER_SIZE = 60;
const int BULLET_SIZE = 15;
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

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, BOARD_WIDTH + 16, BOARD_HEIGHT + 59, NULL, (HMENU)NULL, hInstance, NULL);
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
    int dx, dy;
    int bullets;
    int special_bullets;
    double angle = 0;
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
    int spriteWidth, spriteHeight;
    int frameIndex;
};
vector<Enemy> g_enemies;

struct Disturb {
    int x, y;
};
vector<Disturb> g_disturbs;

void DrawImage(HDC hdc, int x, int y, DWORD dword);
void DrawSpriteImage(HDC hdc, const Enemy& enemy);
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
void DrawEnemies(HDC hDC, CImage imgSprite);
void DeleteEnemies();
void DrawExplosion(HDC hDC, int x, int y);
void CheckCollisions();
void CheckEnemyPlayerCollisions();
void CheckItemPlayerCollisions();
void CheckEnemyBulletCollisions();
void CheckExplosionBulletCollisions();
void CheckExplosionEnemyCollisions();

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);
    HDC mDC;
    HBITMAP hBitmap;
    RECT rt;
    static CImage enemySprite;
    static CImage enemyDieSprite;
    static CImage playerSprite;

    COLORREF COLOR_WHITE = RGB(255, 255, 255);
    COLORREF COLOR_BLACK = RGB(0, 0, 0);

    static int mapType = 3;
    static float enemy_gen_interval = 60;
    static float enemy_gen_interval_minus = 1;
    static int enemy_gen_count = 0;
    static int explosion_time = 10;
    static int dieframeIndex = 0;
    static int dieSpriteWidth;
    static int dieSpriteHeight;
    static int playerFrameIndex = 0;

    switch (message) {
    case WM_CREATE:
        enemySprite.Load(L"bee.png");
        enemyDieSprite.Load(L"bee_die.png");
        dieSpriteWidth = enemyDieSprite.GetWidth() / 4;
        dieSpriteHeight = enemyDieSprite.GetHeight();
        playerSprite.Load(L"kurby.png");
        InitializeMap(mapType);
        InitPlayer();
        SetTimer(hWnd, 1, 1000 / 60, NULL); // 60 FPS�� ����
        break;
    case WM_CHAR:
        switch (wParam)
        {
        case 'Q':
        case 'q':
            PostQuitMessage(0);
            break;
        case 'o':
        case 'O':
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
            g_player.angle = g_player.angle + 0.1;
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

        //--- ��� �׸��⸦ �޸� DC���Ѵ�.
        DrawMap(mDC);
        //DrawPlayer(mDC);
        int playerSpriteX = (playerFrameIndex % 10) * 100;
        playerSprite.Draw(mDC, g_player.x, g_player.y, PLAYER_SIZE, PLAYER_SIZE,
            playerSpriteX + 25, 50, 50, 50);
       playerFrameIndex++;

        DrawBullets(mDC);
        DrawItems(mDC);
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

        // �޸� DC���� ȭ�� DC�� �׸��� ����
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
        
        // �̹����� HDC ����
        image.ReleaseDC();
    }
    else {
        // �̹��� �ε� ���� �� ó��
        MessageBox(NULL, _T("�̹����� �ε��� �� �����ϴ�."), _T("����"), MB_ICONERROR);
    }
}
// Ű�Է�
void ProcessKeyboardDown(WPARAM wParam) {
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
        g_player.dx = 0;
        g_player.dy = -1;
        break;
    case 'S':
    case 's':
        g_player.dx = 0;
        g_player.dy = 1;
        break;
    case 'A':
    case 'a':
        g_player.dx = -1;
        g_player.dy = 0;
        break;
    case 'D':
    case 'd':
        g_player.dx = 1;
        g_player.dy = 0;
        break;
    case 'R':
    case 'r':
        g_player.bullets = 6;
        break;
    }
}
// ��
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
    case 3: // ������
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                map[y][x] = 1;
            }
        }
        // ������ ��ġ 10���� 2�� �Ҵ�
        for (int i = 0; i < 10; ++i) {
            int randX = rand() % MAP_WIDTH;  // �� ��ü���� ������ x ��ġ
            int randY = rand() % MAP_HEIGHT; // �� ��ü���� ������ y ��ġ
            map[randY][randX] = 3;
            Disturb newDisturb;
            newDisturb.x = randX;
            newDisturb.y = randY;
            g_disturbs.push_back(newDisturb);
        }
        break;
    }
}

void DrawMap(HDC hdc) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // ���� ���� ���� 1�̸� ���(�÷��̾� ����), 0�̸� ���������� �׸��ϴ�.
            if (map[y][x] == 0) {
                // ������
                HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
            else if (map[y][x] == 1) {
                // ���
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
                SelectObject(hdc, hBrush);
                Rectangle(hdc, x * GRID, y * GRID, (x + 1) * GRID, (y + 1) * GRID);
                DeleteObject(hBrush);
            }
            else if (map[y][x] == 2) {
                // ����
                DrawImage(hdc, x, y, NOTSRCCOPY);
            }
            else if (map[y][x] == 3) {
                // ����
                DrawImage(hdc, x, y, SRCCOPY);
            }
        }
    }
}

// �÷��̾�
void InitPlayer() {
    g_player.x = BOARD_WIDTH / 2 - 20;
    g_player.y = BOARD_HEIGHT / 2;
    g_player.dx = 1;
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

    // �������� �̵��� ��
    if (g_player.dy < 0) {
        if (topY <= 0 || map[topY][leftX] == 0 || map[topY][rightX] == 0
            || map[topY][leftX] == 3 || map[topY][rightX] == 3) {
            g_player.dy = -g_player.dy;
        }
    }
    // �Ʒ������� �̵��� ��
    if (g_player.dy > 0) {
        if (bottomY >= MAP_HEIGHT || map[bottomY][leftX] == 0 || map[bottomY][rightX] == 0
            || map[bottomY][leftX] == 3 || map[bottomY][rightX] == 3) {
            g_player.dy = -g_player.dy;
        }
    }
    // �������� �̵��� ��
    if (g_player.dx < 0) {
        if (leftX < 0 || map[topY][leftX] == 0 || map[bottomY][leftX] == 0
            || map[topY][leftX] == 3 || map[bottomY][leftX] == 3) {
            g_player.dx = -g_player.dx;
        }
    }
    // ���������� �̵��� ��
    if (g_player.dx > 0) {
        if (rightX >= MAP_WIDTH || map[topY][rightX] == 0 || map[bottomY][rightX] == 0
            || map[topY][rightX] == 3 || map[bottomY][rightX] == 3) {
            g_player.dx = -g_player.dx;
        }
    }
    g_player.x = newX;
    g_player.y = newY;
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
// ������
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
// �Ѿ�
void ShootBullet(int dx, int dy) {
    if (g_player.bullets > 0) {
        if (g_player.special_bullets > 0) { // �����
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
        // ���� ���� ������Ʈ �������
        int gridX = it->x / GRID;
        int gridY = it->y / GRID;
        if (gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT) {
            if (it->type == 0) {
                if (map[gridY][gridX] == 2) {
                    map[gridY][gridX] = 1;
                    for (auto disturb = g_disturbs.begin(); disturb != g_disturbs.end();) {
                        if (disturb->x == gridX && disturb->y == gridY) {
                            disturb = g_disturbs.erase(disturb);
                        }
                        else { disturb++; }
                    }
                    it = g_bullets.erase(it);
                    return;
                }
                else if (map[gridY][gridX] == 3) {
                    map[gridY][gridX] = 2;
                    it = g_bullets.erase(it);
                    return;
                }
            }
            else {  // Ư��
                map[gridY][gridX] = 1;
            }
        }
        // �ʹ����� ������ �����
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
// ��
void GenerateEnemy() {
    int newX, newY, newDx, newDy;
    const int range = 100;
    do {
        newX = rand() % (BOARD_WIDTH - 100) + 50;
    } while (newX > g_player.x + range && newX < g_player.x - range);

    do {
        newY = rand() % (BOARD_HEIGHT - 100) + 50;
    } while (newY > g_player.y + range && newY < g_player.y - range);

    newDx = rand() % 3 - 1;
    newDy = rand() % 3 - 1;

    //for(int i = 0; i < 3; i++){
    Enemy newEnemy;
    newEnemy.x = newX + /*i * */ GRID;
    newEnemy.y = newY;
    newEnemy.dx = newDx;
    newEnemy.dy = newDy;
    newEnemy.spriteWidth = 150;//width / 3;
    newEnemy.spriteHeight = 100;// height;
    newEnemy.frameIndex = 0;
    g_enemies.push_back(newEnemy);
    //}
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
        int currentX = it->x;
        int currentY = it->y;
        int newX = currentX + it->dx * 3;
        int newY = currentY + it->dy * 3;

        int leftX = (newX - PLAYER_SIZE / 2) / GRID;
        int rightX = (newX + PLAYER_SIZE / 2 - 1) / GRID;
        int topY = (newY - PLAYER_SIZE / 2) / GRID;
        int bottomY = (newY + PLAYER_SIZE / 2 - 1) / GRID;

        // �������� �̵��� ��
        if (it->dy < 0) {
            if (topY <= 0 || map[topY][leftX] == 0 || map[topY][rightX] == 0
                || map[topY][leftX] == 3 || map[topY][rightX] == 3) {
                it->dy = -it->dy;
            }
        }
        // �Ʒ������� �̵��� ��
        if (it->dy > 0) {
            if (bottomY >= MAP_HEIGHT || map[bottomY][leftX] == 0 || map[bottomY][rightX] == 0
                || map[bottomY][leftX] == 3 || map[bottomY][rightX] == 3) {
                it->dy = -it->dy;
            }
        }
        // �������� �̵��� ��
        if (it->dx < 0) {
            if (leftX < 0 || map[topY][leftX] == 0 || map[bottomY][leftX] == 0
                || map[topY][leftX] == 3 || map[bottomY][leftX] == 3) {
                it->dx = -it->dx;
            }
        }
        // ���������� �̵��� ��
        if (it->dx > 0) {
            if (rightX >= MAP_WIDTH || map[topY][rightX] == 0 || map[bottomY][rightX] == 0
                || map[topY][rightX] == 3 || map[bottomY][rightX] == 3) {
                it->dx = -it->dx;
            }
        }
        it->x = newX;
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
// �� ȿ��
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
    // ������ �ʿ� ������ ��ħ
    int gridX = x / GRID;
    int gridY = y / GRID;
    for (int y = gridY - 2; y <= gridY + 2; ++y) {
        for (int x = gridX - 2; x <= gridX + 2; ++x) {
            if (abs(x - gridX) + abs(y - gridY) <= 2) {
                if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                    if(map[y][x] != 3) map[y][x] = 1;
                }
            }
        }
    }
}
// �浹 Ȯ�� �Լ�
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
            // �浹 �� �÷��̾ �����ϰ� ���� ���� ó�� ���� ����
            g_explosionX = g_player.x;
            g_explosionY = g_player.y;
            g_explosionOccurred = true;
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
            if (bullet.x >= it->x - GRID / 2 && bullet.x <= it->x + GRID / 2 &&
                bullet.y >= it->y - GRID / 2 && bullet.y <= it->y + GRID / 2) {
                // �浹 �� ���� �����
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
