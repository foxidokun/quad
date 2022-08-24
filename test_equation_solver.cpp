#include <time.h>
#include <cassert>
#include <cfloat>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include "equation_solver.h"
#include "test_equation_solver.h"

static double rand_range   (double min, double max);
static int    is_equal     (double x, double y);
static int    is_equal_set (double x1, double x2, double y1, double y2);


void test_solve_quad_eq (num_roots n_roots_ref, double a, double b, double c, double x1_ref, double x2_ref)
{
    double x1_out = NAN, x2_out = NAN;

    num_roots n_roots = solve_quad_eq (a, b, c, &x1_out, &x2_out);

    if (n_roots != n_roots_ref)
    {
        fprintf (stderr, "## Test Error: Wrong number of roots ##\n");
        fprintf
            (
            stderr,
            "Func: solve_quad_eq, parameters: (%lg, %lg, %lg, &x1, &x2). Expected %d, got %d\n\n",
            a, b, c, n_roots_ref, n_roots
            );

        return;
    }
    else if (n_roots == ONE_ROOT)
    {
        if (!is_equal (x1_out, x1_ref))
        {
            fprintf (stderr, "## Test Error: Wrong root (ONE_ROOT) ##\n");
            fprintf
                (
                stderr,
                "Func: solve_quad_eq, parameters: (%lg, %lg, %lg, &x1, &x2), output: (x1: %lg, x2: %lg), reference: (x1: %lg, x2: %lg) \n\n",
                a, b, c, x1_out, x2_out, x1_ref, x2_ref
                );
            return;
        }
    }
    else if (n_roots == TWO_ROOTS)
    {
        if (!is_equal_set (x1_out, x2_out, x1_ref, x2_ref))
        {
            fprintf (stderr, "## Test Error: Wrong roots (TWO_ROOTS) ##\n");
            fprintf
                (
                stderr,
                "Func: solve_quad_eq, parameters: (%lg, %lg, %lg, &x1, &x2), output: (x1: %lg, x2: %lg), reference: (x1: %lg, x2: %lg) \n\n",
                a, b, c, x1_out, x2_out, x1_ref, x2_ref
                );
            return;
        }
    }
}

void test_solve_lin_eq (num_roots n_roots_ref, double k, double b, double x_ref)
{
    double x_out = NAN;

    num_roots n_roots = solve_lin_eq (k, b, &x_out);

    if (n_roots != n_roots_ref)
    {
        fprintf (stderr, "## Test Error: Wrong number of roots. ##\n");
        fprintf (stderr,
            "Func: solve_lin_eq, parameters: (%lg, %lg, &x), expected: %d, got: %d\n\n",
            k, b, n_roots_ref, n_roots);

        return;
    }
    else if (n_roots == ONE_ROOT && !is_equal(x_out, x_ref))
    {
        fprintf (stderr, "## Test Error: Wrong roots ##\n");
        fprintf (stderr,
            "Func: solve_lin_eq, parameters: (%lg, %lg, &x), output (x: %lg), reference: (x: %lg)\n\n",
            k, b, x_out, x_ref);

        return;
    }
}

void manual_test_solve_lin_eq (FILE *in_stream)
{
    double k = NAN, b = NAN, x = NAN;
    int n_roots = 0;
    const int buffer_size = 1024;
    char buffer[buffer_size] = "";

    while (fgets (buffer, buffer_size, in_stream) != NULL)
    {
        if (buffer[0] == '#' || buffer[0] == '\n') continue;

        sscanf (buffer, "%lg %lg %d %lg", &k, &b, &n_roots, &x);

        test_solve_lin_eq ( (num_roots) n_roots, k, b, x);
    }
}

void manual_test_solve_quad_eq (FILE *in_stream)
{
    double a = NAN, b = NAN, c = NAN, x1 = NAN, x2 = NAN;
    int n_roots = 0;
    const int buffer_size = 1024;
    char buffer[buffer_size] = "";

    test_solve_quad_eq (ERANGE_SOLVE, -DBL_MAX / 15.5, sqrt(DBL_MAX / 2), DBL_MAX / 15.5, 0, 0);

    while (fgets (buffer, buffer_size, in_stream) != NULL)
    {
        if (buffer[0] == '#' || buffer[0] == '\n') continue;

        sscanf (buffer, "%lg %lg %lg %d %lg %lg", &a, &b, &c, &n_roots, &x1, &x2);

        test_solve_quad_eq ((num_roots) n_roots, a, b, c, x1, x2);
    }

}

void manual_test_input_coeffs (FILE *in_stream, FILE *dev_null)
{
    const int num_coeffs = 3;
    double coeffs[num_coeffs] = {};

    while (input_coeffs (num_coeffs, coeffs, in_stream, dev_null) != EIO)
    {
        for (int i = 0; i < num_coeffs; ++i)
        {
            if (!is_equal (coeffs[i], 5)) 
            {
                fprintf (stderr, "## Test Error: Wrong input ##\n");
                fprintf
                    (
                    stderr,
                    "Expected %lg, got %lg\n\n",
                    5.0, coeffs[i]
                    );

                return;
            }
        }
    }
}

void manual_test_output_format (const char *tmp_file, FILE *ref_stream)
{
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
            fprintf (stderr, "## Test Error: Wrong output ##\n");
            fprintf
                (
                stderr,
                "Mismatched character, expected %d, got %c on position %lu\n\n",
                c_ref, c, pos
                );
        }
    }

    fclose (read_stream);
}

void check_solve_lin_eq (double k, double b, double x)
{
    if (!is_zero (k*x + b))
    {
        fprintf (stderr, "## Test Error: Wrong answer##\n");
        fprintf
            (
            stderr,
            "k*x + b != 0, k: %lg, b: %lg, x: %lg\n\n",
            k, b, x
            );
    }
}

void auto_test_solve_lin_eq ()
{
    double k = NAN, b = NAN, x = NAN;
    const int num_test = 100; // Number of test to run

    for (int i = 0; i < num_test; ++i)
    {
        k = rand_range (-100, 100);
        b = rand_range (-100, +100);

        switch(solve_lin_eq (k, b, &x)) {
            case ONE_ROOT:
                check_solve_lin_eq (k, b, x);
                break;

            case ZERO_ROOTS:
                break;

            case INF_ROOTS:
                x = rand_range (-100, +100);
                check_solve_lin_eq (k, b, x);
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
}

void check_solve_quad_eq (double a, double b, double c, double x)
{
    if (!is_zero (a*x*x + b*x + c))
    {
        fprintf (stderr, "## Test Error: Wrong answer##\n");
        fprintf
            (
            stderr,
            "ax^2 + bx + c != 0, a: %lg, b: %lg, c: %lg, x: %lg\n\n",
            a, b, c, x
            );
    }
}

void auto_test_solve_quad_eq ()
{
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
                check_solve_quad_eq (a, b, c, x2);
                [[fallthrough]];
                // And then we check x1 along with the ONE_ROOT branch

            case ONE_ROOT:
                check_solve_quad_eq (a, b, c, x1);
                break;
       
            case INF_ROOTS:
                x1 = rand_range (-100, +100);
                check_solve_quad_eq (a, b, c, x1);
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
}

void auto_test_input_coeffs (const char *tmp_file, FILE *dev_null)
{
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
            fprintf (stderr, "## Test Error: Wrong input ##\n");
            fprintf (stderr, "Failed to read input string with vals\n");

            for (int j = 0; j < num_coeffs; ++j) 
            {
                fprintf (stderr, "Val #%d: %lg\n", j, ref_coeffs[j]);
            }

            putc ('\n', stderr);
        }


        for (int j = 0; j < num_coeffs; ++j)
        {
            if (!is_equal (inp_coeffs[j], ref_coeffs[j]))
            {
                fprintf (stderr, "## Test Error: Wrong input ##\n");
                fprintf (stderr, "Invalid value #%d: expected %lg, got %lg\n\n", j, ref_coeffs[j], inp_coeffs[j]);
            }
        }
    }

    fclose (read_s);
    fclose (write_s);
}

void run_test(const char *tmp_file, const char *input_file, const char *lin_file, const char *quad_file,  const char *output_ref_file, const char *dev_null)
{
    FILE *in_stream       = fopen (input_file,      "r");
    FILE *quad_stream     = fopen (quad_file,       "r");
    FILE *lin_stream      = fopen (lin_file,        "r");
    FILE *ref_stream      = fopen (output_ref_file, "r");
    FILE *dev_null_stream = fopen (dev_null,        "w");

    manual_test_solve_lin_eq  (lin_stream);
    manual_test_solve_quad_eq (quad_stream);

    fseek(in_stream, 0, SEEK_SET);

    manual_test_input_coeffs  (in_stream, dev_null_stream);
    manual_test_output_format (tmp_file, ref_stream);

    auto_test_solve_lin_eq  ();
    auto_test_solve_quad_eq ();
    auto_test_input_coeffs  (tmp_file, dev_null_stream);
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