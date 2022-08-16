#include <cstdio>
#include "equation_solver.h"

void input_coeffs(double *a, double *b, double *c);
void print_solution(enum num_roots n_roots, double x1, double x2);

int main()
{
    double a = 0, b = 0, c = 0, x1 = 0, x2 = 0;

    input_coeffs(&a, &b, &c);
    enum num_roots n_roots = solve_quad_eq(a, b, c, &x1, &x2);
    print_solution(n_roots, x1, x2);

    return 0;
}

/**
 * Считывает коэффициенты из stdin и записывает в переданные переменные. При неудаче переспрашивает
 */
void input_coeffs(double *a, double *b, double *c)
{
    double *coeffs[3] = {a, b, c};
    const char *prompts[3] = {"a = ", "b = ", "c = "};
    const unsigned int buf_size = 256;
    char input_buf[buf_size] = {}; //Буфер, куда отправляется неправильный ввод (/dev/null)

    printf("Введите коэффициенты уравнения ax^2 + bx + c = 0\n");
    for (int i = 0; i < 3;) {
        printf("%s", prompts[i]);
        if (scanf("%lf", coeffs[i]) != 1) {
            // Сбрасываем неправильный ввод
            fgets(input_buf, buf_size, stdin);
            printf("Неправильный ввод, пожалуйста, введите число\n");
        } else {
            i++;
        }
    }
}

/**
 * Выводит количество корней и сами корни.
 */
void print_solution(enum num_roots n_roots, double x1, double x2)
{
    switch (n_roots) {
        case TWO_ROOTS:
            printf("Найдено 2 решения: %.3e и %.3e\n", x1, x2);
            break;
        case ONE_ROOT:
            printf("Найдено одно решение: %.3e\n", x1);
            break;
        case ZERO_ROOTS:
            printf("Решений не найдено\n");
            break;
        case INF_ROOTS:
            printf("Решений бесконечно много\n");
            break;
        default:
            fprintf(stderr, "Некорректное количество корней");
            break;
    }
}