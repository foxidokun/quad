#include <math.h>
#include <cfloat>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include <time.h>
#include "equation_solver.h"

/// Такое значение, что 0+DBL_ERROR все еще 0
/// Не стандартный DBL_EPSILON, так как он слишком маленький
const double DBL_ERROR = 1e-5;

static bool is_zero(double x);
static int read_double(double *x, const char *prompt);
static void flush_stdin();
static int is_equal(double x, double y);
static double rand_range(double min, double max);


enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2)
{
    assert(isfinite(a) && "parameter must be finite");
    assert(isfinite(b) && "parameter must be finite");
    assert(isfinite(c) && "parameter must be finite");
    assert(x1 != NULL && "pointer can't be null");
    assert(x2 != NULL && "pointer can't be null");

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
            *x1 = -b / a / 2;
            return ONE_ROOT;
        } else if (disc < 0) {
            return ZERO_ROOTS;
        } else {
            double sq_disc = sqrt(disc);
            *x1 = (-b + sq_disc) / a / 2;
            *x2 = (-b - sq_disc) / a / 2;
            return TWO_ROOTS;
        }
    }
}

enum num_roots solve_lin_eq(double k, double b, double *x)
{
    assert(isfinite(k) && "parameter must be finite");
    assert(isfinite(b) && "parameter must be finite");
    assert(x != NULL && "pointer can't be null");

    // k = 0 и решений либо нет, либо бесконечно много (при 0=0)
    if (is_zero(k)) {
        if (is_zero(b)) {
            return INF_ROOTS;
        } else {
            return ZERO_ROOTS;
        }
    } else {
        *x = -b / k;
        return ONE_ROOT;
    }
}

void print_solution(enum num_roots n_roots, double x1, double x2)
{
    assert(isfinite(x1) && "parameter must be finite");
    assert(isfinite(x2) && "parameter must be finite");

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
 * Сбрасывает ввод до символа \n
 */
static void flush_stdin()
{
    while (getchar() != '\n') {}
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
    assert(x != NULL && "pointer can't be null");
    assert(prompt != NULL && "pointer can't be null");

    errno = 0;
    int scanf_res = 0;

    while (true) {
        printf("%s", prompt);
        scanf_res = scanf("%lf", x);

        // При слишком больших числах или при нечисловом вводе переспрашиваем
        if (errno == ERANGE || (errno == 0 && scanf_res == 0)) {
            flush_stdin(); //Сбрасываем неправильный ввод
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
    assert(a != NULL && "pointer can't be null");
    assert(b != NULL && "pointer can't be null");
    assert(c != NULL && "pointer can't be null");

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
 * abs(x) < DBL_EPSILON
 */
static bool is_zero(double x)
{
    assert(isfinite(x) && "parameter must be finite");

    return fabs(x) < DBL_ERROR;
}

/**
 * Сравнивает x == y с учетом double погрешности
 */
static int is_equal(double x, double y)
{
    return is_zero(x - y);
}

/**
 * Генерирует псевдослучайное число в диапазоне [min, max]
 */
static double rand_range(double min, double max)
{
    double range = (max - min);
    return min + ((double) rand() / RAND_MAX) * range;
}

//******************
// Тесты
//******************

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

void test_input_coeffs(char *input_file)
{
    double a = NAN, b = NAN, c = NAN;

    freopen(input_file, "r", stdin);

    while (input_coeffs(&a, &b, &c) != EIO) {
        assert(is_equal(a, 5) && is_equal(b, 5) && is_equal(c, 5));
    }
}

void test_read_double(char *input_file)
{
    double x = NAN;
    freopen(input_file, "r", stdin);

    while (read_double(&x, "") != EIO) {
        assert(is_equal(x, 5));
    }
}

void test_output_format(char *tmp_file, char *output_ref_file)
{
    freopen(tmp_file, "w", stdout);

    print_solution(ERANGE_SOLVE, 228, 282);
    print_solution(INF_ROOTS, 228, 282);
    print_solution(ZERO_ROOTS, 228, 282);
    print_solution(ONE_ROOT, 228, 282);
    print_solution(TWO_ROOTS, 228, 282);

    fflush(stdout);

    //Сравниваем с эталоном
    FILE *test_output = fopen(tmp_file, "r");
    FILE *ref_output = fopen(output_ref_file, "r");
    int c = 0;

    while ((c = getc(test_output)) != EOF) {
        assert(c == getc(ref_output));
    }
}

void auto_test_solve_lin_eq()
{
    double k = NAN, b = NAN, x = NAN;
    const int num_test = 100; // Количество прогоняемых тестов
    srand(time(NULL));

    for (int i = 0; i < num_test; ++i) {
        k = rand_range(-100, 100);
        b = rand_range(-100, +100);

        if (solve_lin_eq(k, b, &x) == ONE_ROOT) {
            assert(is_zero(k * x + b));
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
                assert(is_zero(a * x2 * x2 + b * x2 + c));
                // И далее проверяем x1 вместе с ONE_ROOT веткой
            case ONE_ROOT:
                assert(is_zero(a * x1 * x1 + b * x1 + c));
                break;
                // В данном случае бесконечность корней == подходит любой
            case INF_ROOTS:
                x1 = rand_range(-100, +100);
                assert(is_zero(a * x1 * x1 + b * x1 + c));
                break;
                // Если корней нет, значит никакой не подходит
            case ZERO_ROOTS:
                x1 = rand_range(-100, +100);
                assert(!is_zero(a * x1 * x1 + b * x1 + c));
                break;
            case ERANGE_SOLVE:
                assert(0 && "Test parameters can't be out of range");
                break;
            default:
                assert(0 && "Not enum member");
        }
    }
}

void auto_test_input_coeffs(char *tmp_file)
{
    assert(tmp_file != NULL && "pointer can't be null");

    FILE *pipe = fopen(tmp_file, "w");
    double a_inp = NAN, b_inp = NAN, c_inp = NAN; // Значения, которые считает программа
    double a_ref = NAN, b_ref = NAN, c_ref = NAN; // Значения, которые она должна принять
    const int num_test = 100;
    srand(time(NULL));
    freopen(tmp_file, "r", stdin);

    for (int i = 0; i < num_test; ++i) {
        a_ref = rand_range(-100, 100);
        b_ref = rand_range(-100, 100);
        c_ref = rand_range(-100, 100);

        fprintf(pipe, "%.18lf\n9e999\n%.18lf\nasd\ndadsfa\n%.18lf\n", a_ref, b_ref, c_ref);
        fflush(pipe);

        assert(input_coeffs(&a_inp, &b_inp, &c_inp) == 0);
        assert(is_equal(a_inp, a_ref));
        assert(is_equal(b_inp, b_ref));
        assert(is_equal(c_inp, c_ref));
    }
}

void run_test(char *tmp_file, char *input_file, char *output_ref_file)
{
    test_solve_lin_eq();
    test_solve_quad_eq();
    test_read_double(input_file);
    test_input_coeffs(input_file);
    test_output_format(tmp_file, output_ref_file);

    auto_test_solve_lin_eq();
    auto_test_solve_quad_eq();
    auto_test_input_coeffs(tmp_file);
}