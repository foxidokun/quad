#include <math.h>
#include <cfloat>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include "equation_solver.h"

static bool is_zero(double x);
static void set_if_not_null(double *ptr, double val);
static int read_double(double *x, const char *prompt);


enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2)
{
    assert(!isnan(a));
    assert(!isnan(b));
    assert(!isnan(c));

    // Проверка на валидность коэффициентов для дальнейших вычислений
    // Требования:
    // 1. b^2 - 4*a*c < DOUBLE_MAX для дискриминанта
    // 1.1 b^2 < DOUBLE_MAX
    // 1.2 4*a*c < DOUBLE_MAX
    // 2. b + abs(sq_disc) < DOUBLE_MAX -- всегда следует из предыдущих
    if (fabs(b) > sqrt(DBL_MAX) || //1.1
        (!is_zero(c) && fabs(a) > (DBL_MAX / fabs(c) / 4)) || //1.2
        pow(b, 2) > (DBL_MAX - 4 * a * c)) //1
    {
        return ERANGE_SOLVE;
    }

    // Уравнение является линейным
    if (is_zero(a)) {
        return solve_lin_eq(b, c, x1);
    } else {
        double disc = pow(b, 2) - 4 * a * c;

        if (is_zero(disc)) {
            set_if_not_null(x1, -b / a / 2);
            return ONE_ROOT;
        } else if (disc < 0) {
            return ZERO_ROOTS;
        } else {
            double sq_disc = sqrt(disc);
            set_if_not_null(x1, (-b + sq_disc) / a / 2);
            set_if_not_null(x2, (-b - sq_disc) / a / 2);
            return TWO_ROOTS;
        }
    }
}

enum num_roots solve_lin_eq(double k, double b, double *x)
{
    assert(!isnan(k));
    assert(!isnan(b));

    // k = 0 и решений либо нет, либо бесконечно много (при 0=0)
    if (is_zero(k)) {
        if (is_zero(b)) {
            return INF_ROOTS;
        } else {
            return ZERO_ROOTS;
        }
    } else {
        set_if_not_null(x, -b / k);
        return ONE_ROOT;
    }
}

void print_solution(enum num_roots n_roots, double x1, double x2)
{
    assert(n_roots < 3 && n_roots > -3);
    assert(!isnan(x1));
    assert(!isnan(x2));

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
        case ERANGE_SOLVE:
            printf("Не удалось решить уравнение: слишком большие коэффициенты\n");
            break;
        default:
            fprintf(stderr, "Некорректное количество корней\n");
            break;
    }
}

/**
 * Считывает аргумент из stdin, предварительно выводя prompt, и записывает в x. В случае некорректного ввода переспрашивает.
 * В случае ошибки возвращает ненулевое значение, соответствующее errno значению этой ошибки
 * @param x Куда записывается считанный double
 * @param prompt Приглашающая строка перед вводом
 * @return 0 или errno в случае ошибки
 */
static int read_double(double *x, const char *prompt)
{
    assert(x != NULL);
    assert(prompt != NULL);

    errno = 0;
    int scanf_res = 0;

    while (true) {
        printf("%s", prompt);
        scanf_res = scanf("%lf", x);

        // При слишком больших числах или при нечисловом вводе переспрашиваем
        if (errno == ERANGE || (errno == 0 && scanf_res == 0)) {
            while (getchar() != '\n'); //Сбрасываем неправильный ввод
            errno = 0; // Обнуляем возможную ошибку ERANGE
            printf("Неправильный ввод, пожалуйста, введите число, причем не слишком большое\n");
        } else {
            break;
        }
    }

    // scanf таки завершился с ошибкой, но ввода/вывода, а не преобразования
    if (errno != 0) {
        return errno;
    }

    // Ошибочные вводы были переспрошены, тут либо успех, либо EOF
    assert(scanf_res == 1 || scanf_res == EOF);

    if (scanf_res == 1) {
        return 0;
    } else {
        return EIO;
    }
}

int input_coeffs(double *a, double *b, double *c)
{
    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    double *coeffs[3] = {a, b, c};
    const char *prompts[3] = {"a = ", "b = ", "c = "};
    int read_res = 0;

    printf("Введите коэффициенты уравнения ax^2 + bx + c = 0\n");

    for (int i = 0; i < 3; i++) {
        read_res = read_double(coeffs[i], prompts[i]);

        // Если произошла ошибка, пробрасываем ее дальше
        if (read_res != 0) {
            return read_res;
        }
    }
    return 0;
}

/**
 * Сравнивает переданное double число с нулем с учетом погрешности double арифметики
 *
 * abs(x) < 10 * DBL_EPSILON
 */
static bool is_zero(double x)
{
    assert(!isnan(x));

    return fabs(x) < 10 * DBL_EPSILON;
}

/**
 * Записывает значение в данный указатель, если он не NULL. Иначе ничего не делает
 */
static void set_if_not_null(double *ptr, double val)
{
    assert(!isnan(val));

    if (ptr != NULL) {
        *ptr = val;
    }
}

//******************
// Тесты
//******************

int is_equal(double x, double y)
{
    return is_zero(x - y);
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
    double a = NAN, b = NAN, c = NAN;

    freopen("input.txt", "r", stdin);

    while (input_coeffs(&a, &b, &c) != EIO) {
        assert(is_equal(a, 5) && is_equal(b, 5) && is_equal(c, 5));
    }
}

void test_read_double()
{
    double x = NAN;
    freopen("input.txt", "r", stdin);

    while (read_double(&x, "") != EIO) {
        assert(is_equal(x, 5));
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

    fflush(stdout);

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
    test_read_double();
    test_output_format();
}