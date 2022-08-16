#include <cstdio>
#include <cassert>
#include <cerrno>
#include "equation_solver.h"

void input_coeffs(double *a, double *b, double *c);


int main()
{
    double a = 0, b = 0, c = 0, x1 = 0, x2 = 0;

    input_coeffs(&a, &b, &c);
    if (errno != 0) {
        perror("Не удалось считать коэффициенты");
        return -1;
    }

    enum num_roots n_roots = solve_quad_eq(a, b, c, &x1, &x2);

    // Обрабатываем возможные ошибки
    if (n_roots == ERR_SOLVE) {
        perror("Не удалось решить уравнение");
        return -1;
    } else {
        // В противном случае используем стандартный вывод решений
        print_solution(n_roots, x1, x2);
    }
    return 0;
}

/**
 * Считывает коэффициенты из stdin и записывает в переданные переменные. При неудаче переспрашивает
 * В случае ошибки считывания устанавливает errno != 0
 */
void input_coeffs(double *a, double *b, double *c)
{
    assert(a != nullptr && b != nullptr && c != nullptr);
    double *coeffs[3] = {a, b, c};
    const char *prompts[3] = {"a = ", "b = ", "c = "};
    errno = 0;
    int scan_res = 0;

    printf("Введите коэффициенты уравнения ax^2 + bx + c = 0\n");
    for (int i = 0; i < 3;) {
        printf("%s", prompts[i]);

        scan_res = scanf("%lf", coeffs[i]);
        if (errno != 0) {
            return;
        }
        switch (scan_res) {
            case 1: // Считалость правильно
                i++;
                break;
            case EOF: // Пользователь оборвал ввод
                errno = EIO;
                return;
            default:
                while (getchar() != '\n');
                printf("Неправильный ввод, пожалуйста, введите число\n");
                break;
        }
    }
}
