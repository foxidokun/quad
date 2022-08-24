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
static bool is_zero (double x)
{
    return fabs(x) < DBL_ERROR;
}

/**@brief Solve linear equation, write root into given variable (x) and return number of solutions found
 *
 * @param [in] k Linear coefficient
 * @param [in] b Free coefficient
 * @param [out] x Variable to store equation root
 * @return Number of equation roots
 */
enum num_roots solve_lin_eq (double k, double b, double *x);

/**@brief Solve quadratic equation, write roots into given variables (x1&x2) and return number of solutions found or error
 *
 * Possible errors
 * 1. ERANGE_SOLVE -- overflow in internal calculations
 *
 * @note Overflow checks:
 * 1. b^2 < DOUBLE_MAX
 * 2. abs(4*a*c) < DOUBLE_MAX
 * 3. abs(b^2 - 4*a*c) < DOUBLE_MAX
 *
 * If there are less than two solutions, unused variables do not change their value.
 * If there is one solution, it is written in x1.
 * If there are two solutions, the order of x1 and x2 is not guaranteed.
 *
 * @param [in] a Quadratic coefficient
 * @param [in] b Linear coefficient
 * @param [in] c Free coefficient
 * @param [out] x1 Pointer to store equation root
 * @param [out] x2 Pointer to store equation root
 * @return Number of equation roots
 */
enum num_roots solve_quad_eq (double a, double b, double c, double *x1, double *x2);

///@brief Print solution to stream
void print_solution (enum num_roots n_roots, double roots[], FILE *stream);

/**
 * @brief Read coefficients from in_stream and write them to given variables using out_stream for asking question.
 * 
 * @note Order in coeffs: from biggest exponent (zero index) to lowest exponent (n_coeffs-1 index)
 *
 * In case of a read error, returns a non-zero value (errno value of the error)
 */
int input_coeffs (int n_coeffs, double coeffs[], FILE *in_stream, FILE *out_stream);

/**
 * @brief     Parse given strings into coefficients
 *
 * @param[in]  n_coeffs  Number of parsing coefficients
 * @param[out] coeffs    Array of coefficients: from biggest exponent (zero index) to lowest exponent (n_coeffs-1 index)
 * @param[in]  strings   n_coeffs strings to parse
 *
 * @return     Non zero value on parsing error
 */
int parse_coeffs (int n_coeffs, double coeffs[], char **strings);

#endif //QUAD_EQUATION_SOLVER_H
