#ifndef QUAD_EQUATION_SOLVER_H
#define QUAD_EQUATION_SOLVER_H

#ifdef TEST
/// Запескает все доступные юнит тесты
void run_test();
int is_equal(double x, double y);
int is_equal_set(double x1, double x2, double y1, double y2);
void test_solve_lin_eq();
void test_solve_quad_eq();
void test_input_coeffs();
void test_is_zero();
void test_set_if_not_null();
void test_output_format();
#endif

/// Количество корней у уравнения
enum num_roots {
    TWO_ROOTS    =  2,
    ONE_ROOT     =  1,
    ZERO_ROOTS   =  0,
    INF_ROOTS    = -1,
    /// При вычислении корней произошла ошибка
    ERANGE_SOLVE = -2
};

/**
 * Решает линейное уравнение вида kx + b = 0, записывает решение (при наличии) в переданную переменную (x)
 * и возвращает количество найденных решений в объекте enum num_roots
 * @param k Коэффициент при линейном члене
 * @param b Свободный коэффициент
 * @param x Указатель на переменную для корня (при наличии)
 * @return Количество найденных корней
 */
enum num_roots solve_lin_eq(double k, double b, double *x);

/**
 * Решает квадратное уравнение вида a*x^2 + b*x + c = 0, записывает решения (при наличии) в переданные переменные (x1 и x2),
 * если они не NULL, и возвращает их количество или произошедшую ошибку в объекте enum num_roots
 *
 *
 * Возможное ошибки
 * 1. ERANGE_SOLVE -- при переполнении double во внутренних вычислениях
 *
 * Критерии переполнения:
 * 1. b^2 < DOUBLE_MAX
 * 2. 4*a*c < DOUBLE_MAX
 * 3. b^2 - 4*a*c < DOUBLE_MAX
 *
 *
 * Если решений меньше двух, неиспользованные переменные не изменяют своего значения.
 * При наличии одного решения оно записывается в x1.
 * При наличии двух решений порядок x1 и x2 не гарантируется.
 * @param a Коэффициент при квадратичном члене
 * @param b Коэффициент при линейном члене
 * @param c Свободный коэффициент
 * @param x1 Указатель на переменную для первого корня (при наличии) или NULL
 * @param x2 Указатель на переменную для второго корня (при наличии) или NULL
 * @return Количество найденных корней
 */
enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2);

/**
 * Выводит количество корней и сами корни.
 */
void print_solution(enum num_roots n_roots, double x1, double x2);

/**
 * Считывает коэффициенты из stdin и записывает в переданные переменные. При неудаче переспрашивает
 * В случае ошибки считывания устанавливает возвращает ненулевое значение, соответствующее errno значению ошибки
 */
int input_coeffs(double *a, double *b, double *c);

#endif //QUAD_EQUATION_SOLVER_H
