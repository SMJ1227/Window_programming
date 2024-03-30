#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_CLASS 13
#define MAX_CANDIDATES 1000
#define NUM_REPRESENTATIVES 109
#define VOTE 1000

typedef struct {
    int id;
    int class_number;
    int votes;
} Candidate;

int reps[] = { 9, 12, 9, 13, 10, 6, 10, 6, 9, 5, 6, 7, 7 };
int vote[VOTE];

void select_representatives_per_class(int class_representatives[]) {    // �� ���� ���ǿ� �� ������ ����
    printf("�� �� ���ǿ�:\n");
    int total_representatives = 0;

    for (int i = 0; i < NUM_CLASS; ++i) {
        class_representatives[i] = reps[i];
        total_representatives += reps[i];
        printf("[%d] ", reps[i]);
    }
    printf("\n����: %d\n", total_representatives);
}

void assign_class_to_candidates(Candidate candidates[], int num_candidates) {  // �ĺ��ڿ��� �� �Ҵ�
    for (int i = 0; i < num_candidates; ++i) {
        candidates[i].id = i + 1;
        candidates[i].class_number = rand() % NUM_CLASS + 1;
        candidates[i].votes = 0;
        //printf("%3d// CLASS: %2d\n", candidates[i].id, candidates[i].class_number);
    }
    for (int i = 0; i < VOTE; i++) {
        vote[i] = rand() % num_candidates;
        candidates[vote[i]].votes++;
        //printf("%d\n", vote[i]);
    }
    for (int i = 0; i < num_candidates; i++) {
        //printf("%d: %d\n", i, candidates[i].votes);
    }
}

void sort_candidates_upper(Candidate candidates[], int num_candidates) {    // �ĺ��ڵ��� ������������ ����
    for (int i = 0; i < num_candidates - 1; i++) {
        for (int j = 0; j < num_candidates - i - 1; ++j) {
            if (candidates[j].id > candidates[j + 1].id) {
                Candidate temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }
    printf("�ݺ� ��������:\n");
    for (int i = 1; i <= NUM_CLASS; ++i) {
        printf("CLASS [%2d]\n", i);
        for (int j = 0; j < num_candidates; j++) {
            if (candidates[j].class_number == i) {
                printf("�ĺ��� %d (��ǥ�� %d)\n", candidates[j].id, candidates[j].votes);
            }
        }
        printf("\n");
    }
}

void sort_candidates_desc(Candidate candidates[], int num_candidates) { // �ĺ��ڵ��� ������������ ����
    for (int i = 0; i < num_candidates - 1; i++) {
        for (int j = 0; j < num_candidates - i - 1; ++j) {
            if (candidates[j].id < candidates[j + 1].id) {
                Candidate temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }

    printf("�ݺ� ��������:\n");
    for (int i = 1; i <= NUM_CLASS; ++i) {
        printf("CLASS [%2d]\n", i);
        for (int j = 0; j < num_candidates; j++) {
            if (candidates[j].class_number == i) {
                printf("�ĺ��� %d (��ǥ�� %d)\n", candidates[j].id, candidates[j].votes);
            }
        }
        printf("\n");
    }
}

void print_elected_per_class(Candidate candidates[], int num_candidates) { // (1�� ���)
    for (int i = 0; i < num_candidates - 1; i++) {
        for (int j = 0; j < num_candidates - i - 1; ++j) {
            if (candidates[j].votes < candidates[j + 1].votes) {
                Candidate temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }
    int candidates_count;
    printf("�ݸ��� �缱:\n");
    for (int i = 1; i <= NUM_CLASS; i++) {
        printf("CLASS [%2d]\n", i);
        candidates_count = 0;
        for (int j = 0; j < num_candidates; j++) {
            if (reps[i - 1] <= candidates_count) {
                break;
            }
            if (candidates[j].class_number == i) {
                printf("[%d-%d] �ĺ��� %d (��ǥ�� %d)\n", reps[i-1], candidates_count+1, candidates[j].id, candidates[j].votes);
                candidates_count++;
            }
        }
        printf("\n");
    }
}

void print_global_elected(Candidate candidates[], int num_candidates) { // (2�� ���)
    printf("��ü �ٵ�ǥ ��\n");
    for (int i = 0; i < num_candidates - 1; i++) {
        for (int j = 0; j < num_candidates - i - 1; ++j) {
            if (candidates[j].votes < candidates[j + 1].votes) {
                Candidate temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i <= 109; i++) {
        printf("�缱 ���� %3d: �ĺ��� %3d (��ǥ�� %2d)\n", i, candidates[i].id, candidates[i].votes);
    }
}

int main() {
    srand(time(NULL));

    int class_representatives[NUM_CLASS];
    Candidate candidates[MAX_CANDIDATES];
    int num_candidates = 0;
    char command;
    int num;

    while (1) {
        printf("\nCommands: p, v, a, d, m, n, r, q\n");
        printf("�Է�: ");
        scanf(" %c", &command);

        switch (command) {
        case 'p':
            select_representatives_per_class(class_representatives);
            while (1) {
                printf("�ĺ��� ��: (150 ~ 250): ");
                scanf("%d", &num_candidates);
                if (num_candidates < 150 || num_candidates > 250) {
                    printf("�ٽ� �Է��ϼ���.\n");
                    continue;
                }
                break;
            }    
            assign_class_to_candidates(candidates, num_candidates);
            break;
        case 'v':
            printf("��ǥ�� ��ȣ(1~1000): ");
            scanf("%d", &num);
            printf("�ĺ��� %d���� ��ǥ��", vote[num-1]);
            break;
        case 'a':
            sort_candidates_upper(candidates, num_candidates);
            break;
        case 'd':
            sort_candidates_desc(candidates, num_candidates);
            break;
        case 'm':
            print_elected_per_class(candidates, num_candidates);
            break;
        case 'n':
            print_global_elected(candidates, num_candidates);
            break;
        case 'r':
            select_representatives_per_class(class_representatives);
            while (1) {
                printf("�ĺ��� ��: (150 ~ 250): ");
                scanf("%d", &num_candidates);
                if (num_candidates < 150 || num_candidates > 250) {
                    printf("�ٽ� �Է��ϼ���.\n");
                    continue;
                }
                break;
            }
            assign_class_to_candidates(candidates, num_candidates);
            break;
        case 'q':
            return 0;
        default:
            printf("�ٽ� �Է�.\n");
        }
    }

    return 0;
}


