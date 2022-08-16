#ifndef QUAD_EQUATION_SOLVER_H
#define QUAD_EQUATION_SOLVER_H

/// Количество корней у уравнения
enum num_roots {
    TWO_ROOTS = 2,
    ONE_ROOT = 1,
    ZERO_ROOTS = 0,
    INF_ROOTS = -1
};

enum num_roots solve_lin_eq(double k, double b, double *x);
enum num_roots solve_quad_eq(double a, double b, double c, double *x1, double *x2);
void print_solution(enum num_roots n_roots, double x1, double x2);

#endif //QUAD_EQUATION_SOLVER_H
