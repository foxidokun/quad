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

#endif //TEST_EQUATION_SOLVER_H