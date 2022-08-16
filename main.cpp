#include <cstdio>
#include "equation_solver.h"

int main()
{
    double a = 0, b = 0, c = 0, x1 = 0, x2 = 0;

    printf("Введите коэффициенты уравнения ax^2 + bx + c = 0\n");
    printf("a = ");
    scanf("%lf", &a);
    printf("b = ");
    scanf("%lf", &b);
    printf("c = ");
    scanf("%lf", &c);

    switch (solve_quad_eq(a, b, c, &x1, &x2)) {
        case 2:
            printf("Найдено 2 решения: %.3e и %.3e\n", x1, x2);
            break;
        case 1:
            printf("Найдено одно решение: %.3e\n", x1);
            break;
        case 0:
            printf("Решений не найдено\n");
            break;
        case -1:
        default:
            printf("Решений бесконечно много\n");
            break;
    }

    return 0;
}