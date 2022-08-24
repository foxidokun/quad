#ifndef COMMON_EQUATION_SOLVER_H
#define COMMON_EQUATION_SOLVER_H

#include<math.h>

///@brief Floating point calculations accuracy
const double DBL_ERROR = 1e-11;

/**
 * @brief Compare double to zero, taking into account floating point calculation error
 */
static inline bool is_zero (double x)
{
    return fabs(x) < DBL_ERROR;
}

#endif