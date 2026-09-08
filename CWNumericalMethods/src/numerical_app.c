#include "numerical_app.h"
#include "draw_plots.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Ф-ция вычисления выражения math функций
 */
double calculate(struct PlotProperties *pp, double x)
{    
    return eval_rpn(pp->rpn, pp->rpn_count, x);
}

// ------------------------------------------------------------

/**
 * @brief Пересечение F1 и F3 (F1 - F3)
 * @param ap структура данных приложения
 * @param x аргумент
 * @param f математическая функция
*/ 
double intersection_F_1_3(struct AppProperties *ap, double x, Func f)
{
    return f(&ap->plots_props_array[0], x) - f(&ap->plots_props_array[2], x);
}

/**
 * @brief Пересечение F2 и F3 (F2 - F3)
 * @param ap структура данных приложения
 * @param x аргумент
 * @param f математическая функция
*/ 
double intersection_F_2_3(struct AppProperties *ap, double x, Func f)
{
    return f(&ap->plots_props_array[1], x) - f(&ap->plots_props_array[2], x); 
}

/**
 * @brief Пересечение F1 и F2 (F1 - F2)
 * @param ap структура данных приложения
 * @param x аргумент
 * @param f математическая функция
*/ 
double intersection_F_1_2(struct AppProperties *ap, double x, Func f)
{
    return f(&ap->plots_props_array[0], x) - f(&ap->plots_props_array[1], x);
}

/**
 * @brief Найти корень
 */
double find_root_div_method
(struct AppProperties *ap, int index, IntersectionFunc F, Func f) 
{
    int root_id = index - 1;
    double c;
    size_t step_count = 0; // Число шагов
    double xl = ap->inters_points[root_id].xl;
    double xr = ap->inters_points[root_id].xr;

    if (F(ap, xl, f) * F(ap, xr, f) > 0) 
    {
        wprintf(L"Ошибка: на интервале [%.2f, %.2f] нет корня.\n", xl, xr);
        return (xl + xr) / 2.0; 
    }

    while ((xr - xl) / 2.0 > ap->eps_1) 
    {
        step_count++;
        c = (xl + xr) / 2.0;
        
        // Если корень найден
        if (fabs(F(ap, c, f)) < 1e-15) 
        {
            ap->inters_points[root_id].nums_steps = step_count;
            wprintf(L"Корень найден точно за %zu шагов.\n", step_count);
            return c;
        }
        
        // Сужение интервала
        if (F(ap, xl, f) * F(ap, c, f) < 0) 
        {
            xr = c;
        } 
        else 
        {
            xl = c;
        }
    }

    ap->inters_points[root_id].nums_steps = step_count;
    wprintf(L"Корень найден с точностью %e за %zu шагов.\n", ap->eps_1, step_count);

    return (xl + xr) / 2.0;

}

/**
 * @brief Метод деления отрезка пополам (поиск корня)
 */
int find_root(struct AppProperties *ap)
{
    if (ap == NULL)
    {
        perror("\nError in find_root()! NULL pointer *ap\n");
        return 0;
    }
    if (ap->size_arr_inters_points != 3)
    {
        wprintf(L"\nОшибка! Кол-во диапазовнов поиска точек пересечения не равно 3!\nУкажите диапазоны xl, xr в файле properties.json, массив 'intersection_points'\n");
        return 0;
    }
    double xF1 = find_root_div_method(ap, 1, intersection_F_1_3, calculate);
    double xF2 = find_root_div_method(ap, 2, intersection_F_2_3, calculate);
    double xF3 = find_root_div_method(ap, 3, intersection_F_1_2, calculate);
    ap->inters_points[0].root = xF1;
    ap->inters_points[1].root = xF2;
    ap->inters_points[2].root = xF3;

    wprintf(L"\nТочки пересечения: xF1 = %lf, xF2 = %lf, xF3 = %lf\n\n", xF1, xF2, xF3);
    
    return 1;
}

