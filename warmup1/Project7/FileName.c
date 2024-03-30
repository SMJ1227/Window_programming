#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define PLANETS 8

typedef struct {
    char name[10];      // �¾�� �༺ �̸�
    double r;              // ũ��(������)
    double satellite;      // ������ ����
    double orbital;        // ���� �ֱ�
    double rotation;       // ���� �ֱ�
    double orbital_speed;  // ���� �ӵ�
    double tilt;           // ����
    double distance;       // �¾���� �Ÿ�
} Planet;

typedef struct {
    Planet planet[PLANETS];
    Planet copy[PLANETS];
} SS;

void copy(SS* ss) {
    for (int i = 0; i < PLANETS; i++) {
        ss->copy[i] = ss->planet[i];
    }
}

void init(SS* ss) {
    strcpy(ss->planet[0].name, "mercury");
    ss->planet[0].r = 3.8;
    ss->planet[0].satellite = 0;
    ss->planet[0].orbital = 88;
    ss->planet[0].rotation = 58.65;
    ss->planet[0].orbital_speed = 47.36;
    ss->planet[0].tilt = 2;
    ss->planet[0].distance = 0.39;

    strcpy(ss->planet[1].name, "venus");
    ss->planet[1].r = 9.5;
    ss->planet[1].satellite = 0;
    ss->planet[1].orbital = 226;
    ss->planet[1].rotation = 243;
    ss->planet[1].orbital_speed = 35.020;
    ss->planet[1].tilt = -177.3;
    ss->planet[1].distance = 0.72;

    strcpy(ss->planet[2].name, "earth");
    ss->planet[2].r = 10;   
    ss->planet[2].satellite = 1;
    ss->planet[2].orbital = 365;
    ss->planet[2].rotation = 1;
    ss->planet[2].orbital_speed = 29.76;
    ss->planet[2].tilt = 23.5;
    ss->planet[2].distance = 1;

    strcpy(ss->planet[3].name, "mars");
    ss->planet[3].r = 5.3;
    ss->planet[3].satellite = 2;
    ss->planet[3].orbital = 686;
    ss->planet[3].rotation = 1.02;
    ss->planet[3].orbital_speed = 24.077;
    ss->planet[3].tilt = 25.2;
    ss->planet[3].distance = 1.52;

    strcpy(ss->planet[4].name, "jupitor");
    ss->planet[4].r = 112.1;
    ss->planet[4].satellite = 63;
    ss->planet[4].orbital = 4343.5;
    ss->planet[4].rotation = 0.4;
    ss->planet[4].orbital_speed = 13.05624;
    ss->planet[4].tilt = 3.1;
    ss->planet[4].distance = 5.19;

    strcpy(ss->planet[5].name, "saturn");
    ss->planet[5].r = 94.5;
    ss->planet[5].satellite = 56;
    ss->planet[5].orbital = 10731;
    ss->planet[5].rotation = 0.427;
    ss->planet[5].orbital_speed = 9.639;
    ss->planet[5].tilt = 26.7;
    ss->planet[5].distance = 8.87;

    strcpy(ss->planet[6].name, "uranus");
    ss->planet[6].r = 40.1;
    ss->planet[6].satellite = 27;
    ss->planet[6].orbital = 30587;
    ss->planet[6].rotation = 0.71875;
    ss->planet[6].orbital_speed = 6.795;
    ss->planet[6].tilt = -97.9;
    ss->planet[6].distance = 19.13;

    strcpy(ss->planet[7].name, "neptune");
    ss->planet[7].r = 38.8;
    ss->planet[7].satellite = 13;
    ss->planet[7].orbital = 59788;
    ss->planet[7].rotation = 0.67;
    ss->planet[7].orbital_speed = 5.43;
    ss->planet[7].tilt = 29.6;
    ss->planet[7].distance = 30;
}

void print_all(SS* ss) {
    printf("Planet\t\tRadius\tSatellites\tOrbital\t\tRotation\tOrbital Speed\tTilt\tDistance from Sun\n");
    for (int i = 0; i < PLANETS; ++i) {
        printf("%-10s\t%.1f\t%.0f\t\t%.1f\t\t%.2f\t\t%.2f\t\t%.1f\t%.2f\n",
            ss->planet[i].name, ss->planet[i].r, ss->planet[i].satellite,
            ss->planet[i].orbital, ss->planet[i].rotation,
            ss->planet[i].orbital_speed, ss->planet[i].tilt,
            ss->planet[i].distance);
    }
}

void swap(Planet* a, Planet* b) {
    Planet temp = *a;
    *a = *b;
    *b = temp;
}

void sort_orbital_speed(SS* ss) {
    for (int i = 0; i < PLANETS - 1; i++) {
        for (int j = 0; j < PLANETS - 1 - i; j++) {
            if (ss->copy[j].orbital_speed > ss->copy[j + 1].orbital_speed) {
                swap(&ss->copy[j], &ss->copy[j + 1]);
            }
        }
    }
    printf("���� �ӵ��� ���� �������� ����\n");
    for (int i = 0; i < PLANETS; i++) {
        printf("%s\t%.2lf\n", ss->copy[i].name, ss->copy[i].orbital_speed);
    }
    printf("\n");
}

void sort_r_lower(SS* ss) {
    for (int i = 0; i < PLANETS - 1; i++) {
        for (int j = 0; j < PLANETS - 1 - i; j++) {
            if (ss->copy[j].r < ss->copy[j + 1].r) {
                swap(&ss->copy[j], &ss->copy[j + 1]);
            }
        }
    }
    printf("�༺�� ũ�⿡ ���� �������� ����\n");
    for (int i = 0; i < PLANETS; i++) {
        printf("%s\t%.2lf\n", ss->copy[i].name, ss->copy[i].r);
    }
    printf("\n");
}

void sort_distance_lower(SS* ss) {
    double planet_distance[PLANETS - 1];
    char* distance_name[PLANETS - 1];
    int j = 0;

    for (int i = 0; i < 7; i++) {
        planet_distance[i] = fabs(ss->copy[j].distance - ss->copy[j + 1].distance);
        distance_name[i] = malloc(sizeof(char) * 20);
        strcpy(distance_name[i], ss->copy[j].name);
        strcat(distance_name[i], " - ");
        strcat(distance_name[i], ss->copy[j + 1].name);
        j++;
    }
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6 - i; j++) {
            if (planet_distance[j] < planet_distance[j + 1]) {
                double temp = planet_distance[j];
                planet_distance[j] = planet_distance[j + 1];
                planet_distance[j + 1] = temp;

                char* temp_name = distance_name[j];
                distance_name[j] = distance_name[j + 1];
                distance_name[j + 1] = temp_name;
            }
        }
    }
    printf("�༺�� �Ÿ��� ���� �������� ����\n");
    for (int i = 0; i < PLANETS-1; i++) {
        printf("[%s]\t\t%.2lf\n", distance_name[i], planet_distance[i]);
    }
    printf("\n");
    for (int i = 0; i < PLANETS - 1; i++) {
        free(distance_name[i]);
    }
}

void change_day_to_hour(SS* ss) {
    for (int i = 0; i < PLANETS; i++) {
        printf("%-10s\t%-.2lf hour\n", ss->planet[i].name, ss->planet[i].rotation * 24);
    }
}

void sort_tilt_upper(SS* ss) {
    for (int i = 0; i < PLANETS - 1; i++) {
        for (int j = 0; j < PLANETS - 1 - i; j++) {
            if (ss->copy[j].tilt > ss->copy[j + 1].tilt) {
                swap(&ss->copy[j], &ss->copy[j + 1]);
            }
        }
    }
    printf("�����࿡ ���� �������� ����\n");
    for (int i = 0; i < PLANETS; i++) {
        printf("%s\t%.2lf��\n", ss->copy[i].name, ss->copy[i].tilt);
    }
    printf("\n");
}

int main(void) {
    SS ss;
    init(&ss);
    char command;

    while (1) {
        printf("��ɾ �Է��ϼ��� (p, a, b, c, d, e, q): ");
        scanf(" %c", &command);
        copy(&ss);
        switch (command) {
        case 'p':
            copy(&ss);
            print_all(&ss);
            break;
        case 'a':   // ���� �ӵ��� ���� ������������ ����Ѵ�
            sort_orbital_speed(&ss);
            break;            
        case 'b':   // �༺�� ũ�⿡ ���� ������������ ����Ѵ�.
            sort_r_lower(&ss);
            break;
        case 'c':   // �� �༺���� �Ÿ��� ���Ͽ� ������������ ����Ѵ�.
            copy(&ss);
            sort_distance_lower(&ss);
            break;
        case 'd':   // ��� �༺�� ���� �ֱ⸦ �ð����� �ٲ㼭 ����Ѵ�.
            change_day_to_hour(&ss);
            break;
        case 'e':   // ���⸦ ������������ ����Ѵ�
            sort_tilt_upper(&ss);
            break;
        case 'q':
            printf("���α׷��� �����մϴ�.\n");
            return 0;
        default:
            printf("�߸��� ��ɾ��Դϴ�. �ٽ� �Է��ϼ���.\n");
            continue;
        }
    }
    return 0;
}



