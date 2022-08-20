#ifndef QUAD_EQUATION_SOLVER_H
#define QUAD_EQUATION_SOLVER_H
#include<math.h>
///@brief Floating point calculations accuracy
const double DBL_ERROR = 1e-11;

///@brief Number of equation roots
enum num_roots {
    TWO_ROOTS    =  2,
    ONE_ROOT     =  1,
    ZERO_ROOTS   =  0,
    INF_ROOTS    = -1,
    /// Coefficient out of range
    ERANGE_SOLVE = -2
};

/**
 * @brief Compare double to zero, taking into account floating point calculation error
 */
static bool is_zero(double x)
{
    return fabs(x) < DBL_ERROR;
}

/**@brief Solve linear equation, write root into given variable (x) and return number of solutions found
 *
 * @param k Linear coefficient
 * @param b Free coefficient
 * @param x Variable to store equation root
 * @return Number of equation roots
 */
enum num_roots solve_lin_eq(double k, double b, double *x);

/**@brief Solve quadratic equation, write roots into given variables (x1&x2) and return number of solutions found or error
 *
 * Possible errors
 * 1. ERANGE_SOLVE -- overflow in internal calculations
 *
 * Overflow checks:
 * 1. b^2 < DOUBLE_MAX
 * 2. abs(4*a*c) < DOUBLE_MAX
 * 3. abs(b^2 - 4*a*c) < DOUBLE_MAX
 *
 * If there are less than two solutions, unused variables do not change their value.
 * If there is one solution, it is written in x1.
 * If there are two solutions, the order of x1 and x2 is not guaranteed.
 *
 * @param a Quadratic coefficient
 * @param b Linear coefficient
 * @param c Free coefficient
 * @param x1 Pointer to store equation root
 * @param x2 Pointer to store equation root
 * @return Number of equation roots
 */
enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2);

///@brief Print solution to stream
void print_solution(enum num_roots n_roots, double x1, double x2, FILE *stream);

/**
 * @brief Read coefficients from in_stream and write them to given variables using out_stream for asking question.
 *
 * In case of a read error, returns a non-zero value corresponding to the errno value of the error
 */
int input_coeffs(double *a, double *b, double *c, FILE *in_stream, FILE *out_stream);

/**
 * @brief Run all tests
 * @param tmp_file Temporary file
 * @param input_file File with sample input
 * @param output_ref_file File with sample output
 * @param dev_null /dev/null or analog
 */
void run_test(const char *tmp_file, const char *input_file, const char *output_ref_file, const char *dev_null);

void test_solve_lin_eq();
void test_solve_quad_eq();

///@param input_file File with sample input
///@param dev_null /dev/null stream
void test_input_coeffs(FILE *in_stream, FILE *dev_null);

/** @param tmp_file Temporary file
 * @param output_ref_file File with sample output
 */
void test_output_format(const char *tmp_file, FILE *ref_stream);

void auto_test_solve_lin_eq();
void auto_test_solve_quad_eq();

///@param tmp_file Temporary file
///@param dev_null /dev/null stream
void auto_test_input_coeffs(const char *tmp_file, FILE *dev_null);


#endif //QUAD_EQUATION_SOLVER_H
