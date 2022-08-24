#include <math.h>
#include <cfloat>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include "equation_solver.h"

static int read_double(double *x, const char *prompt, FILE *in_stream, FILE *out_stream);
static void flush_input(FILE *stream);

#ifdef NDEBUG

    #define CHECK_RANGE(cond) {if (!(cond)) return ERANGE_SOLVE; }

#else
    
    #define CHECK_RANGE(cond)                                                       \
    {                                                                               \
        if (!(cond))                                                                \
        {                                                                           \
            fprintf(stderr, "\n-- Warning: Overflow in internal calculation -- \n");\
            fprintf(stderr, "Condition: %s\n", #cond);                              \
            fprintf(stderr, "Line: %d, File: %s, Func: %s\n\n",                     \
                    __LINE__, __FILE__, __PRETTY_FUNCTION__);                       \
            return ERANGE_SOLVE;                                                    \
        }                                                                           \
    }                                                                               \

#endif


num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2)
{
    assert (isfinite(a) && "parameter must be finite");
    assert (isfinite(b) && "parameter must be finite");
    assert (isfinite(c) && "parameter must be finite");
    assert (x1 != NULL  && "pointer can't be null");
    assert (x2 != NULL  && "pointer can't be null");
    assert (x1 != x2    && "pointers can't be same");

    // The equation is linear

    if (is_zero(a))
    {
        return solve_lin_eq(b, c, x1);
    }
    else
    {
        CHECK_RANGE (!(fabs (b) > sqrt (DBL_MAX)));
        CHECK_RANGE (!(!is_zero (c) && fabs (a) > (DBL_MAX / fabs(c) / 4)));
        CHECK_RANGE (!(b*b > (DBL_MAX - 4*a*c)));

        double disc = b*b - 4*a*c;

        if (is_zero(disc))
        {
            *x1 = -b / a / 2;
            return ONE_ROOT;
        }
        else if (disc < 0)
        {
            return ZERO_ROOTS;
        }
        else // disc > 0
        {
            if (is_zero(b))
            {
                *x1 = -sqrt(-c / a);
                *x2 = +sqrt(-c / a);
            }
            else if (is_zero(c))
            {
                *x1 = 0;
                *x2 = -b / a;
            }
            else
            {
                double sq_disc = sqrt(disc);

                *x1 = (-b + sq_disc) / a / 2;
                *x2 = (-b - sq_disc) / a / 2;
            }

            return TWO_ROOTS;
        }
    }
}

enum num_roots solve_lin_eq(double k, double b, double *x)
{
    assert (isfinite(k) && "parameter must be finite");
    assert (isfinite(b) && "parameter must be finite");
    assert (x != NULL   && "pointer can't be null");

    // k = 0 and there are either no solutions or infinitely many (when b=0)
    if (is_zero(k)) 
    {
        if (is_zero(b))  return INF_ROOTS;
        else             return ZERO_ROOTS;
    }
    else 
    {
        CHECK_RANGE (fabs(b) < DBL_MAX*fabs(k));

        *x = -b / k;
        return ONE_ROOT;
    }
}

void print_solution(enum num_roots n_roots, double roots[], FILE *stream)
{
    assert (stream != NULL && "pointer can't be null");
    assert (roots  != NULL && "pointer can't be null");

    for (int i = n_roots-1; i >= 0; --i)
    {
        assert (isfinite(roots[i]) && "parameter must be finite");

        if (is_zero(roots[i])) roots[i] = 0;
    }

    switch (n_roots) {
        case TWO_ROOTS:
            fprintf(stream, "2 solutions: %.3e и %.3e\n", roots[0], roots[1]);
            break;

        case ONE_ROOT:
            fprintf(stream, "1 solution: %.3e\n", roots[0]);
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
            assert (0 && "Invalid enum member");
            break;
    }
}

///@brief Flush input stream to '\\n' symbol
static void flush_input(FILE *stream)
{
    assert (stream != NULL && "pointer can't be null");
    while (getc(stream) != '\n') {}
}

/**
 * @brief Read coefficients from in_stream and write them to given variables using out_stream for asking question with given prompt.
 *
 * In case of a read error, returns a non-zero value corresponding to the errno value of the error
 */
static int read_double(double *x, const char *prompt, FILE *in_stream, FILE *out_stream)
{
    assert (x            != NULL && "pointer can't be null");
    assert (prompt       != NULL && "pointer can't be null");
    assert (in_stream    != NULL && "pointer can't be null");
    assert (out_stream   != NULL && "pointer can't be null");

    errno = 0;
    int scanf_res = 0;

    while (true)
    {
        fprintf(out_stream, "%s", prompt);
        scanf_res = fscanf(in_stream, "%lf", x);

        // For too large numbers or for non-numeric input, ask again
        if (errno == ERANGE || (errno == 0 && scanf_res == 0))
        {
            flush_input(in_stream); //Flush bad input
            errno = 0;
            fprintf(out_stream, "Bad input, please enter not very big number\n");
        }
        else break;
    }

    if (errno != 0) {
        return errno;
    }

    assert (scanf_res == 1 || scanf_res == EOF);

    if (scanf_res == 1)  return 0;
    else                 return EIO;
}

int input_coeffs(int n_coeffs, double coeffs[], FILE *in_stream, FILE *out_stream)
{
    assert (in_stream  != NULL && "pointer can't be null");
    assert (out_stream != NULL && "pointer can't be null");
    assert (coeffs     != NULL && "pointer can't be null");

    int err = 0;
    char print_buf[32] = "";

    fprintf(out_stream, "Enter equation (ax^n + ... + bx^2 + cx + d = 0) coefficients\n");

    for (int i = 0; i < n_coeffs; ++i)
    {
        sprintf(print_buf, "Coefficient at x^%d: ", n_coeffs - 1 - i);
        err = read_double (&coeffs[i], print_buf, in_stream, out_stream);

        if (err) return err;
    }

    return 0;
}

int parse_coeffs(int n_coeffs, double coeffs[], const char **strings)
{
    assert (strings != NULL && "pointer can't be null");
    assert (coeffs  != NULL && "pointer can't be null");

    for (int i = n_coeffs - 1; i >= 0; --i)
    {
        assert (strings[i] != NULL && "pointer can't be null");

        const char *start = strings[i];
        char **end = (char **) &strings[i];

        coeffs[i] = strtod(start, end);

        //Nothing converted or bad input
        if (start == *end || !isfinite(coeffs[i]) || coeffs[i] == HUGE_VAL) 
        {
            return -1;
        }


   }

   return 0;
}