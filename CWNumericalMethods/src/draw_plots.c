#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pbPlots.h"
#include "supportLib.h"
#include "draw_plots.h"

/**
 * @warning ВЫДЕЛЯЕТ ПАМЯТЬ
 * @brief Создать массив точек графика
 * @param current_x начать с этого значения
 * @param num_points количесво точек
 * @param function вычислитель 
 */
struct ArrayPointsXY calculate_function
(double current_x, size_t num_points, Func function)
{
    struct ArrayPointsXY arrpxy;
    double *ptx = (double*)malloc(num_points * sizeof(double));
    double *pty = (double*)malloc(num_points * sizeof(double));

    if (ptx == NULL || pty == NULL)
    {
        free(ptx);
        free(pty);
        perror("\nMemory allocation error in calculate_function() Exit...\n");
        exit(1);
    }

    for(size_t i = 0; i < num_points; i++) 
    {
        ptx[i] = current_x;
        pty[i] = function(current_x);
        current_x += 0.01;
    }

    arrpxy.x = ptx;
    arrpxy.y = pty;
    arrpxy.num_points = num_points;
    arrpxy.function = function;

    return arrpxy;
}

/**
 * Очистить память массивов точек
 */
void free_array_points(struct PlotDrawFun pdf[], size_t size)
{
    if (pdf)
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (pdf[i].points.x)
                free(pdf[i].points.x);
            if (pdf[i].points.y)
                free(pdf[i].points.y);
        }
    }
}

/**
 * @brief Создать график, сохранить в файл .png
 * @param PlotDrawFun массив структур графиков для отрисовки
 * @param nums_draw_plots количество графиков на холсте
 * @param filename имя выходного png файла
 */
void draw_plots(struct PlotDrawFun plots[], size_t nums_draw_plots, const char *filename)
{
    if (nums_draw_plots == 0 || plots == NULL) 
    {
        perror("\nAn empty array was passed!\n");
        return;
    }

    // Выделить память под массив указателей на ScatterPlotSeries
    ScatterPlotSeries **series_array = (ScatterPlotSeries **)malloc(nums_draw_plots * sizeof(ScatterPlotSeries *));
    
    if (series_array == NULL)
    {
        free(series_array);
        perror("\nMemory allocation error in draw_plots() Exit...\n");
        exit(2);
        return;
    } 

    // Создание серии графиков
    for (size_t i = 0; i < nums_draw_plots; ++i)
    {
        series_array[i] = GetDefaultScatterPlotSeriesSettings();
        series_array[i]->xs = plots[i].points.x;
        series_array[i]->xsLength = plots[i].points.num_points;
        series_array[i]->ys = plots[i].points.y;
        series_array[i]->ysLength = plots[i].points.num_points;
        series_array[i]->linearInterpolation = true;
        
        // тип линии
        series_array[i]->lineType = L"solid";
        series_array[i]->lineTypeLength = wcslen(series_array[i]->lineType);
        // толщина
        series_array[i]->lineThickness = 2;                          
        // цвет rgb
        series_array[i]->color = CreateRGBColor(plots[i].color.r, plots[i].color.g, plots[i].color.b);
    }

    // Общие настройки холста
    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 1024;
    settings->height = 780;
    settings->autoBoundaries = true;
    settings->autoPadding = true;
    settings->xLabel = L"X axis";
    settings->xLabelLength = wcslen(settings->xLabel); 
    settings->yLabel = L"Y axis";
    settings->yLabelLength = wcslen(settings->yLabel);
    settings->scatterPlotSeries = series_array;
    settings->scatterPlotSeriesLength = nums_draw_plots;

    // Выделение памяти под объект изображения и генерация
    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();
    StringReference str_ref_err;
    str_ref_err.string = L"";

    // Отрисовка графика со всеми настройками и текстом
    int success = DrawScatterPlotFromSettings(imageReference, settings, &str_ref_err);

    // Сохранение в файл, если отрисовка успешна
    if (success) 
    {
        double heigt_title = 25.;
        for (size_t i = 0; i < nums_draw_plots; ++i, heigt_title += 20)
        {
            // Отрисовка текста заголовка цветом графика 
            RGBA *color_title = CreateRGBColor(plots[i].color.r, plots[i].color.g, plots[i].color.b);
            DrawText(imageReference->image, 100, heigt_title, plots[i].title, wcslen(plots[i].title), color_title);
        }
        wchar_t *sign = L"cource work 2 (C adv) Popov V.G";
        DrawText(imageReference->image, 600, 25, sign, wcslen(sign), CreateRGBColor(0, 0, 0));
        // Запись в файл
        ByteArray *pngdata = ConvertToPNG(imageReference->image);
        const char* actual_filename = (filename == NULL) ? "plot.png" : filename;
        WriteToFile(pngdata, (char *)actual_filename);

        // Открыть файл png программой просмотра изображений
        char command[512];
        #if defined(_WIN32) || defined(_WIN64)
            snprintf(command, sizeof(command), "start %s", actual_filename);
        #else
            snprintf(command, sizeof(command), "xdg-open %s", actual_filename);
        #endif
        system(command);
    } 
    else 
    {
        printf("\nError saving to file in draw_plots()\n");
    }

    // Освобождение выделенной памяти библиотеки
    free(series_array);
    FreeAllocations();    
}
