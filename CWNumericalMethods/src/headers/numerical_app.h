
#ifndef _NUMERICAL_APP_H_
#define _NUMERICAL_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include <math.h>

/**
 * Функция вычислитель (указатель)
 */
typedef double (*Func)(double);

double f(double x);
double f2(double x);



#ifdef __cplusplus
}
#endif

#endif
