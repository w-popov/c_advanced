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
double find_root_div_method (struct Intersection_points *ip, double eps1, IntersectionFunc F)
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

    while ((b - a) / 2.0 > eps1) 
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
    wprintf(L"Корень найден с точностью %e за %zu шагов.\n", eps1, step_count);

    return (a + b) / 2.0;

}

/**
 * @brief Метод деления отрезка пополам (поиск корня)
 */
int root(struct AppProperties *ap)
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

// --------------------------------- ИНТЕГРАЛ --------------------------------------------

// Площадь на участке сверху f1, снизу f3
double integrand_segment1(struct AppProperties *ap, double x)
{
    return calculate_F(&ap->inters_points[0], x);
}

// На участке сверху f1, снизу f2
double integrand_segment2(struct AppProperties *ap, double x)
{
    return calculate_F(&ap->inters_points[2], x);
}

/**
 * Вычисление интеграла методом трапеций с фиксированным числом разбиений n
 */
double integrate_trapezoid(struct AppProperties *ap, double eps2, double a, double b, IntegralFunc f)
{
    int n = 2;          // Старт с двух разбиений
    double S_old = 0.0; // Предыдущее вычисление площади
    double S_new = 0.0; // Новове вычисление площади

    // Вычислять пока не будет достигнута точность eps2
    while (1) 
    {
        double h = (b - a) / n;
        double sum = (f(ap, a) + f(ap, b)) / 2.0;
        
        for (int i = 1; i < n; ++i) 
        {
            sum += f(ap, a + i * h);
        }
        S_new = sum * h;

        if (n > 2 && fabs(S_new - S_old) < eps2) 
        {
            wprintf(L"Интеграл на отрезке [%.3f, %.3f] вычислен за n = %d разбиений.\n", a, b, n);
            break; 
        }

        S_old = S_new;
        n *= 2; // Удвоить количество полосок
    }

    return S_new;
}

/**
 * Вычисление интеграла 
 */
int integral(struct AppProperties *ap)
{
    if (ap == NULL)
    {
        perror("\nError in integral()! NULL pointer *ap\n");
        return 0;
    }
    // Хардкор. Если графиков не 3, то выход.
    // Можно было бы сделать по аналогии с поиском корней, создав массив 
    // данных в property.json (сделал, но вышло слишком сложно -> оставлю как есть сейчас)
    if (ap->size_arr_inters_points != 3)
    {
        wprintf(L"\nОшибка: заполните данные для 3 функций в property.json!\n");
        return 0;
    }
    
    double S1 = integrate_trapezoid(ap, ap->eps_2, ap->inters_points[0].root, ap->inters_points[1].root, integrand_segment1);
    double S2 = integrate_trapezoid(ap, ap->eps_2, ap->inters_points[1].root, ap->inters_points[2].root, integrand_segment2);
    double total_S = S1 + S2;

    wprintf(L"Площадь первого участка: %.6f\n", S1);
    wprintf(L"Площадь второго участка: %.6f\n", S2);
    wprintf(L"Итоговая площадь всей плоской фигуры: %.6f\n", total_S);
    wprintf(L"Точность: %e\n", ap->eps_2);
    
    return 1;
}

// ==================== ПРИЛОЖЕНИЕ =========================================

// Вызов справки
void show_help(void) 
{
    wprintf(L"======================================================================\n");
    wprintf(L"        Программа вычисления интегралов       \n");
    wprintf(L"======================================================================\n\n");
    
    wprintf(L"Использование:\n");
    wprintf(L"  ./numerical [параметры]\n\n");
    
    wprintf(L"Параметры и ключи запуска:\n");
    wprintf(L"  -h, --help       Показать данную справочную информацию.\n");
    wprintf(L"  -p, --plots      Выполнить отрисовку графиков функций в файл 'plot.png'.\n");
    wprintf(L"  -r, --roots      Найти корни уравнений (метод деления отрезка пополам).\n");
    wprintf(L"  -i, --integral   Вычислить площади фигур (метод трапеций).\n\n");
    
    wprintf(L"Примеры запуска:\n");
    wprintf(L"  ./numerical --help       (Вывод справки)\n");
    wprintf(L"  ./numerical -p           (Только отрисовка графиков)\n");
    wprintf(L"  ./numerical -r i         (Поиск корней и расчет интегралов)\n");
    wprintf(L"  ./numerical --plots -r   (Расчет корней и вывод графиков)\n\n");
    wprintf(L"  ./numerical -r -i -p     (Расчет корней, расчет интегралов и вывод графиков)\n\n");

    
    wprintf(L"Конфигурация приложения считывается автоматически из файла 'properties.json'.\n");
    wprintf(L"Если файл 'properties.json' отсутствует, то он будет создан автоматически\
 с дефолтными настройками.\n");

    wprintf(L"======================================================================\n");
}
