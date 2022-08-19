#include <cstdio>
#include <cstring>
#include <cmath>
#include "equation_solver.h"

int main(int argc, const char *argv[])
{
#ifdef TEST
    if (argc == 4) {
        run_test(argv[1], argv[2], argv[3]);
    } else {
        run_test("tmp.txt", "input.txt", "output.txt");
    }

    return 0;
#endif

    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN;
    int input_res = 0;

    input_res = input_coeffs(&a, &b, &c, stdin, stdout);
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
        print_solution(n_roots, x1, x2, stdout);
    }
    return 0;
}
