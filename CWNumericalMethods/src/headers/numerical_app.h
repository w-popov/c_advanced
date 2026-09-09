
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
double find_root_div_method (struct Intersection_points *ip, double eps, IntersectionFunc F);

/**
 * @brief Метод деления отрезка пополам (поиск корня)
 */
int find_root(struct AppProperties *ap);


#ifdef __cplusplus
}
#endif

#endif
