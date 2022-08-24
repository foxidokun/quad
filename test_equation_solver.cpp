#include <time.h>
#include <cassert>
#include <cfloat>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include "equation_solver.h"
#include "common_equation_solver.h"
#include "test_equation_solver.h"

static double rand_range   (double min, double max);
static int    is_equal     (double x, double y);
static int    is_equal_set (double x1, double x2, double y1, double y2);

///Max line length in test
static const int inp_buffer_size = 128;

/// If condition is not zero, return -1 (bypass error)
#define UNWRAP(cond) {if (cond) return -1;}

/// Print "Test OK: {test_name}" to report_stream
#define REPORT_OK() { fprintf(report_stream, " ### Test OK: %s\n\n", __func__);}

int test_solve_quad_eq (num_roots n_roots_ref, double a, double b, double c, double x1_ref, double x2_ref, FILE *report_stream)
{
    assert (isfinite(a)      && "parameter must be finite");
    assert (isfinite(b)      && "parameter must be finite");
    assert (isfinite(c)      && "parameter must be finite");
    assert (isfinite(x1_ref) && "parameter must be finite");
    assert (isfinite(x2_ref) && "parameter must be finite");
    assert (report_stream != NULL && "pointer can't be NULL");

    double x1_out = NAN, x2_out = NAN;

    num_roots n_roots = solve_quad_eq (a, b, c, &x1_out, &x2_out);

    if (n_roots != n_roots_ref)
    {
        fprintf (report_stream, "## Test Error: Wrong number of roots ##\n");
        fprintf
            (
            report_stream,
            "Func: solve_quad_eq, parameters: (%lg, %lg, %lg, &x1, &x2). Expected %d, got %d\n\n",
            a, b, c, n_roots_ref, n_roots
            );

        return -1;
    }
    else if (n_roots == ONE_ROOT)
    {
        if (!is_equal (x1_out, x1_ref))
        {
            fprintf (report_stream, "## Test Error: Wrong root (ONE_ROOT) ##\n");
            fprintf
                (
                report_stream,
                "Func: solve_quad_eq, parameters: (%lg, %lg, %lg, &x1, &x2), output: (x1: %lg, x2: %lg), reference: (x1: %lg, x2: %lg) \n\n",
                a, b, c, x1_out, x2_out, x1_ref, x2_ref
                );
            return -1;
        }
    }
    else if (n_roots == TWO_ROOTS)
    {
        if (!is_equal_set (x1_out, x2_out, x1_ref, x2_ref))
        {
            fprintf (report_stream, "## Test Error: Wrong roots (TWO_ROOTS) ##\n");
            fprintf
                (
                report_stream,
                "Func: solve_quad_eq, parameters: (%lg, %lg, %lg, &x1, &x2), output: (x1: %lg, x2: %lg), reference: (x1: %lg, x2: %lg) \n\n",
                a, b, c, x1_out, x2_out, x1_ref, x2_ref
                );
            return -1;
        }
    }

    return 0;
}

int test_solve_lin_eq (num_roots n_roots_ref, double k, double b, double x_ref, FILE *report_stream)
{
    assert (isfinite(k)     && "parameter must be finite");
    assert (isfinite(b)     && "parameter must be finite");
    assert (isfinite(x_ref) && "parameter must be finite");
    assert (report_stream != NULL && "pointer can't be NULL");

    double x_out = NAN;

    num_roots n_roots = solve_lin_eq (k, b, &x_out);

    if (n_roots != n_roots_ref)
    {
        fprintf (report_stream, "## Test Error: Wrong number of roots. ##\n");
        fprintf 
            (
            report_stream,
            "Func: solve_lin_eq, parameters: (%lg, %lg, &x), expected: %d, got: %d\n\n",
            k, b, n_roots_ref, n_roots
            );

        return -1;
    }
    else if (n_roots == ONE_ROOT && !is_equal(x_out, x_ref))
    {
        fprintf (report_stream, "## Test Error: Wrong roots ##\n");
        fprintf
            (
            report_stream,
            "Func: solve_lin_eq, parameters: (%lg, %lg, &x), output (x: %lg), reference: (x: %lg)\n\n",
            k, b, x_out, x_ref
            );

        return -1;
    }

    return 0;
}

int manual_test_solve_lin_eq (FILE *in_stream, FILE *report_stream)
{
    assert (in_stream     != NULL && "pointer can't be NULL");
    assert (report_stream != NULL && "pointer can't be NULL");

    double k = NAN, b = NAN, x = NAN;
    int n_roots = 0;
    char buffer[inp_buffer_size] = "";

    while (fgets (buffer, inp_buffer_size, in_stream) != NULL)
    {
        if (buffer[0] == '#' || buffer[0] == '\n') continue;

        sscanf (buffer, "%lg %lg %d %lg", &k, &b, &n_roots, &x);

        UNWRAP (test_solve_lin_eq ( (num_roots) n_roots, k, b, x, report_stream));
    }

    REPORT_OK();
    return 0;
}

int manual_test_solve_quad_eq (FILE *in_stream, FILE *report_stream)
{
    assert (in_stream     != NULL && "pointer can't be NULL");
    assert (report_stream != NULL && "pointer can't be NULL");

    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN;
    int n_roots = 0;
    char buffer[inp_buffer_size] = "";

    UNWRAP (test_solve_quad_eq (ERANGE_SOLVE, -DBL_MAX / 15.5, sqrt(DBL_MAX / 2), DBL_MAX / 15.5, 0, 0, report_stream));

    while (fgets (buffer, inp_buffer_size, in_stream) != NULL)
    {
        if (buffer[0] == '#' || buffer[0] == '\n') continue;

        sscanf (buffer, "%lg %lg %lg %d %lg %lg", &a, &b, &c, &n_roots, &x1, &x2);

        UNWRAP (test_solve_quad_eq ((num_roots) n_roots, a, b, c, x1, x2, report_stream));
    }

    REPORT_OK();
    return 0;
}

int manual_test_input_coeffs (FILE *in_stream, FILE *dev_null, FILE *report_stream)
{
    assert (in_stream     != NULL && "pointer can't be NULL");
    assert ( dev_null     != NULL && "pointer can't be NULL");
    assert (report_stream != NULL && "pointer can't be NULL");

    const int num_coeffs = 3;
    double coeffs[num_coeffs] = {};

    while (input_coeffs (num_coeffs, coeffs, in_stream, dev_null) != EIO)
    {
        for (int i = 0; i < num_coeffs; ++i)
        {
            if (!is_equal (coeffs[i], 5)) 
            {
                fprintf (report_stream, "## Test Error: Wrong input ##\n");
                fprintf
                    (
                    report_stream,
                    "Expected %lg, got %lg\n\n",
                    5.0, coeffs[i]
                    );

                return -1;
            }
        }
    }

    REPORT_OK();
    return 0;
}

int manual_test_output_format (const char *tmp_file, FILE *ref_stream, FILE *report_stream)
{
    assert (tmp_file      != NULL && "pointer can't be NULL");
    assert (ref_stream    != NULL && "pointer can't be NULL");
    assert (report_stream != NULL && "pointer can't be NULL");

    FILE *write_stream = fopen (tmp_file, "w");
    double ref_roots[] = {228, 282};

    print_solution (ERANGE_SOLVE, ref_roots, write_stream);
    print_solution (INF_ROOTS,    ref_roots, write_stream);
    print_solution (ZERO_ROOTS,   ref_roots, write_stream);
    print_solution (ONE_ROOT,     ref_roots, write_stream);
    print_solution (TWO_ROOTS,    ref_roots, write_stream);
    fclose (write_stream);

    // Compare with sample
    int c = 0, c_ref = 0;
    unsigned long int pos = 0;
    FILE *read_stream = fopen (tmp_file, "r");

    while ((c = getc (read_stream)) != EOF)
    {
        pos++;
        c_ref = getc (ref_stream);

        if (c != c_ref) 
        {
            fprintf (report_stream, "## Test Error: Wrong output ##\n");
            fprintf
                (
                report_stream,
                "Mismatched character, expected %d, got %c on position %lu\n\n",
                c_ref, c, pos
                );
            return -1;
        }
    }

    fclose (read_stream);
    REPORT_OK();
    return 0;
}

int check_solve_lin_eq (double k, double b, double x, FILE *report_stream)
{
    assert (isfinite(k) && "parameter must be finite");
    assert (isfinite(b) && "parameter must be finite");
    assert (isfinite(x) && "parameter must be finite");
    assert (report_stream != NULL && "pointer can't be NULL");

    if (!is_zero (k*x + b))
    {
        fprintf (report_stream, "## Test Error: Wrong answer##\n");
        fprintf
            (
            report_stream,
            "k*x + b != 0, k: %lg, b: %lg, x: %lg\n\n",
            k, b, x
            );

        return -1;
    }

    return 0;
}

int auto_test_solve_lin_eq (FILE *report_stream)
{
    assert (report_stream != NULL && "pointer can't be null");

    double k = NAN, b = NAN, x = NAN;
    const int num_test = 100; // Number of test to run

    for (int i = 0; i < num_test; ++i)
    {
        k = rand_range (-100, 100);
        b = rand_range (-100, +100);

        switch(solve_lin_eq (k, b, &x)) {
            case ONE_ROOT:
                UNWRAP (check_solve_lin_eq (k, b, x, report_stream));
                break;

            case ZERO_ROOTS:
                break;

            case INF_ROOTS:
                x = rand_range (-100, +100);
                UNWRAP (check_solve_lin_eq (k, b, x, report_stream));
                break;

            case ERANGE_SOLVE:
                break;

            case TWO_ROOTS:
                assert (0 && "impossible number of roots");
                break;

            default:
                assert (0 && "Not a enum member");
                break;
        }
    }

    REPORT_OK();
    return 0;
}

int check_solve_quad_eq (double a, double b, double c, double x, FILE *report_stream)
{
    assert (isfinite(a) && "parameter must be finite");
    assert (isfinite(b) && "parameter must be finite");
    assert (isfinite(c) && "parameter must be finite");
    assert (isfinite(x) && "parameter must be finite");
    assert (report_stream != NULL && "pointer can't be NULL");


    if (!is_zero (a*x*x + b*x + c))
    {
        fprintf (report_stream, "## Test Error: Wrong answer##\n");
        fprintf
            (
            report_stream,
            "ax^2 + bx + c != 0, a: %lg, b: %lg, c: %lg, x: %lg\n\n",
            a, b, c, x
            );

        return -1;
    }

    return 0;
}

int auto_test_solve_quad_eq (FILE *report_stream)
{
    assert (report_stream != NULL && "pointer can't be NULL");

    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN;
    const int num_test = 100;

    for (int i = 0; i < num_test; ++i)
    {
        a = rand_range (-100, +100);
        b = rand_range (-100, +100);
        c = rand_range (-100, +100);

        switch (solve_quad_eq (a, b, c, &x1, &x2))
        {
            case TWO_ROOTS:
                UNWRAP (check_solve_quad_eq (a, b, c, x2, report_stream));
                [[fallthrough]];
                // And then we check x1 along with the ONE_ROOT branch

            case ONE_ROOT:
                UNWRAP (check_solve_quad_eq (a, b, c, x1, report_stream));
                break;
       
            case INF_ROOTS:
                x1 = rand_range (-100, +100);
                UNWRAP (check_solve_quad_eq (a, b, c, x1, report_stream));
                break;
       
            case ZERO_ROOTS:
                break;
       
            case ERANGE_SOLVE:
                assert (0 && "Test parameters can't be out of range");
                break;
       
            default:
                assert (0 && "Invalid enum member");
        }
    }

    REPORT_OK();
    return 0;
}

int auto_test_input_coeffs (const char *tmp_file, FILE *dev_null, FILE *report_stream)
{
    assert (tmp_file      != NULL && "pointer can't be null");
    assert (dev_null      != NULL && "pointer can't be null");
    assert (report_stream != NULL && "pointer can't be null");

    FILE *read_s  = fopen (tmp_file, "r");
    FILE *write_s = fopen (tmp_file, "w");

    const int num_coeffs = 3;
    const int num_test = 100;
    double inp_coeffs[num_coeffs] = {}; // Values from input_coeffs
    double ref_coeffs[num_coeffs] = {}; // Reference values

    for (int i = 0; i < num_test; ++i)
    {
        for (int j = 0; j < num_coeffs; ++j)
        {
            ref_coeffs[j] = rand_range (-100, 100);
            fprintf (write_s, "%.18lf\nadsjkfhakjlhgljak\n9e999\n", ref_coeffs[j]);
        }

        fflush (write_s);

        if (input_coeffs (num_coeffs, inp_coeffs, read_s, dev_null) != 0)
        {
            fprintf (report_stream, "## Test Error: Wrong input ##\n");
            fprintf (report_stream, "Failed to read input string with vals\n");

            for (int j = 0; j < num_coeffs; ++j) 
            {
                fprintf (report_stream, "Val #%d: %lg\n", j, ref_coeffs[j]);
            }

            putc ('\n', report_stream);

            return -1;
        }


        for (int j = 0; j < num_coeffs; ++j)
        {
            if (!is_equal (inp_coeffs[j], ref_coeffs[j]))
            {
                fprintf (report_stream, "## Test Error: Wrong input ##\n");
                fprintf (report_stream, "Invalid value #%d: expected %lg, got %lg\n\n", j, ref_coeffs[j], inp_coeffs[j]);

                return -1;
            }
        }
    }

    fclose (read_s);
    fclose (write_s);

    REPORT_OK();
    return 0;
}

void run_test(const char *tmp_file, const char *input_file, const char *lin_file, const char *quad_file,
              const char *output_ref_file, const char *dev_null, const char *report_file)
{
    assert (tmp_file        != NULL && "pointer can't be null");
    assert (input_file      != NULL && "pointer can't be null");
    assert (quad_file       != NULL && "pointer can't be null");
    assert (output_ref_file != NULL && "pointer can't be null");
    assert (dev_null        != NULL && "pointer can't be null");

    FILE *in_stream       = fopen (input_file,      "r");
    FILE *quad_stream     = fopen (quad_file,       "r");
    FILE *lin_stream      = fopen (lin_file,        "r");
    FILE *ref_stream      = fopen (output_ref_file, "r");
    FILE *dev_null_stream = fopen (dev_null,        "w");
    FILE *report_stream   = NULL;

    if (report_file != NULL) report_stream  = fopen (report_file, "w");
    else                     report_stream  = stdout;
 
    unsigned int success = 0;
    unsigned int failed  = 0;

    #define _LOG_TEST(test)                                             \
    {                                                                   \
        if (test)                                                       \
        {                                                               \
            fprintf(report_stream, " ### TEST FAILED: %s\n\n", #test);    \
            failed++;                                                   \
        }                                                               \
        else                                                            \
        {                                                               \
            success++;                                                  \
        }                                                               \
    }                   

    fprintf(report_stream, "=== === Tests running... === ===\n");

    _LOG_TEST (manual_test_solve_lin_eq  (lin_stream,  report_stream));
    _LOG_TEST (manual_test_solve_quad_eq (quad_stream, report_stream));

    fseek(in_stream, 0, SEEK_SET);

    _LOG_TEST (manual_test_input_coeffs  (in_stream, dev_null_stream, report_stream));
    _LOG_TEST (manual_test_output_format (tmp_file, ref_stream, report_stream));

    _LOG_TEST (auto_test_solve_lin_eq  (report_stream));
    _LOG_TEST (auto_test_solve_quad_eq (report_stream));
    _LOG_TEST (auto_test_input_coeffs  (tmp_file, dev_null_stream, report_stream));

    fprintf(report_stream, "\n==========================================\n");
    fprintf(report_stream, "Tests: All: %u Failed: %u Passed: %u Success ratio: %3.1f%%\n",
            success+failed, failed, success, success * 100.0 / (failed + success));

    #undef _LOG_TEST
}

/**
 * Generate random value in [min, max] range
 */
static double rand_range (double min, double max)
{
    double range = (max - min);
    return min + ((double) rand() / RAND_MAX) * range;
}

/**
 * Compare x with y, taking into account floating point error
 */
static int is_equal (double x, double y)
{
    return is_zero (x - y);
}

/**
 * @brief      Compare {x1, x2} to {y1, y2}, taking into account floating point error
 *
 * @return     True if equal set, False otherwise.
 */
static int is_equal_set (double x1, double x2, double y1, double y2)
{
    return  (is_equal (x1, y1) && is_equal (x2, y2)) ||
            (is_equal (x1, y2) && is_equal (x2, y1));
}