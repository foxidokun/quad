#include <cstdio>
#include <cstring>
#include <cmath>
#include "equation_solver.h"

int main(int argc, const char *argv[])
{
#ifdef TEST
    if (argc == 4) {
        run_test(argv[1], argv[2], argv[3], "/dev/null");
    } else if (argc == 5) {
        run_test(argv[1], argv[2], argv[3], argv[4]);
    } else {
        run_test("tmp.txt", "input.txt", "output.txt", "/dev/null");
    }

    return 0;
#endif

    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN;
    int input_res = 0;

    input_res = input_coeffs(&a, &b, &c, stdin, stdout);
    if (input_res != 0) {
        printf("Failed to read coefficients: %s", strerror(input_res));
        return -1;
    }

    enum num_roots n_roots = solve_quad_eq(a, b, c, &x1, &x2);

    if (n_roots == ERANGE_SOLVE) {
        printf("Failed to solve equation: Coefficients out of range");
        return -1;
    } else {
        print_solution(n_roots, x1, x2, stdout);
    }
    return 0;
}
