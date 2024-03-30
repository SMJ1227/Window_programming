#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

#define NUMBERS 5
#define MAX_SIZE 100

int calculate_priority(char* expression) {
    int stack[50];
    char op_stack[50];  
    int top = -1;

    for (int i = 0; expression[i] != '\0'; i++) {
        if (isdigit(expression[i])) {
            int num = 0;
            while (isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            i--;
            stack[++top] = num;
        }
        else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
            while (top != -1 && ((op_stack[top] == '*' || op_stack[top] == '/') || (expression[i] == '+' || expression[i] == '-') && (op_stack[top] == '*' || op_stack[top] == '/'))) {
                int operand2 = stack[top--];
                int operand1 = stack[top--];
                char op = op_stack[top];
                if (op == '+')
                    stack[++top] = operand1 + operand2;
                else if (op == '-')
                    stack[++top] = operand1 - operand2;
                else if (op == '*')
                    stack[++top] = operand1 * operand2;
                else if (op == '/')
                    stack[++top] = operand1 / operand2;
            }
            op_stack[++top] = expression[i];
        }
    }

    while (top != -1) {
        int operand2 = stack[top--];
        int operand1 = stack[top--];
        char op = op_stack[top];
        if (op == '+')
            stack[++top] = operand1 + operand2;
        else if (op == '-')
            stack[++top] = operand1 - operand2;
        else if (op == '*')
            stack[++top] = operand1 * operand2;
        else if (op == '/')
            stack[++top] = operand1 / operand2;
    }
    return stack[0];
}

int calculate_in_order(char* expression) {
    // 순서대로 계산
    int result = 0;
    char op = '+';
    int num = 0;

    for (int i = 0; expression[i] != '\0'; i++) {
        printf("%c", expression[i]);
        if (isdigit(expression[i])) {
            num = num * 10 + (expression[i] - '0');
        }
        else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
            switch (op) {
            case '+':
                result += num;
                break;
            case '-':
                result -= num;
                break;
            case '*':
                result *= num;
                break;
            case '/':
                result /= num;
                break;
            }
            op = expression[i];
            num = 0;
        }
    }

    switch (op) {
    case '+':
        result += num;
        break;
    case '-':
        result -= num;
        break;
    case '*':
        result *= num;
        break;
    case '/':
        result /= num;
        break;
    }

    return result;
}

int calculate_reverse_order(char* expression) {
    int result = 0;
    char op = '+';
    int num = 0;

    for (int i = strlen(expression) - 1; i >= 0; i--) {
        printf("%c", expression[i]);
        if (isdigit(expression[i])) {
            num = num + (expression[i] - '0') * pow(10, strlen(expression) - 1 - i);

        }
        else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
            switch (op) {
            case '+':
                result += num;
                break;
            case '-':
                result -= num;
                break;
            case '*':
                result *= num;
                break;
            case '/':
                result /= num;
                break;
            }
            op = expression[i];
            num = 0;
        }
    }

    switch (op) {
    case '+':
        result += num;
        break;
    case '-':
        result -= num;
        break;
    case '*':
        result *= num;
        break;
    case '/':
        result /= num;
        break;
    }

    return result;
}

void generate_expression(char* expression) {
    srand(time(NULL));
    int num_numbers = NUMBERS;
    int num_operators = NUMBERS - 1;

    int index = 0;
    for (int i = 0; i < num_numbers; i++) {
        int num = rand() % 99 + 1;
        if (i != 0) {
            int op_index = rand() % 4;
            char op;
            switch (op_index) {
            case 0:
                op = '+';
                break;
            case 1:
                op = '-';
                break;
            case 2:
                op = '*';
                break;
            case 3:
                op = '/';
                break;
            }
            expression[index++] = op;
        }
        sprintf(&expression[index], "%d", num);
        index += (int)(log10(num)) + 1;
    }
    expression[index] = '\0';
}

int main() {
    char expression[50];
    char command;

    while (1) {
        printf("계산할 식: ");
        generate_expression(expression);
        printf("%s\n", expression);

        printf("Input the calculating order (1: 연산자 우선순위, 2: 순서대로, 3: 역 순서대로, 4: 식 다시 설정, q: 종료): ");
        scanf(" %c", &command);

        switch (command) {
        case '1':
            printf(" = %d\n", calculate_priority(expression));
            break;
        case '2':
            printf(" = %d\n", calculate_in_order(expression));
            break;
        case '3':
            printf(" = %d\n", calculate_reverse_order(expression));
            break;
        case '4':
            generate_expression(expression);
            break;
        case 'q':
            printf("프로그램을 종료합니다.\n");
            exit(0);
        default:
            printf("잘못된 명령어입니다. 다시 입력하세요.\n");
        }
    }
    return 0;
}
