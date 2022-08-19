#include <math.h>
#include <cfloat>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include "equation_solver.h"

static int read_double(double *x, const char *prompt, FILE *in_stream, FILE *out_stream);
static void flush_input(FILE *stream);


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

void print_solution(enum num_roots n_roots, double x1, double x2, FILE *stream)
{
    assert(stream != NULL && "pointer can't be null");

    switch (n_roots) {
        case TWO_ROOTS:
            assert(isfinite(x1) && "parameter must be finite");
            assert(isfinite(x2) && "parameter must be finite");

            fprintf(stream, "Найдено 2 решения: %.3e и %.3e\n", x1, x2);
            break;
        case ONE_ROOT:
            assert(isfinite(x1) && "parameter must be finite");

            fprintf(stream, "Найдено одно решение: %.3e\n", x1);
            break;
        case ZERO_ROOTS:
            fprintf(stream, "Решений не найдено\n");
            break;
        case INF_ROOTS:
            fprintf(stream, "Решений бесконечно много\n");
            break;
        case ERANGE_SOLVE:
            fprintf(stream, "Не удалось решить уравнение: слишком большие коэффициенты\n");
            break;
        default:
            assert(0 && "Invalid enum member");
            break;
    }
}

/**
 * Сбрасывает ввод из stream до символа \n
 */
static void flush_input(FILE *stream)
{
    assert(stream != NULL && "pointer can't be null");
    while (getc(stream) != '\n') {}
}

/**
 * Считывает аргумент из in_stream, предварительно выводя prompt в out_stream, и записывает в x. В случае некорректного ввода переспрашивает.
 * В случае ошибки возвращает ненулевое значение, соответствующее errno значению этой ошибки
 * @param x Куда записывается считанный double
 * @param prompt Приглашающая строка перед вводом
 * @return 0 или errno в случае ошибки
 */
static int read_double(double *x, const char *prompt, FILE *in_stream, FILE *out_stream)
{
    assert(x != NULL && "pointer can't be null");
    assert(prompt != NULL && "pointer can't be null");
    assert(in_stream != NULL && "pointer can't be null");
    assert(out_stream != NULL && "pointer can't be null");

    errno = 0;
    int scanf_res = 0;

    while (true) {
        fprintf(out_stream, "%s", prompt);
        scanf_res = fscanf(in_stream, "%lf", x);

        // При слишком больших числах или при нечисловом вводе переспрашиваем
        if (errno == ERANGE || (errno == 0 && scanf_res == 0)) {
            flush_input(in_stream); //Сбрасываем неправильный ввод
            errno = 0; // Обнуляем возможную ошибку ERANGE
            fprintf(out_stream, "Неправильный ввод, пожалуйста, введите число, причем не слишком большое\n");
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

int input_coeffs(double *a, double *b, double *c, FILE *in_stream, FILE *out_stream)
{
    assert(in_stream != NULL && "pointer can't be null");
    assert(out_stream != NULL && "pointer can't be null");
    assert(a != NULL && "pointer can't be null");
    assert(b != NULL && "pointer can't be null");
    assert(c != NULL && "pointer can't be null");

    double *coeffs[3] = {a, b, c};
    const char *prompts[3] = {"a = ", "b = ", "c = "};
    int read_res = 0;

    fprintf(out_stream, "Введите коэффициенты уравнения ax^2 + bx + c = 0\n");

    for (int i = 0; i < 3; i++) {
        read_res = read_double(coeffs[i], prompts[i], in_stream, out_stream);

        // Если произошла ошибка, пробрасываем ее дальше
        if (read_res != 0) {
            return read_res;
        }
    }
    return 0;
}