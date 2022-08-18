#include <cmath>
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
            fprintf(stderr, "Не удалось решить уравнение: слишком большие коэффициенты\n");
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
    assert(x != NULL && prompt != NULL && "Указатели не должны быть null");

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

    assert(scanf_res == 1 || scanf_res == EOF && "Ошибочные вводы были переспрошены, тут либо успех либо EOF");

    if (scanf_res == 1) {
        return 0;
    } else {
        return EIO;
    }
}

int input_coeffs(double *a, double *b, double *c)
{
    assert(a != NULL && b != NULL && c != NULL && "Коэффициенты не должны быть null");

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
 */
static bool is_zero(double x)
{
    return fabs(x) < 10 * DBL_EPSILON;
}

/**
 * Записывает значение в данный указатель, если он не NULL. Иначе ничего не делает
 */
static void set_if_not_null(double *ptr, double val)
{
    if (ptr != NULL) {
        *ptr = val;
    }
}

#ifdef TEST
#include "test_equation_solver.cpp"
#endif