
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

/**
 * Функция вычислитель (указатель)
 */
typedef double (*Func)(struct PlotProperties *, double);

/**
 * Функция поиска пересечения графиков
 */
typedef double (*IntersectionFunc)(struct AppProperties *, double, Func);

/**
 * Ф-ция вычисления выражения math функций
 */
double calculate(struct PlotProperties *pp, double x);

// ------------- Функции F(x) = 0 для поиска точек пересечения ---------

/**
 * @brief Пересечение F1 и F3 (F1 - F3)
 * @param ap структура данных приложения
 * @param x аргумент
 * @param f математическая функция
*/ 
double intersection_F_1_3(struct AppProperties *ap, double x, Func f);

/**
 * @brief Пересечение F2 и F3 (F2 - F3)
 * @param ap структура данных приложения
 * @param x аргумент
 * @param f математическая функция
*/ 
double intersection_F_2_3(struct AppProperties *ap, double x, Func f);

/**
 * @brief Пересечение F1 и F2 (F1 - F2)
 * @param ap структура данных приложения
 * @param x аргумент
 * @param f математическая функция
*/ 
double intersection_F_1_2(struct AppProperties *ap, double x, Func f); 

// -------------- МЕТОД ДЕЛЕНИЯ ОТРЕЗКА ПОПОЛАМ ------------------------------

/**
 * @brief Найти корень
 */
double find_root_div_method
(struct AppProperties *ap, int root_id, IntersectionFunc F, Func f);

/**
 * @brief Метод деления отрезка пополам (поиск корня)
 */
int find_root(struct AppProperties *ap);


#ifdef __cplusplus
}
#endif

#endif
