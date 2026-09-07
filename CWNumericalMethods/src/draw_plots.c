#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "pbPlots.h"
#include "supportLib.h"
#include "cJSON.h"
#include "draw_plots.h"

/**
 * @warning ВЫДЕЛЯЕТ ПАМЯТЬ
 * @brief Создать массив точек графика
 * @param start_x начать с этого значения
 * @param end_x до этого
 * @param num_points количесво точек
 * @param function вычислитель 
 */
struct ArrayPointsXY calculate_function(struct PlotProperties *pp, Func function)
{
    struct ArrayPointsXY arrpxy;

    double *ptx = (double*)malloc(pp->num_points * sizeof(double));
    double *pty = (double*)malloc(pp->num_points * sizeof(double));

    if (ptx == NULL || pty == NULL) 
    {
        free(ptx); free(pty);
        perror("\nMemory allocation error\n");
        exit(1);
    }
    
    // Вычисление шага
    double range = pp->end_x - pp->start_x;
    size_t last_index = pp->num_points - 1;

    for (size_t i = 0; i <= last_index; ++i) 
    {
        // вычисление координаты x
        double current_x = pp->start_x + (range * (double)i) / (double)last_index;
        
        ptx[i] = current_x;
        pty[i] = function(pp, current_x);
    }

    arrpxy.x = ptx;
    arrpxy.y = pty;
    arrpxy.num_points = pp->num_points;

    return arrpxy;
}

/**
 * Очистить память
 */
void free_array_points(struct AppProperties *ap, size_t size)
{
    if (ap)
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (ap->pdf[i].points.x)
                free(ap->pdf[i].points.x);
            if (ap->pdf[i].points.y)
                free(ap->pdf[i].points.y);
            free(ap->plots_props_array[i].expr);
        }
        free(ap->plots_props_array);
        free(ap->pdf);
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
        series_array[i]->linearInterpolation = 0;   //---------------> НЕ ВКЛЮЧАТЬ!
                               
        // цвет rgb
        series_array[i]->color = CreateRGBColor(plots[i].color.r, plots[i].color.g, plots[i].color.b);
    }

    // Общие настройки холста
    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 1280;
    settings->height = 860;
    settings->autoBoundaries = 0;
    settings->autoPadding = 1;
    /* !Значения ограничений должны быть не менее переданных
     в calculate_function(start_x, end_x, ...) */
    settings->xMin = -6.0;
    settings->xMax = 6.0;
    settings->yMin = -10.0; // от -10
    settings->yMax = 10.0;  // до 10 по вертикали

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
        double heigt_title = 15.;
        for (size_t i = 0; i < nums_draw_plots; ++i, heigt_title += 20)
        {
            // Отрисовка текста заголовка цветом графика 
            RGBA *color_title = CreateRGBColor(plots[i].color.r, plots[i].color.g, plots[i].color.b);
            DrawText(imageReference->image, 180, heigt_title, plots[i].title, wcslen(plots[i].title), color_title);
        }
        wchar_t *sign = L"cource work 2 (C adv) Popov V.G";
        DrawText(imageReference->image, 600, 15., sign, wcslen(sign), CreateRGBColor(0, 0, 0));

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

// ------------- JSON ----------------------------------------

/**
 * @brief Функция чтения JSON файла в строку
 * @warning ВЫДЕЛЯЕТ ПАМЯТЬ!
 */
char* read_JSON_file_to_string(const char *json_filename)
{
    FILE *file = fopen(json_filename, "rb");
    if (!file) 
    {
        fprintf(stderr, "Failed to open file: %s\n", json_filename);
        return NULL;
    }

    // размер файла
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char*)malloc(length + 1);
    if (!buffer) 
    {
        fprintf(stderr, "Error allocating memory for file buffer\n");
        fclose(file);
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, length, file);
    buffer[read_bytes] = '\0';
    fclose(file);

    return buffer;
}

/**
 * @brief Парсинг JSON
 * @warning ВЫДЕЛЯЕТ ПАМЯТЬ!
 */
struct AppProperties parse_json(const char *filename_json)
{
    char *json_data = read_JSON_file_to_string(filename_json);
    if (!json_data)
    {
        fprintf(stderr, "Failed to open file: %s\n", filename_json);
        exit(4);
    }

    // парсинг строки
    cJSON *root = cJSON_Parse(json_data);
    free(json_data);

    if (!root) 
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        fprintf(stderr, "JSON parsing error");
        if (error_ptr) 
            fprintf(stderr, "near: %s", error_ptr);
        fprintf(stderr, "\n");
        exit(5);
    }

    // Получение массива "inputs"
    cJSON *inputs = cJSON_GetObjectItemCaseSensitive(root, "inputs");
    if (!cJSON_IsArray(inputs)) 
    {
        fprintf(stderr, "Error: 'inputs' not found or is not an array\n");
        cJSON_Delete(root);
        exit(6);
    }

    // Проход по элементам массива "inputs"
    // Размер массива
    int array_size = cJSON_GetArraySize(inputs);

    // Выходная структура
    struct AppProperties app_props;
    app_props.plots_props_array = (struct PlotProperties*)malloc(sizeof(struct PlotProperties) * array_size);

    if (!app_props.plots_props_array)
    {
        fprintf(stderr, "Error: alloc memory in parse_json() for plots_props_array\n");
        cJSON_Delete(root);
        exit(7);
    }
    app_props.size_prors_array = (size_t)array_size;

    for (int i = 0; i < array_size; ++i) 
    {
        cJSON *item = cJSON_GetArrayItem(inputs, i);

        if (!item) 
            continue;

        // Извлечь поле "expr" - выражение для вычисления
        // Преобразование из char* в wchar_t*
        cJSON *expr = cJSON_GetObjectItemCaseSensitive(item, "expr");
        if (cJSON_IsString(expr) && expr->valuestring) 
        {
            // сколько места нужно для широкой строки
            size_t wlen = mbstowcs(NULL, expr->valuestring, 0);
            if (wlen == (size_t)-1) 
            {
                fprintf(stderr, "Error: Failed to estimate wide string length\n");
                exit(EXIT_FAILURE);
            }
            wchar_t *wstr = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
            if (!wstr) 
            {
                fprintf(stderr, "Error: Memory allocation failed for wide string\n");
                exit(EXIT_FAILURE);
            }
            mbstowcs(wstr, expr->valuestring, wlen + 1);
            app_props.plots_props_array[i].expr = wstr;
        }

        // Извлечь границы x
        cJSON *start_x = cJSON_GetObjectItemCaseSensitive(item, "start_x");
        cJSON *end_x = cJSON_GetObjectItemCaseSensitive(item, "end_x");
        if (cJSON_IsNumber(start_x) && cJSON_IsNumber(end_x)) 
        {
            app_props.plots_props_array[i].start_x = start_x->valuedouble;
            app_props.plots_props_array[i].end_x = end_x->valuedouble; 
        }

        // Извлечь количество точек
        cJSON *num_points = cJSON_GetObjectItemCaseSensitive(item, "num_points");
        if (cJSON_IsNumber(num_points))
        {
            app_props.plots_props_array[i].num_points = (size_t)num_points->valuedouble;
        }

        // Извлечь вложенный объект цвета
        cJSON *color = cJSON_GetObjectItemCaseSensitive(item, "color");
        if (cJSON_IsObject(color))
        {
            cJSON *r = cJSON_GetObjectItemCaseSensitive(color, "r");
            cJSON *g = cJSON_GetObjectItemCaseSensitive(color, "g");
            cJSON *b = cJSON_GetObjectItemCaseSensitive(color, "b");

            if (cJSON_IsNumber(r) && cJSON_IsNumber(g) && cJSON_IsNumber(b))
            {
                app_props.plots_props_array[i].color.r = r->valuedouble;
                app_props.plots_props_array[i].color.g = g->valuedouble;
                app_props.plots_props_array[i].color.b = b->valuedouble;
            }
        }
    }
    cJSON_Delete(root);

    return app_props;
}


/**
 * @brief Собрать данные для отрисовки графиков
 * @warning ВЫДЕЛЯЕТ ПАМЯТЬ!
 * @param props Структура (указатель) из распарсенного json файла настроек
 */
struct PlotDrawFun* make_properties(struct AppProperties *props)
{
    if (props == NULL)
    {
        fprintf(stderr, "Error: Zero NULL pointer in make_properties()\n");
        exit(7);
    }

    struct PlotDrawFun *pdf = (struct PlotDrawFun*)malloc(sizeof(struct PlotDrawFun) * props->size_prors_array);
    for (size_t i = 0; i < props->size_prors_array; ++i)
    {
        pdf[i].title = props->plots_props_array[i].expr;
        pdf[i].color = props->plots_props_array[i].color;
        // компилировать текстовые формулы в rpn один раз для последующих вычислений
        size_t rpn_cnt = compile_to_rpn(pdf[i].title, props->plots_props_array[i].rpn);
        props->plots_props_array[i].rpn_count = rpn_cnt;

        pdf[i].points = calculate_function(&(props->plots_props_array[i]), calculate);
    }

    return pdf;
}
