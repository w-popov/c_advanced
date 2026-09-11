
#ifndef _NUMERICAL_APP_H_
#define _NUMERICAL_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include <math.h>
#include "math_parser.h"

// Цвета текста консоли
#define CRED     L"\x1b[31m"
#define CGREEN   L"\x1b[32m"
#define CBLUE    L"\x1b[36m"
#define CRESET   L"\x1b[0m"

// Размер массива площадей интеграла
#define MAX_SQUARES_INTEGRAL    64

struct AppProperties;
struct PlotProperties;
struct Intersection_points;
struct IntegralProperties;

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
typedef double (*IntegralFunc)(struct IntegralProperties *, double);

/**
 * Ф-ция вычисления выражения math функций f(x)
 */
double calculate_f(struct PlotProperties *pp, double x);

/**
 * Ф-ция вычисления выражения math функций F(x), т-е корней
 */
double calculate_F(struct Intersection_points *ip, double x);

/**
 * Вычисление выражения для интеграла
 */
double calculate_S(struct IntegralProperties *ip, double x);

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

/**
 * Вычисление интеграла методом трапеций с фиксированным числом разбиений n
 */
double integrate_trapezoid
(struct AppProperties *ap, struct IntegralProperties *ip, double a, double b, IntegralFunc f);

/**
 * Вычисление интеграла 
 */
int integral(struct AppProperties *ap);

// =========================== ПРИЛОЖЕНИЕ ===================================

// Справка
void show_help(void);

/**
 * @brief Тест поиска корня методом деления отрезка пополам
 */
void test_root(struct AppProperties *ap);

/**
 * @brief Тест интеграла
 */
void test_integral(struct AppProperties *ap);

/**
 * @brief Единая точка запуска тестов. 
 */
void run_all_tests(struct AppProperties *ap);

#ifdef __cplusplus
}
#endif

#endif
