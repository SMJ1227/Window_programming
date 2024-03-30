#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LENGTH 60
#define SIZE 100

char sentence[SIZE]; // ����
char copy[MAX_LENGTH]; // ���纻

void print_sentence() {
    printf("%s.\n", copy);
}

void restore_characters(char command) {
    for (int i = 0; copy[i] != '\0'; i++) {
        if (copy[i] == command) {
            copy[i] = '*';
        }
        else if (sentence[i] == command && copy[i] == '*'){
            copy[i] = command;
        }
    }
}

int random_num() {
    srand(time(NULL)); // �õ� �� ����
    return (rand() % 4); // 1���� 4������ ������ ���� ����
}

void replace_spaces() {
    char symbols[] = { '@', '%', '#', '.' };
    int random = random_num();
    for (int i = 0; copy[i] != '\0'; i++) {
        if (copy[i] == ' ') {
            copy[i] = symbols[random];
        }
    }
}

void replace_symbols() {
    for (int i = 0; copy[i] != '\0'; i++) {
        if (copy[i] == '@' || copy[i] == '%' || copy[i] == '#' || copy[i] == '.') {
            copy[i] = ' ';
        }
    }
}

int count_words() {
    int count = 0;
    for (int i = 0; copy[i] != '\0'; i++) {
        if (sentence[i] == ' ') {
            count++;
        }
    }
    if (sentence[strlen(sentence) - 1] == ' ') {
        count--;
    }
    return count + 1; // ������ �ܾ�� ������ �����Ƿ� +1
}

void switch_case() {
    for (int i = 0; copy[i] != '\0'; i++) {
        if (islower(copy[i])) {
            copy[i] = toupper(copy[i]); // �ҹ��ڸ� �빮�ڷ� �ٲ�
        }
        else if (isupper(copy[i])) {
            copy[i] = tolower(copy[i]); // �빮�ڸ� �ҹ��ڷ� �ٲ�
        }
    }
}

void shift_left() {
    int length = strlen(copy);
    char first_char = copy[0];
    for (int i = 0; i < length - 1; i++) {
        copy[i] = copy[i + 1];
    }
    copy[length - 1] = first_char;
}

void shift_right() {
    int length = strlen(copy);
    char last_char = copy[length - 1];
    for (int i = length - 1; i > 0; i--) {
        copy[i] = copy[i - 1];
    }
    copy[0] = last_char;
}

int main() {
    while (1) {
        printf("��ħǥ�� ��ġ�� ���� ������ �Է��ϼ���.(�ִ� %d��): ", MAX_LENGTH);
        fgets(sentence, sizeof(sentence), stdin);
        // ������ ���̰� �ִ� ���̺��� ū ���
        if (strlen(sentence) > MAX_LENGTH) {
            printf("�Է��� ������ �ʹ� ��ϴ�.\n");
            continue;
        }
        // ������ �ܾ .�� �ƴ� ���
        else if (sentence[strlen(sentence) - 2] != '.') {
            printf("������ ��ħǥ�� ������ �մϴ�.\n");
            continue;
        }
        sentence[strcspn(sentence, ".")] = '\0'; // ������ �� ����
        strcpy(copy, sentence);
        print_sentence();
        break;
    }
    
    int switch_num = 0;

    while (1) {
        printf("��ɾ �Է��ϼ��� (a-z, 1, 2, 3, 4, 5, 0): ");
        char command;
        scanf(" %c", &command);

        switch (command) {
        case '1':
            if (switch_num == 0) {
                replace_spaces();
                switch_num++;
                break;
            }
            else if (switch_num == 1) {
                replace_symbols();
                switch_num--;
                break;
            }
        case '2':
            printf("�ܾ��� ����: %d\n", count_words());
            continue; // ���� �ݺ����� �Ѿ
        case '3':
            switch_case();
            break;
        case '4':
            shift_left();
            break;
        case '5':
            shift_right();
            break;
        case '0':
            printf("���α׷��� �����մϴ�.\n");
            return 0;
        default:
            if (command >= 'a' && command <= 'z') {
                restore_characters(command);
                break;
            }
            printf("�߸��� ��ɾ��Դϴ�. �ٽ� �Է��ϼ���.\n");
            continue;
        }
        print_sentence();
    }
    return 0;
}
