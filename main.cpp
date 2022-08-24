#include <cstdio>
#include <cstring>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "equation_solver.h"

#ifdef TEST
#include "test_equation_solver.h"
#endif

int parse_argv (int argc, char *argv[], int n_coeffs, double *coeffs);
int test_main  (int argc, char *argv[]);

/// Number of coefficients in quadric equation
static const int NUM_COEFFS = 3;

int main (int argc, char *argv[])
{
#ifdef TEST
    return test_main (argc, argv);
#endif

    double coeffs[NUM_COEFFS]     = {NAN, NAN, NAN};
    double  roots[NUM_COEFFS - 1] = {NAN, NAN};

    if (parse_argv (argc, argv, NUM_COEFFS, coeffs) != 0) return -1;

    num_roots n_roots = solve_quad_eq (coeffs[0], coeffs[1], coeffs[2], &roots[0], &roots[1]);

    if (n_roots == ERANGE_SOLVE)
    {
        printf ("Failed to solve equation: Coefficients out of range");
        return -1;
    }
    else
    {
        print_solution (n_roots, roots, stdout);
    }

    return 0;
}

int test_main (int argc, char *argv[])
{
    assert (argv != NULL && "pointer can't be NULL");

    int offset = 0;
    char *report_file = NULL;

    srand((unsigned int) time(NULL));

    if (argc >= 3 && strcmp (argv[1], "-r") == 0)
    {
        report_file = argv[2];
        offset = 2;
    }

    if      (argc == offset + 6) run_test(  argv[offset+1],     argv[offset+2],   argv[offset+3],
                                            argv[offset+4],     argv[offset+5],   "/dev/null"   , report_file);

    else if (argc == offset + 7) run_test(  argv[offset+1],     argv[offset+2],   argv[offset+3],
                                            argv[offset+4],     argv[offset+5],   argv[offset+6], report_file);
    else if (argc == offset + 1) run_test("tmp.txt", "input.txt", "lin.txt", "quad.txt", "output.txt", "/dev/null", report_file);
    else 
    {
        printf
            (
            "Testing quadratic solver\n"
            "Usage: quad [-r report_file] [tmp_file linear_test_file quadratic_test_file input_test_file output_test_file [dev_null_file]]\n"
            "Defaults: \n"
            "        report_file    =     stdout\n"
            "           tmp_file    =    tmp.txt\n"
            "   linear_test_file    =    lin.txt\n"
            "     quad_test_file    =   quad.txt\n"
            "    input_test_file    =  input.txt\n"
            "   output_test_file    = output.txt\n"
            "      dev_null_file    =  /dev/null\n"
            );
    }

    return 0;
}

/**
 * @brief      Get coefficients from CLI args or from interactive mode
 *
 * @param[in]  argc      The count of arguments
 * @param[in]  argv      The arguments array
 * @param[in]  n_coeffs  Number of coefficients
 * @param[out] coeffs    Array of coefficients
 *
 * @return     Non zero value on error
 */
int parse_argv (int argc, char *argv[], int n_coeffs, double *coeffs)
{
    int input_res = 0;

    if ((argc == 2 && strcmp (argv[1],"-h") == 0) || (argc != 2 && argc != n_coeffs + 1))
    {
        printf (
            "Quadratic equation solver\n"                                       
            "Usage:\n"                                                          
            "    * `quad -i` for interactive mode\n"                            
            "    * `quad a b c` for normal mode (solve ax^2 + bx + c = 0)\n"
            );

        return -1;
    }
    else if (argc == 2 && strcmp (argv[1], "-i") == 0)
    {
        input_res = input_coeffs (n_coeffs, coeffs, stdin, stdout);
    
        if (input_res != 0)
        {
            printf ("Failed to read coefficients: %s", strerror(input_res));
            return -1;
        }
    }
    else
    {
        assert (argc == (n_coeffs + 1) && "Error in logic: unexpected argc value");

        input_res = parse_coeffs (n_coeffs, coeffs, &argv[1]);

        if (input_res != 0)
        {
            printf ("Failed to parse coefficients, please use not very bin numbers\n");
            return -1;
        }
    }

    return 0;
}