#include <cstdio>
#include <cstring>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "equation_solver.h"

#ifdef TEST
#include "test_equation_solver.h"
#endif

int main(int argc, const char *argv[])
{
    srand(time(NULL));

#ifdef TEST

    if (argc == 4)      run_test(argv[1], argv[2], argv[3], "/dev/null");
    else if (argc == 5) run_test(argv[1], argv[2], argv[3], argv[4]);
    else                run_test("tmp.txt", "input.txt", "output.txt", "/dev/null");

    return 0;

#endif

    double coeffs[] = {NAN, NAN, NAN};
    double roots[] = {NAN, NAN};
    int input_res = 0;

    input_res = input_coeffs(3, coeffs, stdin, stdout);
    
    if (input_res != 0) {
        printf("Failed to read coefficients: %s", strerror(input_res));
        return -1;
    }

    num_roots n_roots = solve_quad_eq(coeffs[2], coeffs[1], coeffs[0], &roots[0], &roots[1]);

    if (n_roots == ERANGE_SOLVE)
    {
        printf("Failed to solve equation: Coefficients out of range");
        return -1;
    }
    else
    {
        print_solution(n_roots, roots, stdout);
    }

    return 0;
}
