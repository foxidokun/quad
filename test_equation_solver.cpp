#include <time.h>
#include <cassert>
#include <cfloat>
#include <math.h>
#include <cstdio>
#include <errno.h>
#include "equation_solver.h"

static double rand_range(double min, double max);
static int is_equal(double x, double y);


void test_solve_lin_eq()
{
    double x = 0;

    assert(solve_lin_eq(0, 0, &x) == INF_ROOTS);

    // Верно сравнивает с нулем
    assert(solve_lin_eq(2e-16, 0, &x) == INF_ROOTS);
    assert(solve_lin_eq(2e-16, 2e-16, &x) == INF_ROOTS);
}

void test_solve_quad_eq()
{
    double x1 = 0, x2 = 0;

    // Верно выставлены ограничения на параметры
    assert(solve_quad_eq(0, sqrt(DBL_MAX) * 1.01, 0, &x1, &x2) == ERANGE_SOLVE);
    assert(solve_quad_eq(DBL_MAX / 7.5, 0, DBL_MAX / 7.5, &x1, &x2) == ERANGE_SOLVE);
    assert(solve_quad_eq(-DBL_MAX / 15.5, sqrt(DBL_MAX / 2), DBL_MAX / 15.5, &x1, &x2) == ERANGE_SOLVE);
}

void test_input_coeffs(FILE *in_stream, FILE *dev_null)
{
    double a = NAN, b = NAN, c = NAN;

    while (input_coeffs(&a, &b, &c, in_stream, dev_null) != EIO) {
        assert(is_equal(a, 5));
        assert(is_equal(b, 5));
        assert(is_equal(c, 5));
    }
}

void test_output_format(char *tmp_file, FILE *ref_stream)
{
    FILE *write_stream = fopen(tmp_file, "w");

    print_solution(ERANGE_SOLVE, 228, 282, write_stream);
    print_solution(INF_ROOTS, 228, 282, write_stream);
    print_solution(ZERO_ROOTS, 228, 282, write_stream);
    print_solution(ONE_ROOT, 228, 282, write_stream);
    print_solution(TWO_ROOTS, 228, 282, write_stream);
    fclose(write_stream);

    //Сравниваем с эталоном
    int c = 0;
    FILE *read_stream = fopen(tmp_file, "r");

    while ((c = getc(read_stream)) != EOF) {
        assert(c == getc(ref_stream));
    }

    fclose(read_stream);
}

void auto_test_solve_lin_eq()
{
    double k = NAN, b = NAN, x = NAN;
    const int num_test = 100; // Количество прогоняемых тестов
    srand(time(NULL));

    for (int i = 0; i < num_test; ++i) {
        k = rand_range(-100, 100);
        b = rand_range(-100, +100);

        switch(solve_lin_eq(k, b, &x)) {
            case ONE_ROOT:
                assert(is_zero(k * x + b));
                break;
            case ZERO_ROOTS: // Никакой не подойдет
                x = rand_range(-100, +100);
                assert(!is_zero(k * x + b));
                break;
            case INF_ROOTS: // Подойдет любой
                x = rand_range(-100, +100);
                assert(is_zero(k * x + b));
                break;
            case ERANGE_SOLVE:
                break;
            case TWO_ROOTS:
                assert(0 && "impossible number of roots");
                break;
            default:
                assert(0 && "Not a enum member");
                break;
        }
    }
}

void auto_test_solve_quad_eq()
{
    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN;
    const int num_test = 100;
    srand(time(NULL));

    for (int i = 0; i < num_test; ++i) {
        a = rand_range(-100, +100);
        b = rand_range(-100, +100);
        c = rand_range(-100, +100);

        switch (solve_quad_eq(a, b, c, &x1, &x2)) {
            case TWO_ROOTS:
                assert(is_zero(a*x2*x2 + b*x2 + c));
                // И далее проверяем x1 вместе с ONE_ROOT веткой
            case ONE_ROOT:
                assert(is_zero(a*x1*x1 + b*x1 + c));
                break;
                // В данном случае бесконечность корней == подходит любой
            case INF_ROOTS:
                x1 = rand_range(-100, +100);
                assert(is_zero(a*x1*x1 + b*x1 + c));
                break;
                // Если корней нет, значит никакой не подходит
            case ZERO_ROOTS:
                x1 = rand_range(-100, +100);
                assert(!is_zero(a*x1*x1 + b*x1 + c));
                break;
            case ERANGE_SOLVE:
                assert(0 && "Test parameters can't be out of range");
                break;
            default:
                assert(0 && "Invalid enum member");
        }
    }
}

void auto_test_input_coeffs(char *tmp_file, FILE *dev_null)
{
    FILE *read_s = fopen(tmp_file, "r");
    FILE *write_s = fopen(tmp_file, "w");
    double a_inp = NAN, b_inp = NAN, c_inp = NAN; // Значения, которые считает программа
    double a_ref = NAN, b_ref = NAN, c_ref = NAN; // Значения, которые она должна принять
    const int num_test = 100;
    srand(time(NULL));

    for (int i = 0; i < num_test; ++i) {
        a_ref = rand_range(-100, 100);
        b_ref = rand_range(-100, 100);
        c_ref = rand_range(-100, 100);

        fprintf(write_s, "%.18lf\n9e999\n%.18lf\nasd\ndadsfa\n%.18lf\n", a_ref, b_ref, c_ref);
        fflush(write_s);

        assert(input_coeffs(&a_inp, &b_inp, &c_inp, read_s, dev_null) == 0);
        assert(is_equal(a_inp, a_ref));
        assert(is_equal(b_inp, b_ref));
        assert(is_equal(c_inp, c_ref));
    }

    fclose(read_s);
    fclose(write_s);
}

void run_test(char *tmp_file, char *input_file, char *output_ref_file)
{
    FILE *in_stream = fopen(input_file, "r");
    FILE *ref_stream = fopen(output_ref_file, "r");
    FILE *dev_null = fopen("/dev/null", "w");

    test_solve_lin_eq();
    test_solve_quad_eq();
    fseek(in_stream, 0, SEEK_SET);
    test_input_coeffs(in_stream, dev_null);
    test_output_format(tmp_file, ref_stream);

    auto_test_solve_lin_eq();
    auto_test_solve_quad_eq();
    auto_test_input_coeffs(tmp_file, dev_null);
}

/**
 * Генерирует псевдослучайное число в диапазоне [min, max]
 */
static double rand_range(double min, double max)
{
    double range = (max - min);
    return min + ((double) rand() / RAND_MAX) * range;
}

/**
 * Сравнивает x == y с учетом double погрешности
 */
static int is_equal(double x, double y)
{
    return is_zero(x - y);
}