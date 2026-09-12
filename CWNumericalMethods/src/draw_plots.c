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
 * @param pp структура (указатель) с данными для отрисовки
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
 * Очистить динамическую память приложения
 */
void free_array_points(struct AppProperties *ap, size_t size)
{
    if (ap)
    {
        for (size_t i = 0; i < size; ++i)
        {
            free(ap->pdf[i].points.x);
            free(ap->pdf[i].points.y);
            free(ap->plots_props_array[i].expr);
        }
        for (size_t i = 0; i < ap->size_arr_inters_points; ++i)
        {
            free(ap->inters_points[i].root_expr);
        }
        for (size_t i = 0; i < ap->size_integral_array; ++i)
        {
            free(ap->integral_props_array[i].expr_s);
        }
        free(ap->plots_props_array);
        free(ap->pdf);
        free(ap->inters_points);
        free(ap->integral_props_array);
    }
    // Освобождение выделенной памяти библиотеки pbPlots
    FreeAllocations();  
}

/**
 * @brief Создать график, сохранить в файл .png
 * @param ap главная структура (указатель) приложения
 * @param nums_draw_plots количество графиков на холсте
 * @param filename имя выходного png файла
 */
void draw_plots(struct AppProperties *ap, size_t nums_draw_plots, const char *filename)
{
    struct PlotDrawFun *plots = ap->pdf;
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
        wchar_t title_buffer[128];

        for (size_t i = 0; i < nums_draw_plots; ++i, heigt_title += 20)
        {
            // Отрисовка текста заголовка цветом графика 
            RGBA *color_title = CreateRGBColor(plots[i].color.r, plots[i].color.g, plots[i].color.b);
            swprintf(title_buffer, 128, L"f%d = %ls", plots[i].fun_number, plots[i].title);
            DrawText(imageReference->image, 180, heigt_title, title_buffer, wcslen(title_buffer), color_title);
        }
        wchar_t *sign = L"cource work 2 (C adv) Popov V.G";
        DrawText(imageReference->image, 600, 15., sign, wcslen(sign), CreateRGBColor(0, 0, 0));

        // Отрисовка линий точек пересечения
        double ladder_label = 20.;
        for (size_t i = 0; i < ap->size_arr_inters_points; ++i)
        {
            // Если корни вычислены то показать линиями точки пересечения
            if (!isnan(ap->inters_points[i].root))
            {
                ladder_label = i % 2 ? 20 : 0;
                double math_x = ap->inters_points[i].root;
                double pixel_x = MapXCoordinateBasedOnSettings(math_x, settings);
                double pixel_y_bottom;
                double pixel_y_top;

                if (settings->autoBoundaries) 
                {
                    pixel_y_bottom = settings->height - 30.0;
                    pixel_y_top = 30.0; 
                } 
                else 
                {
                    pixel_y_bottom = MapYCoordinateBasedOnSettings(0.0, settings);
                    pixel_y_top    = MapYCoordinateBasedOnSettings(settings->yMax, settings);
                }

                RGBA *line_color = CreateRGBColor(0.1, 0.1, 0.1);
                double thickness = 1.5;

                DrawLine(imageReference->image, pixel_x, pixel_y_bottom, pixel_x, pixel_y_top, thickness, line_color);

                RGBA *point_color = CreateRGBColor(1.0, 0.0, 0.0);
                double radius = 3.0;
                DrawCircle(imageReference->image, pixel_x, pixel_y_bottom, radius, point_color);

                wchar_t root_buffer[32];
                swprintf(root_buffer, 32, L"xF%d=%.4f", ap->inters_points[i].id_F, math_x);
                DrawText(imageReference->image, pixel_x - 35.0, pixel_y_bottom + 25 + ladder_label, root_buffer, wcslen(root_buffer), line_color);
            }
        }

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
      
    free(series_array);
}

// ------------- JSON ----------------------------------------

/**
 * @brief Функция чтения JSON файла в строку
 * @warning ВЫДЕЛЯЕТ ПАМЯТЬ!
 */
char* read_JSON_file_to_string(const char *json_filename)
{
    // Открыть файл json. Если его нет, создать и открыть
    FILE *file = fopen(json_filename, "rb");
    if (!file) 
    {
        int status = create_properties_json(json_filename);
        if (!status)
        {
            fprintf(stderr, "Failed to open file: %s\n", json_filename);
            return NULL;
        }
        file = fopen(json_filename, "rb");
    }
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

// Функция для генерации JSON файла настроек
int create_properties_json(const char *filename) 
{
    // корневой объект {}
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
        return 0;

    // массив "inputs" []
    cJSON *inputs = cJSON_AddArrayToObject(root, "inputs");
    if (inputs == NULL) 
    {
        cJSON_Delete(root);
        return 0;
    }

    // График F1
    cJSON *graph1 = cJSON_CreateObject();
    cJSON_AddStringToObject(graph1, "expr", "0.6 * x + 3");
    cJSON_AddNumberToObject(graph1, "f", 1);
    cJSON_AddNumberToObject(graph1, "start_x", -6.0);
    cJSON_AddNumberToObject(graph1, "end_x", 6.0);
    cJSON_AddNumberToObject(graph1, "num_points", 10000);
    
    cJSON *color1 = cJSON_CreateObject();
    cJSON_AddNumberToObject(color1, "r", 0.8);
    cJSON_AddNumberToObject(color1, "g", 0.0);
    cJSON_AddNumberToObject(color1, "b", 0.0);
    cJSON_AddItemToObject(graph1, "color", color1);
    
    cJSON_AddItemToArray(inputs, graph1);

    // График F2
    cJSON *graph2 = cJSON_CreateObject();
    cJSON_AddStringToObject(graph2, "expr", "(x - 2) ^ 3 - 1");
    cJSON_AddNumberToObject(graph2, "f", 2);
    cJSON_AddNumberToObject(graph2, "start_x", -6.0);
    cJSON_AddNumberToObject(graph2, "end_x", 6.0);
    cJSON_AddNumberToObject(graph2, "num_points", 10000);
    
    cJSON *color2 = cJSON_CreateObject();
    cJSON_AddNumberToObject(color2, "r", 0.0);
    cJSON_AddNumberToObject(color2, "g", 0.7);
    cJSON_AddNumberToObject(color2, "b", 0.0);
    cJSON_AddItemToObject(graph2, "color", color2);
    
    cJSON_AddItemToArray(inputs, graph2);

    // График F3
    cJSON *graph3 = cJSON_CreateObject();
    cJSON_AddStringToObject(graph3, "expr", "3 / x");
    cJSON_AddNumberToObject(graph3, "f", 3);
    cJSON_AddNumberToObject(graph3, "start_x", -6.0);
    cJSON_AddNumberToObject(graph3, "end_x", 6.0);
    cJSON_AddNumberToObject(graph3, "num_points", 10000);
    
    cJSON *color3 = cJSON_CreateObject();
    cJSON_AddNumberToObject(color3, "r", 0.0);
    cJSON_AddNumberToObject(color3, "g", 0.6);
    cJSON_AddNumberToObject(color3, "b", 1.0);
    cJSON_AddItemToObject(graph3, "color", color3);
    
    cJSON_AddItemToArray(inputs, graph3);

    // Создание массива "roots"
    cJSON *roots = cJSON_AddArrayToObject(root, "roots");
    if (roots == NULL) 
    {
        cJSON_Delete(root);
        return 0;
    }

    // Корень #1
    cJSON *root1 = cJSON_CreateObject();
    cJSON_AddStringToObject(root1, "root_expr", "0.6 * x + 3 - 3 / x");
    cJSON_AddNumberToObject(root1, "F", 1);
    cJSON_AddNumberToObject(root1, "a", 0.5);
    cJSON_AddNumberToObject(root1, "b", 1.5);
    cJSON_AddItemToArray(roots, root1);

    // Корень #2
    cJSON *root2 = cJSON_CreateObject();
    cJSON_AddStringToObject(root2, "root_expr", "(x - 2) ^ 3 - 1 - 3 / x");
    cJSON_AddNumberToObject(root2, "F", 2);
    cJSON_AddNumberToObject(root2, "a", 3.0);
    cJSON_AddNumberToObject(root2, "b", 4.0);
    cJSON_AddItemToArray(roots, root2);

    // Корень #3
    cJSON *root3 = cJSON_CreateObject();
    cJSON_AddStringToObject(root3, "root_expr", "0.6 * x + 3 - (x - 2) ^ 3 + 1");
    cJSON_AddNumberToObject(root3, "F", 3);
    cJSON_AddNumberToObject(root3, "a", 3.5);
    cJSON_AddNumberToObject(root3, "b", 4.5);
    cJSON_AddItemToArray(roots, root3);

    // Создание массива "integral" []
    cJSON *integral = cJSON_AddArrayToObject(root, "integral");
    if (integral == NULL)
    {
        cJSON_Delete(root);
        return 0;
    }

    // Интеграл #1
    cJSON *int1 = cJSON_CreateObject();
    cJSON_AddStringToObject(int1, "expr_S", "0.6 * x + 3 - 3 / x");
    cJSON_AddNumberToObject(int1, "from_F", 1);
    cJSON_AddNumberToObject(int1, "to_F", 2);
    cJSON_AddItemToArray(integral, int1);

    // Интеграл #2
    cJSON *int2 = cJSON_CreateObject();
    cJSON_AddStringToObject(int2, "expr_S", "0.6 * x + 3 - (x - 2) ^ 3 + 1");
    cJSON_AddNumberToObject(int2, "from_F", 2);
    cJSON_AddNumberToObject(int2, "to_F", 3);
    cJSON_AddItemToArray(integral, int2);

    // Параметры точности Eps1 и Eps2 в корень
    cJSON_AddNumberToObject(root, "Eps1", 0.00001);
    cJSON_AddNumberToObject(root, "Eps2", 0.00001);

    // Перевод JSON в текст
    char *json_string = cJSON_Print(root);
    if (json_string == NULL)
    {
        cJSON_Delete(root);
        return 0;
    }

    // Запись
    FILE *file = fopen(filename, "w");
    if (file == NULL) 
    {
        free(json_string);
        cJSON_Delete(root);
        return 0;
    }

    fprintf(file, "%s", json_string);
    fclose(file);
    free(json_string);
    cJSON_Delete(root);

    return 1;
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

    // Получение глобальных параметров Eps1 и Eps2
    cJSON *eps1_item = cJSON_GetObjectItemCaseSensitive(root, "Eps1");
    if (cJSON_IsNumber(eps1_item)) 
        app_props.eps_1 = eps1_item->valuedouble;

    cJSON *eps2_item = cJSON_GetObjectItemCaseSensitive(root, "Eps2");
    if (cJSON_IsNumber(eps2_item)) 
        app_props.eps_2 = eps2_item->valuedouble;
    
    // Проход по элементам json массива "inputs"
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

        // Идентификатор функции f (f1, f2 итд)
        cJSON *f = cJSON_GetObjectItemCaseSensitive(item, "f");
        app_props.plots_props_array[i].f = cJSON_IsNumber(f) ? (int)f->valuedouble : 0;

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

    // Извлечь массив "roots"
    cJSON *roots = cJSON_GetObjectItemCaseSensitive(root, "roots");
    if (cJSON_IsArray(roots)) 
    {
        int roots_size = cJSON_GetArraySize(roots);
        app_props.inters_points = (struct Intersection_points*)malloc(sizeof(struct Intersection_points) * roots_size);
        if (!app_props.inters_points)
        {
            fprintf(stderr, "Error: alloc memory for inters_points\n");
            cJSON_Delete(root);
            exit(8);
        }
        app_props.size_arr_inters_points = (size_t)roots_size;

        for (int i = 0; i < roots_size; ++i) 
        {
            cJSON *item = cJSON_GetArrayItem(roots, i);
            if (!item) continue;

            struct Intersection_points *curr_root = &app_props.inters_points[i];

            curr_root->root = NAN;

            // Конвертация "root_expr" char* -> wchar_t* (MALLOC)
            cJSON *root_expr = cJSON_GetObjectItemCaseSensitive(item, "root_expr");
            if (cJSON_IsString(root_expr) && root_expr->valuestring) 
            {
                size_t wlen = mbstowcs(NULL, root_expr->valuestring, 0);
                curr_root->root_expr = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
                if (curr_root->root_expr)
                {
                    mbstowcs(curr_root->root_expr, root_expr->valuestring, wlen + 1);
                }
            } 
            else 
            {
                curr_root->root_expr = NULL;
            }

            // Поле "F"
            cJSON *F = cJSON_GetObjectItemCaseSensitive(item, "F");
            curr_root->id_F = cJSON_IsNumber(F) ? (int)F->valuedouble : 0;

            // Чтение интервалов поиска "a" и "b" (с проверкой на null/число)
            cJSON *a_item = cJSON_GetObjectItemCaseSensitive(item, "a");
            curr_root->a = cJSON_IsNumber(a_item) ? a_item->valuedouble : NAN;

            cJSON *b_item = cJSON_GetObjectItemCaseSensitive(item, "b");
            curr_root->b = cJSON_IsNumber(b_item) ? b_item->valuedouble : NAN;
        }
    }

    // Извлечь массив "integral"
    cJSON *integral = cJSON_GetObjectItemCaseSensitive(root, "integral");
    if (cJSON_IsArray(integral)) 
    {
        int integral_size = cJSON_GetArraySize(integral);
        app_props.integral_props_array = (struct IntegralProperties*)malloc(sizeof(struct IntegralProperties) * integral_size);
        if (!app_props.integral_props_array) {
            fprintf(stderr, "Error: alloc memory for integral_props_array\n");
            cJSON_Delete(root);
            exit(9);
        }
        app_props.size_integral_array = (size_t)integral_size;

        for (int i = 0; i < integral_size; ++i) 
        {
            cJSON *item = cJSON_GetArrayItem(integral, i);
            if (!item) continue;
            struct IntegralProperties *curr_int = &app_props.integral_props_array[i];

            // Конвертация строки выражения "expr_S" char* -> wchar_t*
            cJSON *expr_s = cJSON_GetObjectItemCaseSensitive(item, "expr_S");
            if (cJSON_IsString(expr_s) && expr_s->valuestring) 
            {
                size_t wlen = mbstowcs(NULL, expr_s->valuestring, 0);
                curr_int->expr_s = (wchar_t*)malloc((wlen + 1) * sizeof(wchar_t));
                if (curr_int->expr_s)
                {
                    mbstowcs(curr_int->expr_s, expr_s->valuestring, wlen + 1);
                }
            } 
            else 
            {
                curr_int->expr_s = NULL;
            }

            // Целочисленные ключи
            cJSON *from_f = cJSON_GetObjectItemCaseSensitive(item, "from_F");
            curr_int->from_f = cJSON_IsNumber(from_f) ? (int)from_f->valuedouble : 0;

            cJSON *to_f = cJSON_GetObjectItemCaseSensitive(item, "to_F");
            curr_int->to_f = cJSON_IsNumber(to_f) ? (int)to_f->valuedouble : 0;
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
        pdf[i].fun_number = props->plots_props_array[i].f;
        // компилировать текстовые формулы в rpn один раз для последующих вычислений
        size_t rpn_cnt = compile_to_rpn(pdf[i].title, props->plots_props_array[i].rpn);
        props->plots_props_array[i].rpn_count = rpn_cnt;

        pdf[i].points = calculate_function(&(props->plots_props_array[i]), calculate_f);
    }

    return pdf;
}
