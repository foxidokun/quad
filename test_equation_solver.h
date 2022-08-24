#ifndef TEST_EQUATION_SOLVER_H
#define TEST_EQUATION_SOLVER_H

/**
 * @brief Run all tests
 * @param tmp_file          Temporary file (rw)
 * @param input_file        File with sample input (r)
 * @param lin_file          File with tests for solve_lin_eq
 * @param quad_file         File with test for solve_quad_eq
 * @param output_ref_file   File with sample output (w)
 * @param dev_null          /dev/null or analog
 * @param report_file       Report file or NULL if stdout
 */
void run_test(const char *tmp_file, const char *input_file, const char *lin_file, const char *quad_file,
              const char *output_ref_file, const char *dev_null, const char *report_file);

/**
 * @brief      Test solve_lin_eq
 *
 * @param      in_stream  The stream to read data from
 * @param  report_stream  The stream to write report to
 * 
 * @note Stream format: k b num_roots(int) x. Strings begins with '#' or empty lines are ignored
 * 
 * @return Non-zero value if test failed
 */
int manual_test_solve_lin_eq  (FILE *in_stream, FILE *report_stream);

/**
 * @brief      Test solve_quad_eq
 *
 * @param      in_stream  The stream to read data from
 * @param  report_stream  The stream to write report to
 * 
 * @note Stream format: a b c num_roots(int) x1 x2. Strings begins with '#' or empty lines are ignored
 * 
 * @return Non-zero value if test failed
 */
int manual_test_solve_quad_eq (FILE *in_stream, FILE *report_stream);

///@param in_stream Stream with sample input
///@param dev_null /dev/null stream
///@param  report_stream  The stream to write report to
///
///
///@return Non-zero value if failed
int manual_test_input_coeffs (FILE *in_stream, FILE *dev_null, FILE *report_stream);

/** @param tmp_file Temporary file
 * @param  ref_stream Stream with sample output
 * @param  report_stream  The stream to write report to
 * 
 * @return Non-zero value if test failed
 */
int manual_test_output_format (const char *tmp_file, FILE *ref_stream, FILE *report_stream);
 
/// @param  report_stream  The stream to write report to
/// @return Non-zero value if test failed
int auto_test_solve_lin_eq  (FILE *report_stream);

/// @param  report_stream  The stream to write report to
/// @return Non-zero value if test failed
int auto_test_solve_quad_eq (FILE *report_stream);

/// @param tmp_file Temporary file
/// @param dev_null /dev/null stream
/// @param report_stream  The stream to write report to
///
/// @return Non zero value if test failed
int auto_test_input_coeffs (const char *tmp_file, FILE *dev_null, FILE *report_stream);

/**
 * @brief       Run solve_quad_eq with given a, b, c and compare result with given n_roots, x1_ref (if >= ONE_ROOT) and x2_ref (if TWO_ROOTS)
 *
 * @param[in]  n_roots_ref  n_roots reference value
 * @param [in] a            Quadratic coefficient
 * @param [in] b            Linear coefficient
 * @param [in] c            Free coefficient
 * @param[in]  x1_ref       Reference x1 value
 * @param[in]  x2_ref       Reference x2 value
 * @param[in]  report_stream  The stream to write report to
 * 
 * @return Non-zero value if test failed
 */
int test_solve_quad_eq (num_roots n_roots_ref, double a, double b, double c,
                    double x1_ref, double x2_ref, FILE *report_stream);

/**
 * @brief      Run solve_lin_eq with given k, b and compare result with given n_roots and x_ref (if ONE_ROOT)
 *
 * @param[in]  n_roots_ref  n_roots reference value
 * @param[in]  k            Linear coefficient
 * @param[in]  b            Free coefficient
 * @param[in]  x_ref        Reference x value
 * @param[in]  report_stream  The stream to write report to
 * 
 * @return Non-zero value if test failed
 */
int test_solve_lin_eq (num_roots n_roots_ref, double k, double b, double x_ref, FILE *report_stream);

/**
 * @brief      Check if x is the solution of kx + b = 0
 *
 * @param[in]  k     Linear coefficient
 * @param[in]  b     Free coefficient
 * @param[in]  x     x
 * @param  report_stream  The stream to write report to
 * 
 * @return Non-zero value if test failed
 */
int check_solve_lin_eq (double k, double b, double x, FILE *report_stream);
/**
 * @brief      Check if x is the solution of ax^2 + bx + c = 0
 *
 * @param[in]  a     Quadratic coefficient
 * @param[in]  b     Linear coefficient
 * @param[in]  c     Free coefficient
 * @param[in]  x     x
 * @param  report_stream  The stream to write report to
 * 
 * @return Non-zero value if test failed
 */
int check_solve_quad_eq (double a, double b, double c, double x, FILE *report_stream);

#endif //TEST_EQUATION_SOLVER_H