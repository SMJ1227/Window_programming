#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LENGTH 60
#define SIZE 100

char sentence[SIZE]; // 원본
char copy[MAX_LENGTH]; // 복사본

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
    srand(time(NULL)); // 시드 값 설정
    return (rand() % 4); // 1부터 4까지의 랜덤한 숫자 생성
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
    return count + 1; // 마지막 단어는 공백이 없으므로 +1
}

void switch_case() {
    for (int i = 0; copy[i] != '\0'; i++) {
        if (islower(copy[i])) {
            copy[i] = toupper(copy[i]); // 소문자를 대문자로 바꿈
        }
        else if (isupper(copy[i])) {
            copy[i] = tolower(copy[i]); // 대문자를 소문자로 바꿈
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
        printf("마침표로 마치는 영어 문장을 입력하세요.(최대 %d자): ", MAX_LENGTH);
        fgets(sentence, sizeof(sentence), stdin);
        // 문장의 길이가 최대 길이보다 큰 경우
        if (strlen(sentence) > MAX_LENGTH) {
            printf("입력한 문장이 너무 깁니다.\n");
            continue;
        }
        // 마지막 단어가 .이 아닌 경우
        else if (sentence[strlen(sentence) - 2] != '.') {
            printf("문장은 마침표로 끝나야 합니다.\n");
            continue;
        }
        sentence[strcspn(sentence, ".")] = '\0'; // 마지막 점 제거
        strcpy(copy, sentence);
        print_sentence();
        break;
    }
    
    int switch_num = 0;

    while (1) {
        printf("명령어를 입력하세요 (a-z, 1, 2, 3, 4, 5, 0): ");
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
            printf("단어의 개수: %d\n", count_words());
            continue; // 다음 반복으로 넘어감
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
            printf("프로그램을 종료합니다.\n");
            return 0;
        default:
            if (command >= 'a' && command <= 'z') {
                restore_characters(command);
                break;
            }
            printf("잘못된 명령어입니다. 다시 입력하세요.\n");
            continue;
        }
        print_sentence();
    }
    return 0;
}
