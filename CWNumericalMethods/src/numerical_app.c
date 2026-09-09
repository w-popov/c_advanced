#include "numerical_app.h"
#include "draw_plots.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Ф-ция вычисления выражения math функций f(x)
 */
double calculate_f(struct PlotProperties *pp, double x)
{    
    return eval_rpn(pp->rpn, pp->rpn_count, x);
}

/**
 * Ф-ция вычисления выражения math функций F(x), т-е корней
 */
double calculate_F(struct Intersection_points *ip, double x)
{
    return eval_rpn(ip->rpn, ip->rpn_count, x);
}

// ------------------------------------------------------------
/**
 * @brief Найти корень
 */
double find_root_div_method (struct Intersection_points *ip, double eps, IntersectionFunc F)
{
    double c;
    size_t step_count = 0; // Число шагов
    double a = ip->a;
    double b = ip->b;

    if (isnan(a) || isnan(b))
    {
        wprintf(L"Ошибка: интервалы a или b F%d = null\n", ip->id_F);
    }
    
    if (F(ip, a) * F(ip, b) > 0) 
    {
        wprintf(L"Ошибка: на интервале [%.2f, %.2f] нет корня.\n", a, b);
        return (a + b) / 2.0; 
    }

    while ((b - a) / 2.0 > eps) 
    {
        step_count++;
        c = (a + b) / 2.0;
        
        // Если корень найден
        if (fabs(F(ip, c)) < 1e-15) 
        {
            ip->nums_steps = step_count;
            wprintf(L"Корень найден точно за %zu шагов.\n", step_count);
            return c;
        }
        
        // Сужение интервала
        if (F(ip, a) * F(ip, c) < 0) 
        {
            b = c;
        } 
        else 
        {
            a = c;
        }
    }

    ip->nums_steps = step_count;
    wprintf(L"Корень найден с точностью %e за %zu шагов.\n", eps, step_count);

    return (a + b) / 2.0;

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

    if (!ap->size_arr_inters_points)
    {
        wprintf(L"\nОшибка: Массив 'roots' в properties.json ПУСТ.\n");
        return 0;
    }

    wprintf(L"\nТочки пересечения:\n");
    for (size_t i = 0; i < ap->size_arr_inters_points; ++i)
    {
        /* Подготовка парсера для вычисления полей json "roots"[]->"root_expr",
           компилировать текстовые формулы в rpn один раз для последующих вычислений */
        size_t rpn_cnt = compile_to_rpn(ap->inters_points[i].root_expr, ap->inters_points[i].rpn);
        ap->inters_points[i].rpn_count = rpn_cnt;
        // Поиск корней
        double root = find_root_div_method(&ap->inters_points[i], ap->eps_1, calculate_F);
        ap->inters_points[i].root = root;
        wprintf(L"xF%d = %lf\n", ap->inters_points[i].id_F, root);
    }
    
    return 1;
}

