#include <cstdio>
#include "equation_solver.h"

void input_coeffs(double *a, double *b, double *c);

int main()
{
    double a = 0, b = 0, c = 0, x1 = 0, x2 = 0;

    input_coeffs(&a, &b, &c);
    enum num_roots n_roots = solve_quad_eq(a, b, c, &x1, &x2);
    // Обрабатываем возможные ошибки
    if (n_roots == CANT_SOLVE) {
        perror("Не удалось решить уравнение по причине");
    }
    // Но все равно печатаем решения, чтобы пользователь получил два сообщения о неправильном вводе (stderr & stdout)
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

    printf("Введите коэффициенты уравнения ax^2 + bx + c = 0\n");
    for (int i = 0; i < 3;) {
        printf("%s", prompts[i]);
        if (scanf("%lf", coeffs[i]) != 1) {
            // Сбрасываем неправильный ввод
            while (getchar() != '\n');
            printf("Неправильный ввод, пожалуйста, введите число\n");
        } else {
            i++;
        }
    }
}
