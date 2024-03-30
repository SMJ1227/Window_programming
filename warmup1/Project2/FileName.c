#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define RANDOM 50
int random_numbers[RANDOM];
int copy[RANDOM];
int number;

void get_number() {
    int result;
    while (1) {
        printf("100���� ũ�� 300���� ���� ���� �ϳ��� �Է��Ͻÿ�. : ");
        result = scanf("%d", &number);
        if (result != 1) {
            printf("���ڸ� �Է��ϼ���.\n");
            while (getchar() != '\n');  // �Է� ���۸� �����
            continue;
        }
        if (number < 100 || number > 300) {
            printf("100���� ũ�� 300���� ���� ���ڸ� �Է��ϼ���.\n");
            continue;
        }
        break;
    }
}
void print_number() {
    for (int i = 0; i < 50; i++) {
        printf("%d ", copy[i]);
    }
    printf("\n");
}
void make_number(number) {
    rand(time(NULL)); // ���� �õ� �ʱ�ȭ
    int i = 0;
    while (i < 50) {
        int random_number = rand() % number;
        // �ߺ����� �ʴ��� Ȯ��
        int is_duplicate = 0;
        for (int j = 0; j < i; j++) {
            if (random_numbers[j] == random_number) {
                is_duplicate = 1;
                break;
            }
        }
        if (!is_duplicate) {
            random_numbers[i] = random_number;
            copy[i] = random_number;
            i++;
        }
    }
    printf("������ ���� ����:\n");
    print_number();
}

int main(void) {
    get_number(number);
    make_number(number);
    while (1) {
        printf("��ɾ �Է��ϼ��� (a,d,e,f,m,n,r,p,q): ");
        char command;
        int sum;
        int count;
        int array[100];
        scanf(" %c", &command);
        switch (command) {
        case 'a':
            // ������������ ����
            for (int i = 0; i < RANDOM - 1; i++) {
                for (int j = 0; j < RANDOM - 1 - i; j++) {
                    if (copy[j] > copy[j + 1]) {
                        int temp = copy[j];
                        copy[j] = copy[j + 1];
                        copy[j + 1] = temp;
                    }
                }
            }
            printf("�������� ����:\n");
            for (int i = 0; i < RANDOM; i++) {
                printf("%d ", copy[i]);
            }
            printf("\n");
            break;
        case 'd':
            // ������������ ����
            for (int i = 0; i < RANDOM - 1; i++) {
                for (int j = 0; j < RANDOM - 1 - i; j++) {
                    if (copy[j] < copy[j + 1]) {
                        int temp = copy[j];
                        copy[j] = copy[j + 1];
                        copy[j + 1] = temp;
                    }
                }
            }
            printf("�������� ����: ");
            for (int i = 0; i < RANDOM; i++) {
                printf("%d ", copy[i]);
            }
            printf("\n");
            break;
        case 'e':
            sum = 0;
            for (int i = 0; i < 50; i++) {
                if (copy[i] % 3 == 0) {
                    printf("%d ", copy[i]);
                    sum += copy[i];
                }
            }
            printf("-> %d\n", sum);
            break;
        case 'f':
            sum = 0;
            for (int i = 0; i < 50; i++) {
                if (copy[i] % 5 == 0) {
                    printf("%d ", copy[i]);
                    sum += copy[i];
                }
            }
            printf("-> %d\n", sum);
            break;
        case 'm':
            count = 0;
            for (int i = 0; i < 50; i++) {
                if (copy[i] % 3 == 0 || copy[i] % 5 == 0) {
                    array[count] = copy[i];
                    count++;
                }
            }
            for (int i = 0; i < count; i++) {
                printf("%d ", array[i]);
            }
            printf("->\n");
            for (int i = 0; i < count; i++) {
                printf("%d ", array[i]+1);
            }
            printf("\n");
            break;
        case 'n':
            count = 0;
            for (int i = 0; i < 50; i++) {
                if (copy[i] % 3 == 0 && copy[i] % 5 == 0) {
                    array[count] = copy[i];
                    count++;
                }
            }
            for (int i = 0; i < count; i++) {
                printf("%d ", array[i]);
            }
            printf("->\n");
            for (int i = 0; i < count; i++) {
                printf("%d ", array[i] + 1);
            }
            printf("\n");
            break;
        case 'r':
            for (int i = 0; i < 50; i++) {
                copy[i] = random_numbers[i];
            }
            print_number();
            break;
        case 'p':
            get_number(number);
            make_number(number);
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
