
#ifndef _NUMERICAL_APP_H_
#define _NUMERICAL_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include <math.h>
#include "math_parser.h"

struct PlotDrawFun;

/**
 * Функция вычислитель (указатель)
 */
typedef double (*Func)(wchar_t*, double);

/**
 * Ф-ция вычисления выражения math функций
 */
double calculate(wchar_t *expr, double x);

#ifdef __cplusplus
}
#endif

#endif
