#include <cmath>
#include <cfloat>
#include <cstdio>
#include "equation_solver.h"

static bool is_zero(double x);

/**
 * Решает квадратное уравнение вида a*x^2 + b*x + c = 0, записывает решения (при наличии) в переданные переменные (x1 и x2)
 * и возвращает их количество в объекте enum num_roots \n\n
 *
 * Если решений меньше двух, неиспользованные переменные не изменяют своего значения.
 * При наличии одного решения оно записывается в x1.
 * При наличии двух решений порядок x1 и x2 не гарантируется.
 * @param a Коэффициент при квадратичном члене
 * @param b Коэффициент при линейном члене
 * @param c Свободный коэффициент
 * @param x1 Указатель на переменную для первого корня (при наличии)
 * @param x2 Указатель на переменную для второго корня (при наличии)
 * @return Количество найденных корней
 */
enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2)
{
    // Уравнение является линейным
    if (is_zero(a)) {
        return solve_lin_eq(b, c, x1);
    } else {
        double disc = pow(b, 2) - 4 * a * c;

        if (is_zero(disc)) {
            *x1 = -b / (2 * a);
            return ONE_ROOT;
        } else if (disc < 0) {
            return ZERO_ROOTS;
        } else {
            double sq_disc = sqrt(disc);
            *x1 = (-b + sq_disc) / (2 * a);
            *x2 = (-b - sq_disc) / (2 * a);
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
        *x = -b / k;
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
        default:
            fprintf(stderr, "Некорректное количество корней");
            break;
    }
}

/**
 * Сравнивает переданное double число с учетом погрешности double арифметики
 */
static bool is_zero(double x)
{
    return fabs(x) < 10 * DBL_EPSILON;
}