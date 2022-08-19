#ifndef QUAD_EQUATION_SOLVER_H
#define QUAD_EQUATION_SOLVER_H

/// Такое значение, что 0+DBL_ERROR все еще 0
/// Не стандартный DBL_EPSILON, так как он слишком маленький
const double DBL_ERROR = 1e-11;

/// Количество корней у уравнения
enum num_roots {
    TWO_ROOTS    =  2,
    ONE_ROOT     =  1,
    ZERO_ROOTS   =  0,
    INF_ROOTS    = -1,
    /// При вычислении корней произошла ошибка переполнения
    ERANGE_SOLVE = -2
};

/**
 * Сравнивает переданное double число с нулем с учетом погрешности double арифметики
 */
static bool is_zero(double x)
{
    return fabs(x) < DBL_ERROR;
}

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
 * и возвращает их количество или произошедшую ошибку в объекте enum num_roots
 *
 *
 * Возможное ошибки
 * 1. ERANGE_SOLVE -- при переполнении double во внутренних вычислениях
 *
 * Критерии проверки на переполнение:
 * 1. b^2 < DOUBLE_MAX
 * 2. abs(4*a*c) < DOUBLE_MAX
 * 3. abs(b^2 - 4*a*c) < DOUBLE_MAX
 *
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
enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2);

/**
 * Выводит количество корней и сами корни в stream
 */
void print_solution(enum num_roots n_roots, double x1, double x2, FILE *stream);

/**
 * Считывает коэффициенты из in_stream, выводя вопросы в out_stream, и записывает коэфы в переданные переменные. При неудаче переспрашивает
 * В случае ошибки считывания возвращает ненулевое значение, соответствующее errno значению ошибки
 */
int input_coeffs(double *a, double *b, double *c, FILE *in_stream, FILE *out_stream);

/**
 * Прогоняет все доступные тесты
 * @param tmp_file Имя временного файла для тестирования
 * @param input_file Имя файла с тестовым вводом
 * @param output_ref_file Имя файла с референсным выводом
 */
void run_test(const char *tmp_file, const char *input_file, const char *output_ref_file);

void test_solve_lin_eq();
void test_solve_quad_eq();

///@param input_file Имя файла с проверяемым вводом
///@param dev_null Поток для записи, которая никак не используется
void test_input_coeffs(FILE *in_stream, FILE *dev_null);

/** @param tmp_file Имя временного файла для тестирования
 * @param output_ref_file Имя файла с референсным выводом
 */
void test_output_format(const char *tmp_file, FILE *ref_stream);

void auto_test_solve_lin_eq();
void auto_test_solve_quad_eq();

///@param tmp_file Имя для временного файла, требуемого для теста.
///@param dev_null Поток для записи, которая никак не используется
void auto_test_input_coeffs(const char *tmp_file, FILE *dev_null);


#endif //QUAD_EQUATION_SOLVER_H
