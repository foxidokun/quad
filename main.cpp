#include <cstdio>
#include <cstring>
#include "equation_solver.h"

int main()
{
#ifdef TEST
    run_test();
    return 0;
#endif

    double a = 0, b = 0, c = 0, x1 = 0, x2 = 0;
    int input_res = 0;

    input_res = input_coeffs(&a, &b, &c);
    if (input_res != 0) {
        printf("Не удалось считать коэффициенты: %s", strerror(input_res));
        return -1;
    }

    enum num_roots n_roots = solve_quad_eq(a, b, c, &x1, &x2);

    // Обрабатываем возможные ошибки
    if (n_roots == ERANGE_SOLVE) {
        printf("Не удалось решить уравнение: Слишком большие коэффициенты");
        return -1;
    } else {
        // В противном случае используем стандартный вывод решений
        print_solution(n_roots, x1, x2);
    }
    return 0;
}
