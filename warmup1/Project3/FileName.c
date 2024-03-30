#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define ROW 5
#define COL 4
#define MAX 50

int matrix[ROW][COL];
int copy[ROW][COL];

void copy_matrix() {
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            copy[j][i] = matrix[j][i];
        }
    }
}
void print_number() {
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            printf("[%2d] ", copy[j][i]);
        }
        printf("\n");
    }
}

void make_number() {
    srand(time(NULL)); // ���� �õ� �ʱ�ȭ
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            int random_number;
            int is_duplicate;
            do {
                random_number = rand() % MAX + 1;
                is_duplicate = 0;
                // �ߺ� �˻�
                for (int k = 0; k < i; k++) {
                    for (int l = 0; l < ROW; l++) {
                        if (matrix[l][k] == random_number) {
                            is_duplicate = 1;
                            break;
                        }
                    }
                    if (is_duplicate) break;
                }
                // ���� �࿡�� �ߺ� �˻�
                for (int l = 0; l < j; l++) {
                    if (matrix[l][i] == random_number) {
                        is_duplicate = 1;
                        break;
                    }
                }
            } while (is_duplicate);
            matrix[j][i] = random_number;
            copy[j][i] = random_number;
        }
    }
    printf("������ ���� ����:\n");
    print_number();
}
// �����ϰ� ���ڸ� ��迭�ϴ� �Լ�

int switch_r = 0;
void shuffle_matrix() {
    if (!switch_r) {
        for (int i = ROW * COL - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = copy[i / COL][i % COL];
            copy[i / COL][i % COL] = copy[j / COL][j % COL];
            copy[j / COL][j % COL] = temp;
        }
        switch_r++;
    }
    else {
        copy_matrix();
        switch_r--;
    }
}

int switch_a = 0;
void sort_rows() {
    if (!switch_a) {
        int sorted_matrix[ROW * COL];
        int k = 0;
        for (int i = 0; i < COL; i++) {
            for (int j = 0; j < ROW; j++) {
                sorted_matrix[k] = matrix[j][i];
                k++;
            }
        }
        for (int i = 0; i < ROW * COL; i++) {
            for (int j = 0; j < ROW * COL - 1 - i; j++) {
                if (sorted_matrix[j] > sorted_matrix[j + 1]) {
                    int temp = sorted_matrix[j];
                    sorted_matrix[j] = sorted_matrix[j + 1];
                    sorted_matrix[j + 1] = temp;
                }
            }
        }
        k = 0;
        for (int i = 0; i < COL; i++) {
            for (int j = 0; j < ROW; j++) {
                copy[j][i] = sorted_matrix[k];
                k++;
            }
        }
        switch_a++;
    }
    else {
        copy_matrix();
        switch_a--;
    }
}

int switch_d = 0;
void sort_cols() {
    if (!switch_d) {
        int sorted_matrix[ROW * COL];
        int k = 0;
        for (int i = 0; i < COL; i++) {
            for (int j = 0; j < ROW; j++) {
                sorted_matrix[k] = matrix[j][i];
                k++;
            }
        }
        for (int i = 0; i < ROW * COL; i++) {
            for (int j = 0; j < ROW * COL - 1 - i; j++) {
                if (sorted_matrix[j] > sorted_matrix[j + 1]) {
                    int temp = sorted_matrix[j];
                    sorted_matrix[j] = sorted_matrix[j + 1];
                    sorted_matrix[j + 1] = temp;
                }
            }
        }
        k = 0;
        for (int i = 0; i < ROW; i++) {
            for (int j = COL-1; j >= 0; j--) {
                copy[i][j] = sorted_matrix[k];
                k++;
            }
        }
        switch_d++;
    }
    else {
        copy_matrix();
        switch_d--;
    }
}

int main(void) {
    make_number();
    while (1) {
        printf("��ɾ �Է��ϼ��� (a,d,e,o,m,n,p,u,r,s,q): ");
        char command;
        int max = 0;
        int min = MAX;
        int sum = 0;
        int mul = 1;
        scanf(" %c", &command);
        switch (command) {
        case 'a':   // 1�� 1������ ���� ���� ��������
            sort_rows();
            print_number();
            break;
        case 'd':   // 1�� 5������ ���� ���� ��������
            sort_cols();
            print_number();
            break;
        case 'e':   // ¦���� ���
            for (int i = 0; i < COL; i++) {
                for (int j = 0; j < ROW; j++) {
                    if (matrix[j][i] % 2 == 0) {
                        printf("[%2d] ", copy[j][i]);
                    }
                    else {
                        printf("[  ] ", copy[j][i]);
                    }
                }
                printf("\n");
            }
            break;
        case 'o':   // Ȧ���� ���
            for (int i = 0; i < COL; i++) {
                for (int j = 0; j < ROW; j++) {
                    if (matrix[j][i] % 2 != 0) {
                        printf("[%2d] ", matrix[j][i]);
                    }
                    else {
                        printf("[  ] ", matrix[j][i]);
                    }
                }
                printf("\n");
            }
            break;
        case 'm':   // �ִ밪�� ã�� ��� ���� ���� ���� ���
            for (int i = 0; i < COL; i++) {
                for (int j = 0; j < ROW; j++) {
                    if (matrix[j][i] > max) {
                        max = matrix[j][i];
                    }
                }
            }
            printf("�ִ밪 = %d\n", max);
            for (int i = 0; i < COL; i++) {
                for (int j = 0; j < ROW; j++) {
                    printf("[%2d] ", copy[j][i] + max);
                }
                printf("\n");
            }
            break;
        case 'n':   // �ּҰ��� ã�� ��� ������ �� ���� ���
            for (int i = 0; i < COL; i++) {
                for (int j = 0; j < ROW; j++) {
                    if (matrix[j][i] < min) {
                        min = matrix[j][i];
                    }
                }
            }
            printf("�ּҰ� = %d\n", min);
            for (int i = 0; i < COL; i++) {
                for (int j = 0; j < ROW; j++) {
                    printf("[%2d] ", copy[j][i] - min);
                }
                printf("\n");
            }
            break;
        case 'p':   // �� ���� ���� ��� ���� �� ���� 1���� ����
            for (int i = 0; i < COL; i++) {
                for (int j = 1; j < ROW; j++) {
                    sum += copy[j][i];
                }
                printf("[%3d]", copy[0][i] + sum);
                for (int j = 1; j < ROW; j++) {
                    printf("[%3d]", copy[j][i]);
                }
                sum = 0;
                printf("\n");
            }
            break;
        case 'u':   // �� ���� ���� ��� ���� �� ���� 4�࿡ ���� (���� �κ�) (�����ǹ���� 5���� ���Ӱ� ����� ����ص� ������)
            print_number();
            for (int j = 0; j < ROW; j++) {
                for (int i = 0; i < COL; i++) {
                    mul *= matrix[j][i];
                }
                printf("[%2d] ", mul);
                mul = 1;
            }
            break;
        case 'r':   // ������ ���ڸ� �����ϰ� ������
            shuffle_matrix();
            print_number();
            break;
        case 's':   // ���ο� ������ ���ڸ� �ٽ� �ڵ� �Է� (����)
            make_number();
            break;
        case 'q':
            printf("���α׷��� �����մϴ�.\n");
            return 0;
        default:
            break;
        }
    }
    return 0;
}
