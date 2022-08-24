#ifndef TEST_EQUATION_SOLVER_H
#define TEST_EQUATION_SOLVER_H

/**
 * @brief Run all tests
 * @param tmp_file Temporary file (rw)
 * @param input_file File with sample input (w)
 * @param output_ref_file File with sample output (r)
 * @param dev_null /dev/null or analog
 */
void run_test(const char *tmp_file, const char *input_file, const char *output_ref_file, const char *dev_null);

void manual_test_solve_lin_eq();
void manual_test_solve_quad_eq();

///@param input_file File with sample input
///@param dev_null /dev/null stream
void manual_test_input_coeffs(FILE *in_stream, FILE *dev_null);

/** @param tmp_file Temporary file
 * @param output_ref_file File with sample output
 */
void manual_test_output_format(const char *tmp_file, FILE *ref_stream);

void auto_test_solve_lin_eq();
void auto_test_solve_quad_eq();

///@param tmp_file Temporary file
///@param dev_null /dev/null stream
void auto_test_input_coeffs(const char *tmp_file, FILE *dev_null);

/**
 * @brief       Run solve_quad_eq with given a, b, c and compare result with given n_roots, x1_ref (if >= ONE_ROOT) and x2_ref (if TWO_ROOTS)
 *
 * @param[in]  n_roots_ref  n_roots reference value
 * @param [in] a            Quadratic coefficient
 * @param [in] b            Linear coefficient
 * @param [in] c            Free coefficient
 * @param[in]  x1_ref       Reference x1 value
 * @param[in]  x2_ref       Reference x2 value
 */
void test_solve_quad_eq(num_roots n_roots_ref, double a, double b, double c, double x1_ref, double x2_ref);

/**
 * @brief      Run solve_lin_eq with given k, b and compare result with given n_roots and x_ref (if ONE_ROOT)
 *
 * @param[in]  n_roots_ref  n_roots reference value
 * @param[in]  k            Linear coefficient
 * @param[in]  b            Free coefficient
 * @param[in]  x_ref        Reference x value
 */
void test_solve_lin_eq(num_roots n_roots_ref, double k, double b, double x_ref);

/**
 * @brief      Check if x is the solution of kx + b = 0
 *
 * @param[in]  k     Linear coefficient
 * @param[in]  b     Free coefficient
 * @param[in]  x     x
 */
void check_solve_lin_eq(double k, double b, double x);
/**
 * @brief      Check if x is the solution of ax^2 + bx + c = 0
 *
 * @param[in]  a     Quadratic coefficient
 * @param[in]  b     Linear coefficient
 * @param[in]  c     Free coefficient
 * @param[in]  x     x
 */
void check_solve_quad_eq(double a, double b, double c, double x);

#endif //TEST_EQUATION_SOLVER_H