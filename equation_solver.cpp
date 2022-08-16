#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cerrno>
#include "equation_solver.h"

static bool is_zero(double x);
static void set_if_not_null(double *ptr, double val);

/**
 * Решает квадратное уравнение вида a*x^2 + b*x + c = 0, записывает решения (при наличии) в переданные переменные (x1 и x2),
 * если они не nullptr, и возвращает их количество в объекте enum num_roots. В случае ошибки вычисления устанавливает
 * ненулевой код ошибки в errno \n\n
 *
 * Возможное ошибки\n
 * -> ERANGE -- при переполнении double во внутренних вычислениях\n\n
 *
 * Если решений меньше двух, неиспользованные переменные не изменяют своего значения.
 * При наличии одного решения оно записывается в x1.
 * При наличии двух решений порядок x1 и x2 не гарантируется.
 * @param a Коэффициент при квадратичном члене
 * @param b Коэффициент при линейном члене
 * @param c Свободный коэффициент
 * @param x1 Указатель на переменную для первого корня (при наличии) или nullptr
 * @param x2 Указатель на переменную для второго корня (при наличии) или nullptr
 * @return Количество найденных корней
 */
enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2)
{
    // Проверка на валидность коэффициентов для дальнейших вычислений
    // Требования:
    // 1. b^2 - 4*a*c < DOUBLE_MAX для дискриминанта
    // 1.1 b^2 < DOUBLE_MAX
    // 1.2 4*a*c < DOUBLE_MAX
    // 2. b + abs(sq_disc) < DOUBLE_MAX -- всегда следует из предыдущих
    if (fabs(b) > sqrt(DBL_MAX)  || //1.1
        (!is_zero(c) && fabs(a) > (DBL_MAX / c / 4)) || //1.2
        pow(b, 2) > (DBL_MAX - 4 * a * c)) //1
    {
        errno = ERANGE;
        return ERR_SOLVE;
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

/**
 * Решает линейное уравнение вида kx + b = 0, записывает решение (при наличии) в переданную переменную (x)
 * и возвращает количество найденных решений в объекте enum num_roots
 * @param k Коэффициент при линейном члене
 * @param b Свободный коэффициент
 * @param x Указатель на переменную для корня (при наличии)
 * @return Количество найденных корней
 */
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

/**
 * Выводит количество корней и сами корни.
 */
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
        case ERR_SOLVE:
            fprintf(stderr, "Произошла ошибка при решении уравнения\n");
            break;
        default:
            fprintf(stderr, "Некорректное количество корней\n");
            break;
    }
}

/**
 * Сравнивает переданное double число с нулем с учетом погрешности double арифметики
 */
static bool is_zero(double x)
{
    return fabs(x) < 10 * DBL_EPSILON;
}

/**
 * Записывает значение в данный указатель, если он не nullptr. Иначе ничего не делает
 */
static void set_if_not_null(double *ptr, double val)
{
    if (ptr != nullptr) {
        *ptr = val;
    }
}