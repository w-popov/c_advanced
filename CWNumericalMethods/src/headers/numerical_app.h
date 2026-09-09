
#ifndef _NUMERICAL_APP_H_
#define _NUMERICAL_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include <math.h>
#include "math_parser.h"

struct AppProperties;
struct PlotProperties;
struct Intersection_points;

/**
 * Функция вычислитель (указатель)
 */
typedef double (*Func)(struct PlotProperties *, double);

/**
 * Функция поиска пересечения графиков
 */
typedef double (*IntersectionFunc)(struct Intersection_points *, double);

/**
 * Вычисление интеграла
 */
typedef double (*IntegralFunc)(struct AppProperties *ap, double);

/**
 * Ф-ция вычисления выражения math функций f(x)
 */
double calculate_f(struct PlotProperties *pp, double x);

/**
 * Ф-ция вычисления выражения math функций F(x), т-е корней
 */
double calculate_F(struct Intersection_points *ip, double x);

// -------------- МЕТОД ДЕЛЕНИЯ ОТРЕЗКА ПОПОЛАМ ------------------------------

/**
 * @brief Найти корень
 */
double find_root_div_method (struct Intersection_points *ip, double eps1, IntersectionFunc F);

/**
 * @brief Метод деления отрезка пополам (поиск корня)
 */
int root(struct AppProperties *ap);

// ------------------------- ИНТЕГРАЛ ----------------------------------------

// Площадь на участке сверху f1, снизу f3
double integrand_segment1(struct AppProperties *ap, double x);

// На участке сверху f1, снизу f2
double integrand_segment2(struct AppProperties *ap, double x);

/**
 * Вычисление интеграла методом трапеций с фиксированным числом разбиений n
 */
double integrate_trapezoid
(struct AppProperties *ap, double eps2, double a, double b, IntegralFunc f);

/**
 * Вычисление интеграла 
 */
int integral(struct AppProperties *ap);

// =========================== ПРИЛОЖЕНИЕ ===================================



#ifdef __cplusplus
}
#endif

#endif
