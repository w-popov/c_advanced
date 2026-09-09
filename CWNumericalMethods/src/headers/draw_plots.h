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
};

/**
 * Данные о корне
 */
struct Intersection_points
{
    double a;
    double b;
    double root;
    int id_F;
    wchar_t *root_expr;
    size_t nums_steps;
    size_t rpn_count;
    struct Token rpn[MAX_TOKENS];
};

/**
 * График функции
 */
struct PlotDrawFun
{
    struct ArrayPointsXY points;
    struct ColorDraw color;
    wchar_t *title;
    int fun_number;
};

/**
 * Настройки в JSON файле
 * (Для вычислений)
 */
struct PlotProperties
{
    int f;
    wchar_t *expr;
    double start_x;
    double end_x;
    double intersection_point;
    size_t num_points;
    size_t rpn_count;
    struct ColorDraw color;
    struct Token rpn[MAX_TOKENS];
};

/**
 * Настройки
 */
struct AppProperties
{
    struct PlotDrawFun *pdf;
    struct PlotProperties *plots_props_array;
    struct Intersection_points *inters_points;
    size_t size_prors_array;
    size_t size_arr_inters_points;
    double eps_1;
    double eps_2;
};

/**
 * Создать график, сохранить в файл .png
 */
void draw_plots(struct AppProperties *ap, size_t nums_draw_plots, const char *filename);

/**
 * Создать массив точек
 */
struct ArrayPointsXY calculate_function(struct PlotProperties *pp, Func function);

/**
 * Очистить память
 */
void free_array_points(struct AppProperties *ap, size_t size);

// ------------------ ПАРСИНГ JSON --------------------------------------------

// Функция чтения JSON файла в строку
char* read_JSON_file_to_string(const char *filename);

// Функция для генерации JSON файла настроек
int create_properties_json(const char *filename);

// Парсинг JSON
struct AppProperties parse_json(const char *filename);

// Собрать данные для отрисовки графиков
struct PlotDrawFun* make_properties(struct AppProperties *);

#ifdef __cplusplus
}
#endif

#endif