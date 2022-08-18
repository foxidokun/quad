#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include "equation_solver.h"

int is_equal(double x, double y)
{
    return (x - y) < 10 * DBL_EPSILON;
}

int is_equal_set(double x1, double x2, double y1, double y2)
{
    return (is_equal(x1, y1) && is_equal(x2, y2)) || (is_equal(x1, y2) && is_equal(x2, y1));
}

void test_solve_lin_eq()
{
    double x = 0;

    assert(solve_lin_eq(0, 0, &x) == INF_ROOTS);

    // Верно сравнивает с нулем
    assert(solve_lin_eq(2e-16, 0, &x) == INF_ROOTS);
    assert(solve_lin_eq(2e-16, 2e-16, &x) == INF_ROOTS);

    assert(solve_lin_eq(0, 1, &x) == ZERO_ROOTS);

    // Верно считает корни
    assert(solve_lin_eq(1, -1, &x) == ONE_ROOT && is_equal(x, 1));
    assert(solve_lin_eq(-5, 5, &x) == ONE_ROOT && is_equal(x, 1));
    assert(solve_lin_eq(0.05, 0.1, &x) == ONE_ROOT && is_equal(x, -2));
    assert(solve_lin_eq(-0.04, -0.06, &x) == ONE_ROOT && is_equal(x, -1.5));
}

void test_solve_quad_eq()
{
    double x1 = 0, x2 = 0;

    // Верно выставлены ограничения на параметры
    assert(solve_quad_eq(0, sqrt(DBL_MAX) * 1.01, 0, NULL, NULL) == ERANGE_SOLVE);
    assert(solve_quad_eq(DBL_MAX / 7.5, 0, DBL_MAX / 7.5, NULL, NULL) == ERANGE_SOLVE);
    assert(solve_quad_eq(-DBL_MAX / 15.5, sqrt(DBL_MAX / 2), DBL_MAX / 15.5, NULL, NULL) == ERANGE_SOLVE);

    // Верно пробрасывает в линейное уравнение
    assert(solve_quad_eq(0, 7, 7, NULL, NULL) == ONE_ROOT);
    assert(solve_quad_eq(2e-16, 7, 7, NULL, NULL) == ONE_ROOT);

    // Верно считает корни
    assert(solve_quad_eq(1, 0, -4, &x1, &x2) == TWO_ROOTS && is_equal_set(x1, x2, -2, +2));
    assert(solve_quad_eq(-1, 0, 4, &x1, &x2) == TWO_ROOTS && is_equal_set(x1, x2, -2, +2));
    assert(solve_quad_eq(6, -1, -2, &x1, &x2) == TWO_ROOTS && is_equal_set(x1, x2, -0.5, 2.0 / 3.0));
    assert(solve_quad_eq(-6, 1, 2, &x1, &x2) == TWO_ROOTS && is_equal_set(x1, x2, -0.5, 2.0 / 3.0));
    assert(solve_quad_eq(5, 0, 0, &x1, &x2) == ONE_ROOT && is_equal(x1, 0));
    assert(solve_quad_eq(-5, 0, 0, &x1, &x2) == ONE_ROOT && is_equal(x1, 0));
    assert(solve_quad_eq(0.5, -1, 0, &x1, &x2) == TWO_ROOTS && is_equal_set(x1, x2, 0, 2));
    assert(solve_quad_eq(-0.5, 1, 0, &x1, &x2) == TWO_ROOTS && is_equal_set(x1, x2, 0, 2));
}

void test_input_coeffs()
{
    double a = 0, b = 0, c = 0;

    freopen("input.txt", "r", stdin);

    while (input_coeffs(&a, &b, &c) != EIO) {
        assert(is_equal(a, 5) && is_equal(b, 5) && is_equal(c, 5));
    }
}

void test_output_format()
{
    freopen("output_test.txt", "w", stdout);

    print_solution(ERANGE_SOLVE, 228, 282);
    print_solution(INF_ROOTS, 228, 282);
    print_solution(ZERO_ROOTS, 228, 282);
    print_solution(ONE_ROOT, 228, 282);
    print_solution(TWO_ROOTS, 228, 282);

    //Сравниваем с эталоном
    FILE *test_output = fopen("output_test.txt", "r");
    FILE *ref_output = fopen("output.txt", "r");
    int c = 0;

    while ((c = getc(test_output)) != EOF) {
        assert(c == getc(ref_output));
    }
}

void test_is_zero()
{
    assert(is_zero(2e-16));
    assert(is_zero(-2e-16));
    assert(!is_zero(-0.02));
    assert(!is_zero(0.02));
}

void test_set_if_not_null()
{
    double val = 0;

    set_if_not_null(&val, 7);
    assert(is_equal(val, 7));

    set_if_not_null(NULL, 14);
    assert(is_equal(val, 7));
}

void run_test()
{
    test_solve_lin_eq();
    test_solve_quad_eq();
    test_input_coeffs();
    test_is_zero();
    test_set_if_not_null();
    test_input_coeffs();
    test_output_format();
}