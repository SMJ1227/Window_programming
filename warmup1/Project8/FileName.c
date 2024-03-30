#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define SIZE 7
#define NUM_STONES 2
#define EDGE 6

typedef struct {
    char shape;
    int row;
    int col;
} Stone;

typedef struct {
    char grid[SIZE][SIZE];
    Stone stones[NUM_STONES];
} Board;

void init_board(Board* board) {
    board->stones[0].shape = '@';
    board->stones[0].row = EDGE;
    board->stones[0].col = EDGE;
    board->stones[1].shape = 'O';
    board->stones[1].row = EDGE;
    board->stones[1].col = EDGE;

    /*for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((i == 0 && j == 3) || 
                (i == 1 && (j >= 2 && j <= 4)) ||
                (i == 2 && (j == 1 || j == 3 || j == 5)) ||
                (i == 3 && j != 3) ||
                (i == 4 && (j == 1 || j == 3 || j == 5)) ||
                (i == 5 && (j >= 2 && j <= 4)) ||
                (i == 6 && j == 3)) {
                board->grid[i][j] = ' ';
            }
            else {
                board->grid[i][j] = '*';
            }
        }
    }*/
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((i == 0 && j == 3) ||
                (i == 1 && (j >= 2 && j <= 5)) ||
                (i == 2 && (j == 1 || j == 3 || j == 4 || j == 5)) ||
                (i == 3 && j != 3) ||
                (i == 4 && (j == 1 || j == 2 || j == 3 || j == 5)) ||
                (i == 5 && (j > 0 && j <= 4)) ||
                (i == 6 && j == 3)) {
                board->grid[i][j] = ' ';
            }
            else {
                board->grid[i][j] = '*';
            }
        }
    }
}

void print_board(Board* board) {
    printf("[플레이어 1: %c] / [플레이어 2: %c]\n\n", board->stones[0].shape, board->stones[1].shape);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c", board->grid[i][j]);
        }
        printf("\n");
    }
}

int prevCollisionRow = -1;
int prevCollisionCol = -1;
int isCollision = 1;
int endGame = 0;
int acrossSwitch = 0;
int stone0Across = 1;
int stone1Acorss = 0;

void move_stone(Board* board, int stoneIndex, int yut) {
    int prevRow = board->stones[stoneIndex].row;
    int prevCol = board->stones[stoneIndex].col;
    int newRow = board->stones[stoneIndex].row;
    int newCol = board->stones[stoneIndex].col;

    for (int i = 0; i < yut; i++) {
        if ((newRow == 0 && newCol == 6) || (newRow == 0 && newCol == 0)) {
            char across;
            printf("대각선 이동을 원하시면 's'를 입력하세요: ");
            scanf(" %c", &across);

            if (across == 's') {
                if (stoneIndex == 0) {
                    stone0Across = 0;
                }
                else {
                    stone1Acorss = 1;
                }
                if (newRow == 0 && newCol == 0) {
                    acrossSwitch = 2;
                }
            }
        }
        /*if ((stoneIndex == stone0Across || stoneIndex == stone1Acorss) && acrossSwitch == 1) {
            newRow++;
            newCol--;
        }   
        else */
        if ((stoneIndex == stone0Across || stoneIndex == stone1Acorss) && acrossSwitch == 2) {
            newRow++;
            newCol++;;
        }
        else {
            if (newRow == 6 && newCol > 0) {
                newCol--;
                if (newCol == 3) newCol--;
            }
            else if (newRow > 0 && newCol == 0) {
                newRow--;
                if (newRow == 3) newRow--;
            }
            else if (newRow == 0 && newCol < 6) {
                newCol++;
                if (newCol == 3) newCol++;
            }
            else if (newRow < 6 && newCol == 6) {
                newRow++;
                if (newRow == 3) newRow++;
            }
        }
        if (newRow == 6 && newCol == 6) {
            printf("Player %d win.\n", stoneIndex + 1);
            endGame++;
            break;
        }
    }

    if (board->grid[newRow][newCol] != '*') {
        if (stoneIndex == 0) {
            printf("Player 2 die.\n");
            prevCollisionRow = prevRow;
            prevCollisionCol = prevCol;
            board->stones[stoneIndex].row = newRow;
            board->stones[stoneIndex].col = newCol;
            board->grid[newRow][newCol] = board->stones[stoneIndex].shape;
            board->grid[prevRow][prevCol] = '*';
            isCollision = 0;
            board->stones[1].row = EDGE;
            board->stones[1].col = EDGE;
        }
        else {
            printf("Player 1 die.\n");
            prevCollisionRow = prevRow;
            prevCollisionCol = prevCol;
            board->stones[stoneIndex].row = newRow;
            board->stones[stoneIndex].col = newCol;
            board->grid[newRow][newCol] = board->stones[stoneIndex].shape;
            board->grid[prevRow][prevCol] = '*';
            isCollision = 0;
            board->stones[0].row = EDGE;
            board->stones[0].col = EDGE;
        }
    }
    else {
        board->grid[newRow][newCol] = board->stones[stoneIndex].shape;
        if (isCollision) { board->grid[prevRow][prevCol] = '*'; }
        board->stones[stoneIndex].row = newRow;
        board->stones[stoneIndex].col = newCol;
        isCollision++;
    }
}

int main() {
    srand(time(NULL));

    Board board;

    char start = '\0';
    char command = '\0';
    int turn = 0;
    int stoneIndex;
    int yut;
    while (1) {
        printf("PRESS P TO START: ");
        scanf(" %c", &start);
        switch (start)
        {
        case 'p':
            init_board(&board);
            print_board(&board);
            break;
        default:
            printf("다시 입력.\n");
            continue;
        }
        break;
    }

    while(1) {
        if (endGame == 1) {
            return 0;
        }
        stoneIndex = turn % 2;
        printf("\nw로 윷 던지기 Player %d , q로 종료 // TURN %d ): ", stoneIndex + 1, turn + 1);
        scanf(" %c", &command);
        switch (command) {
        case 'w':
            yut = rand() % 5 + 1;
            switch (yut) {
            case 1:
                printf("\n도\n\n");
                break;
            case 2:
                printf("\n개\n\n");
                break;
            case 3:
                printf("\n걸\n\n");
                break;
            case 4:
                printf("\n윷\n\n");
                break;
            case 5:
                printf("\n모\n\n");
                break;
            default:
                printf("문제 발생\n");
                return 0;
            }
            move_stone(&board, stoneIndex, yut);
            print_board(&board);
            turn++;
            break;
        case 'q':
            printf("프로그램을 종료합니다.\n");
            return 0;
        default:
            printf("잘못된 명령어입니다. 다시 입력하세요.\n");
            continue;
        }
    }
    init_board(&board);
    print_board(&board);
    return 0;
}
