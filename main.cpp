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

int main(int argc, char *argv[])
{
    srand(time(NULL));

#ifdef TEST

    if      (argc == 6) run_test(  argv[1],     argv[2],   argv[3],    argv[4],      argv[5], "/dev/null");
    else if (argc == 7) run_test(  argv[1],     argv[2],   argv[3],    argv[4],      argv[5],     argv[6]);
    else if (argc == 1) run_test("tmp.txt", "input.txt", "lin.txt", "quad.txt", "output.txt", "/dev/null");
    else 
    {
        printf
            (
            "Testing quadratic solver\n"
            "Usage: quad [tmp_file linear_test_file quadratic_test_file input_test_file output_test_file [dev_null_file]]\n"
            "Defaults: \n"
            "   tmp_file            =    tmp.txt\n"
            "   linear_test_file    =    lin.txt\n"
            "     quad_test_file    =   quad.txt\n"
            "    input_test_file    =  input.txt\n"
            "   output_test_file    = output.txt\n"
            "      dev_null_file    =  /dev/null\n"
            );
    }

    return 0;

#endif

    double coeffs[] = {NAN, NAN, NAN};
    double  roots[] = {NAN, NAN};
    int input_res   = 0;

    if ((argc == 2 && strcmp (argv[1],"-h") == 0) || (argc != 2 && argc != 4))
    {
        printf (
            "Quadratic equation solver\n"                                       
            "Usage:\n"                                                          
            "    * `quad -i` for interactive mode\n"                            
            "    * `quad a b c` for normal mode (solve ax^2 + bx + c = 0)\n"
            );

        return 0;
    }
    else if (argc == 2 && strcmp (argv[1], "-i") == 0)
    {
        input_res = input_coeffs (3, coeffs, stdin, stdout);
    
        if (input_res != 0)
        {
            printf ("Failed to read coefficients: %s", strerror(input_res));
            return -1;
        }
    }
    else // argc == 4
    {
        assert (argc == 4 && "Error in logic: unexpected argc value");

        input_res = parse_coeffs (3, coeffs, &argv[1]);

        if (input_res != 0)
        {
            printf ("Failed to parse coefficients, please use not very bin numbers\n");
            return -1;
        }
    }


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
