#ifndef _DRAW_PLOTS_H_
#define _DRAW_PLOTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include "numerical_app.h"
#include "math_parser.h"

// Количество графиков
#define NUMS_PLOTS      3

/**
 * Цвет RGB
 */
struct ColorDraw
{
    double r;
    double g;
    double b;
};

/**
 * Массив точек графика
 */
struct ArrayPointsXY
{
    size_t num_points;
    double *x;
    double *y;
    Func function;
};

/**
 * График функции
 */
struct PlotDrawFun
{
    struct ArrayPointsXY points;
    struct ColorDraw color;
    wchar_t *title;
};

/**
 * Настройки в JSON файле
 */
struct PlotProperties
{
    wchar_t *expr;
    double start_x;
    double end_x;
    size_t num_points;
    struct ColorDraw color;
};

/**
 * Настройки
 */
struct AppProperties
{
    struct PlotDrawFun *pdf;
    struct PlotProperties *plots_props_array;
    size_t size_prors_array;
};

/**
 * Создать график, сохранить в файл .png
 */
void draw_plots(struct PlotDrawFun plots[], size_t nums_draw_plots, const char *filename);

/**
 * Создать массив точек
 */
struct ArrayPointsXY calculate_function
(double start_x, double end_x, size_t num_points, wchar_t *title, Func function);

/**
 * Очистить память
 */
void free_array_points(struct AppProperties *ap, size_t size);

// ------------------ ПАРСИНГ JSON --------------------------------------------

// Функция чтения JSON файла в строку
char* read_JSON_file_to_string(const char *filename);

// Парсинг JSON
struct AppProperties parse_json(const char *filename);

// Собрать данные для отрисовки графиков
struct PlotDrawFun* make_properties(struct AppProperties *);

#ifdef __cplusplus
}
#endif

#endif