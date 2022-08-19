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

    // Overflow checks:
    // 1. b^2 - 4*a*c < DOUBLE_MAX
    // 1.1 b^2 < DOUBLE_MAX
    // 1.2 4*a*c < DOUBLE_MAX
    if (fabs(b) > sqrt(DBL_MAX) || //1.1
        (!is_zero(c) && fabs(a) > (DBL_MAX / fabs(c) / 4)) || //1.2
        pow(b, 2) > (DBL_MAX - 4 * a * c)) //1
    {
        return ERANGE_SOLVE;
    }

    // The equation is linear
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

    // k = 0 and there are either no solutions or infinitely many (when b=0)
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

            fprintf(stream, "2 solutions: %.3e и %.3e\n", x1, x2);
            break;
        case ONE_ROOT:
            assert(isfinite(x1) && "parameter must be finite");

            fprintf(stream, "1 solution: %.3e\n", x1);
            break;
        case ZERO_ROOTS:
            fprintf(stream, "No solutions\n");
            break;
        case INF_ROOTS:
            fprintf(stream, "Infinitive number of roots\n");
            break;
        case ERANGE_SOLVE:
            fprintf(stream, "Failed to solve equation: Coefficients out of range\n");
            break;
        default:
            assert(0 && "Invalid enum member");
            break;
    }
}

///@brief Flush input stream to '\\n' symbol
static void flush_input(FILE *stream)
{
    assert(stream != NULL && "pointer can't be null");
    while (getc(stream) != '\n') {}
}

/**
 * @brief Read coefficients from in_stream and write them to given variables using out_stream for asking question with given prompt.
 *
 * In case of a read error, returns a non-zero value corresponding to the errno value of the error
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

        // For too large numbers or for non-numeric input, ask again
        if (errno == ERANGE || (errno == 0 && scanf_res == 0)) {
            flush_input(in_stream); //Flush bad input
            errno = 0;
            fprintf(out_stream, "Bad input, please enter not very big number\n");
        } else {
            break;
        }
    }

    if (errno != 0) {
        return errno;
    }

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

    fprintf(out_stream, "Enter equation (ax^2 + bx + c = 0) coefficients\n");

    for (int i = 0; i < 3; i++) {
        read_res = read_double(coeffs[i], prompts[i], in_stream, out_stream);

        // If error happened, return it
        if (read_res != 0) {
            return read_res;
        }
    }
    return 0;
}