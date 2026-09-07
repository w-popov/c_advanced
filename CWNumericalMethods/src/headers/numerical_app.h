
#ifndef _NUMERICAL_APP_H_
#define _NUMERICAL_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include <math.h>
#include "math_parser.h"

struct PlotProperties;

/**
 * Функция вычислитель (указатель)
 */
typedef double (*Func)(struct PlotProperties *, double);

/**
 * Ф-ция вычисления выражения math функций
 */
double calculate(struct PlotProperties *pp, double x);

#ifdef __cplusplus
}
#endif

#endif
