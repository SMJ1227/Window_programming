#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define SIZE 10
#define NUM_STONES 2

typedef struct {
    char shape;
    int row;
    int col;
} Stone;

typedef struct {
    char grid[SIZE][SIZE];
    Stone stones[2];
} Board;

void initBoard(Board* board) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board->grid[i][j] = ' ';
        }
    }
    for (int k = 0; k < 2; k++) {
        int row, col;
        char shape;
        do {
            row = rand() % SIZE;
            col = rand() % SIZE;
            shape = "ox#@*"[rand() % 5];
        } while (board->grid[row][col] != ' ' || (k == 1 && shape == board->stones[0].shape));
        board->grid[row][col] = board->stones[k].shape = shape;
        board->stones[k].row = row;
        board->stones[k].col = col;
    }
}

void printBoard(Board* board) {
    printf("플레이어 1: %c / 플레이어 2: %c\n", board->stones[0].shape, board->stones[1].shape);
    printf("-----------------------------------------\n");
    for (int i = 0; i < SIZE; i++) {
        printf("|");
        for (int j = 0; j < SIZE; j++) {
            printf(" %c |", board->grid[i][j]);
        }
        printf("\n-----------------------------------------\n");
    }
}

int prevCollisionRow = -1;
int prevCollisionCol = -1;
int isCollision = 1;

void moveStone(Board* board, int stoneIndex, char direction) {
    int row = board->stones[stoneIndex].row;
    int col = board->stones[stoneIndex].col;

    int newRow = row, newCol = col;
    switch (direction) {
    case 'w':
    case 'i':
        newRow = (row - 1 + SIZE) % SIZE;
        if (newRow == 0) {
            // 이전 돌 모양을 기억합니다.
            char prevShape = board->stones[stoneIndex].shape;

            // 새로운 돌 모양을 선택합니다.
            char newShape;
            do {
                newShape = "ox#@*"[rand() % 5];
            } while (newShape == prevShape);

            // 현재 게임 보드에 있는 다른 돌의 모양과 겹치지 않도록 확인합니다.
            int i;
            for (i = 0; i < NUM_STONES; ++i) {
                if (i != stoneIndex && board->stones[i].shape == newShape) {
                    // 겹치는 돌 모양을 발견하면 다시 새로운 돌 모양을 선택합니다.
                    do {
                        newShape = "ox#@*"[rand() % 5];
                    } while (newShape == prevShape);
                    break;
                }
            }

            // 새로운 돌 모양을 보드에 반영합니다.
            board->stones[stoneIndex].shape = newShape;
        }
        break;
    case 'a':
    case 'j':
        newCol = (col - 1 + SIZE) % SIZE;
        if (newCol == 0) {
            // 이전 돌 모양을 기억합니다.
            char prevShape = board->stones[stoneIndex].shape;

            // 새로운 돌 모양을 선택합니다.
            char newShape;
            do {
                newShape = "ox#@*"[rand() % 5];
            } while (newShape == prevShape);

            // 현재 게임 보드에 있는 다른 돌의 모양과 겹치지 않도록 확인합니다.
            int i;
            for (i = 0; i < NUM_STONES; ++i) {
                if (i != stoneIndex && board->stones[i].shape == newShape) {
                    // 겹치는 돌 모양을 발견하면 다시 새로운 돌 모양을 선택합니다.
                    do {
                        newShape = "ox#@*"[rand() % 5];
                    } while (newShape == prevShape);
                    break;
                }
            }

            // 새로운 돌 모양을 보드에 반영합니다.
            board->stones[stoneIndex].shape = newShape;
        }
        break;
    case 's':
    case 'k':
        newRow = (row + 1 + SIZE) % SIZE;
        if (newRow == SIZE - 1) {
            // 이전 돌 모양을 기억합니다.
            char prevShape = board->stones[stoneIndex].shape;

            // 새로운 돌 모양을 선택합니다.
            char newShape;
            do {
                newShape = "ox#@*"[rand() % 5];
            } while (newShape == prevShape);

            // 현재 게임 보드에 있는 다른 돌의 모양과 겹치지 않도록 확인합니다.
            int i;
            for (i = 0; i < NUM_STONES; ++i) {
                if (i != stoneIndex && board->stones[i].shape == newShape) {
                    // 겹치는 돌 모양을 발견하면 다시 새로운 돌 모양을 선택합니다.
                    do {
                        newShape = "ox#@*"[rand() % 5];
                    } while (newShape == prevShape);
                    break;
                }
            }

            // 새로운 돌 모양을 보드에 반영합니다.
            board->stones[stoneIndex].shape = newShape;
        }
        break;
    case 'd':
    case 'l':
        newCol = (col + 1 + SIZE) % SIZE;
        if (newCol == SIZE - 1) {
            // 이전 돌 모양을 기억합니다.
            char prevShape = board->stones[stoneIndex].shape;

            // 새로운 돌 모양을 선택합니다.
            char newShape;
            do {
                newShape = "ox#@*"[rand() % 5];
            } while (newShape == prevShape);

            // 현재 게임 보드에 있는 다른 돌의 모양과 겹치지 않도록 확인합니다.
            int i;
            for (i = 0; i < NUM_STONES; ++i) {
                if (i != stoneIndex && board->stones[i].shape == newShape) {
                    // 겹치는 돌 모양을 발견하면 다시 새로운 돌 모양을 선택합니다.
                    do {
                        newShape = "ox#@*"[rand() % 5];
                    } while (newShape == prevShape);
                    break;
                }
            }

            // 새로운 돌 모양을 보드에 반영합니다.
            board->stones[stoneIndex].shape = newShape;
        }
        break;
    }


    if (board->grid[newRow][newCol] != ' ') {
        printf("Beep! Collision occurred.\n");
        Beep(750, 300); // 비프 소리 재생
        prevCollisionRow = row;
        prevCollisionCol = col;
        board->stones[stoneIndex].row = newRow;
        board->stones[stoneIndex].col = newCol;
        board->grid[newRow][newCol] = board->stones[stoneIndex].shape;
        board->grid[row][col] = ' ';
        isCollision = 0;
    } else {
        board->grid[newRow][newCol] = board->stones[stoneIndex].shape;
        if (isCollision){ board->grid[row][col] = ' '; }
        board->stones[stoneIndex].row = newRow;
        board->stones[stoneIndex].col = newCol;
        isCollision++;
    }
}

int main() {
    srand(time(NULL));

    Board board;
    initBoard(&board);
    printBoard(&board);

    char command = '\0';
    int turn = 0;
    int stoneIndex;

    do {
        if (turn % 2 == 0) {
            printf("명령어를 입력하세요 (w/a/s/d for stone 1, q to quit, r to reset // TURN %d ): ", turn + 1);
            scanf(" %c", &command);
            switch (command) {
            case 'w': case 'a': case 's': case 'd':
                stoneIndex = turn % 2;
                moveStone(&board, stoneIndex, command);
                printBoard(&board);
                turn++;
                break;
            case 'r':
                initBoard(&board);
                printBoard(&board);
                break;
            case 'q':
                printf("프로그램을 종료합니다.\n");
                return 0;
            default:
                printf("잘못된 명령어입니다. 다시 입력하세요.\n");
            }
        }
        else {
            printf("명령어를 입력하세요 (i/j/k/l for stone 2, q to quit, r to reset // TURN %d ): ", turn + 1);
            scanf(" %c", &command);
            switch (command) {
            case 'i': case 'j': case 'k': case 'l':
                stoneIndex = turn % 2;
                moveStone(&board, stoneIndex, command);
                printBoard(&board);
                turn++;
                break;
            case 'r':
                initBoard(&board);
                printBoard(&board);
                break;
            case 'q':
                printf("프로그램을 종료합니다.\n");
                return 0;
            default:
                printf("잘못된 명령어입니다. 다시 입력하세요.\n");
            }
        }
    } while (command != 'q');
    return 0;
}
