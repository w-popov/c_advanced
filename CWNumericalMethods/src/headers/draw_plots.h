#ifndef _DRAW_PLOTS_H_
#define _DRAW_PLOTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include "numerical_app.h"

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
 * Создать график, сохранить в файл .png
 */
void draw_plots(struct PlotDrawFun plots[], size_t nums_draw_plots, const char *filename);

/**
 * Создать массив точек
 */
struct ArrayPointsXY calculate_function
(double current_x, size_t num_points, Func function);

/**
 * Очистить память массивов точек
 */
void free_array_points(struct PlotDrawFun pdf[], size_t size);

#ifdef __cplusplus
}
#endif

#endif