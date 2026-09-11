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

/**
 * Вычисление выражения для интеграла
 */
double calculate_S(struct IntegralProperties *ip, double x)
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

/**
 * Вычисление интеграла методом трапеций с фиксированным числом разбиений n
 */
double integrate_trapezoid(struct AppProperties *ap, struct IntegralProperties *ip, double a, double b, IntegralFunc f)
{
    int n = 2;                  // Старт с двух разбиений
    double S_old = 0.0;         // Предыдущее вычисление площади
    double S_new = 0.0;         // Новове вычисление площади
    double eps2 = ap->eps_2;    // Точность

    // Вычислять пока не будет достигнута точность eps2
    while (1) 
    {
        double h = (b - a) / n;
        double sum = (f(ip, a) + f(ip, b)) / 2.0;
        
        for (int i = 1; i < n; ++i) 
        {
            sum += f(ip, a + i * h);
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
 * Найти значения корня по полю "F" из массива "roots" в property.json
 */
static double find_root(struct AppProperties *ap, int id)
{
    for (size_t i = 0; i < ap->size_arr_inters_points; ++i)
    {
        if (id == ap->inters_points[i].id_F)
            return ap->inters_points[i].root;
    }
    return NAN;
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

    if (!ap->size_integral_array)
    {
        wprintf(L"\nОшибка: Заполните массив 'integral' в properties.json\n");
        exit(14);
    }
    
    // Массив площадей
    double squares[MAX_SQUARES_INTEGRAL] = {NAN};

    for (size_t i = 0, s_i = 0; i < ap->size_integral_array; ++i)
    {
        /* Подготовка парсера для вычисления полей json "integral"[]->"expr_S",
           компилировать текстовые формулы в rpn один раз для последующих вычислений */
        size_t rpn_cnt = compile_to_rpn(ap->integral_props_array[i].expr_s, ap->integral_props_array[i].rpn);
        ap->integral_props_array[i].rpn_count = rpn_cnt;

        // Вычисление площадей фигур

        // Корни
        double from = find_root(ap, ap->integral_props_array[i].from_f);
        double to = find_root(ap, ap->integral_props_array[i].to_f);
        if (isnan(from))
        {
            wprintf(L"\nОшибка: Корень %lf равен Nan\n", from);
            exit(15);
        }
        if (isnan(to))
        {
            wprintf(L"\nОшибка: Корень %lf равен Nan\n", to);
            exit(16);
        }
        double s = integrate_trapezoid(ap, &ap->integral_props_array[i], from, to, calculate_S);
        squares[s_i] = s;
        wprintf(L"Площадь %zu участка: %lf\n", i + 1, squares[s_i]);
        ++s_i;
    }
    // Общая площадь
    double total_S = 0.0;
    for (size_t i = 0; i < MAX_SQUARES_INTEGRAL; ++i)
    {
        if (!isnan(squares[i]))
        {
            total_S += squares[i];
        }
        else break;
    }
    
    wprintf(L"Итоговая площадь всей фигуры: %.6f\n", total_S);
    wprintf(L"Точность: %e\n", ap->eps_2);
    
    return 1;
}

// ==================== ПРИЛОЖЕНИЕ =========================================

// Вызов справки
void show_help(void) 
{
    wprintf(L"======================================================================\n");
    wprintf(L"        Численные методы. Курсовая работа 2. Попов В.Г.\n");
    wprintf(L"======================================================================\n\n");
    
    wprintf(L"Использование:\n");
    wprintf(L"  ./numerical [параметры]\n\n");
    
    wprintf(L"Параметры и ключи запуска:\n");
    wprintf(L"  -h, --help       Показать данную справочную информацию.\n");
    wprintf(L"  -p, --plots      Выполнить отрисовку графиков функций в файл 'plot.png'.\n");
    wprintf(L"  -r, --roots      Найти корни уравнений (метод деления отрезка пополам).\n");
    wprintf(L"  -i, --integral   Вычислить площади фигур (метод трапеций).\n");
    wprintf(L"  -t, --test       Запустить тесты.\n\n");
    
    wprintf(L"Примеры запуска:\n");
    wprintf(L"  ./numerical --help       (Вывод справки)\n");
    wprintf(L"  ./numerical -t           (Запуск тестов вычисления корня и интеграла)\n");
    wprintf(L"  ./numerical -p           (Только отрисовка графиков)\n");
    wprintf(L"  ./numerical -r i         (Поиск корней и расчет интегралов)\n");
    wprintf(L"  ./numerical --plots -r   (Расчет корней и вывод графиков)\n\n");
    wprintf(L"  ./numerical -r -i -p     (Расчет корней, расчет интегралов и вывод графиков)\n\n");

    
    wprintf(L"Конфигурация приложения считывается автоматически из файла 'properties.json'.\n");
    wprintf(L"Если файл 'properties.json' не существует, то он будет создан автоматически\
 с дефолтными настройками.\n");

    wprintf(L"======================================================================\n");
}

// ------------------------ ТЕСТЫ ----------------------------------

/**
 * @brief Тест поиска корня методом деления отрезка пополам
 */
void test_root(struct AppProperties *ap)
{
    wprintf(CBLUE L"\n[ТЕСТ] Модуль поиска корней (find_root_div_method)\n" CRESET);
    wprintf(L"Точность: %e\n", ap->eps_1);
    
    // Временная структура для теста
    struct Intersection_points test_ip;
    test_ip.id_F = 123;
    test_ip.nums_steps = 0;
    
    // Тест 1: Уравнение F(x) = x - 3 = 0. Корень должен быть x = 3.0
    size_t rpn_cnt = compile_to_rpn(L"x - 3", test_ip.rpn);
    test_ip.rpn_count = rpn_cnt;
    test_ip.a = 1.0;  // Левая граница интервала изоляции
    test_ip.b = 5.0;  // Правая граница интервала изоляции
    
    double root1 = find_root_div_method(&test_ip, 1e-5, calculate_F);
    int pass1 = (fabs(root1 - 3.0) < 1e-4);
    wprintf(L"Тест 1 (x - 3 = 0 на [1, 5]): Ожидалось: 3.0000, Получено: %.4lf -> %ls\n", 
            root1, pass1 ? CGREEN L"ПРОЙДЕН" CRESET : CRED L"ОШИБКА" CRESET);

    // Тест 2: Уравнение F(x) = x^2 - 4 = 0. На отрезке [0, 3] корень должен быть x = 2.0
    test_ip.rpn_count = compile_to_rpn(L"x * x - 4", test_ip.rpn); 
    test_ip.a = 0.0;
    test_ip.b = 3.0;
    
    double root2 = find_root_div_method(&test_ip, 1e-5, calculate_F);
    int pass2 = (fabs(root2 - 2.0) < 1e-4);
    wprintf(L"Тест 2 (x^2 - 4 = 0 на [0, 3]): Ожидалось: 2.0000, Получено: %.4lf -> %ls\n", 
            root2, pass2 ? CGREEN L"ПРОЙДЕН" CRESET : CRED L"ОШИБКА" CRESET);
}

/**
 * @brief Тест интеграла методом трапеций
 */
void test_integral(struct AppProperties *ap)
{
    wprintf(CBLUE L"\n[ТЕСТ] Модуль интегрирования (integrate_trapezoid)\n" CRESET);
    wprintf(L"Точность: %e\n", ap->eps_1);
    
    // Временная структура для теста интеграла
    struct IntegralProperties test_ip;
    double original_eps = ap->eps_2;
    ap->eps_2 = 1e-4; 

    // Тест 1: Интеграл от константы f(x) = 5.0 на отрезке [0, 2]
    // Ответ: 5.0 * (2 - 0) = 10.0
    test_ip.rpn_count = compile_to_rpn(L"5", test_ip.rpn);
    double res1 = integrate_trapezoid(ap, &test_ip, 0.0, 2.0, calculate_S);
    int pass1 = (fabs(res1 - 10.0) < 1e-3);
    wprintf(L"Тест 1 (f(x) = 5 на [0, 2]): Ожидалось: 10.0000, Получено: %.4lf -> %ls\n", 
            res1, pass1 ? CGREEN L"ПРОЙДЕН" CRESET : CRED L"ОШИБКА" CRESET);

    // Тест 2: Интеграл от линейной функции f(x) = 2*x на отрезке [1, 3]
    // Ответ: (3^2) - (1^2) = 9.0 - 1.0 = 8.0
    test_ip.rpn_count = compile_to_rpn(L"2 * x", test_ip.rpn);
    double res2 = integrate_trapezoid(ap, &test_ip, 1.0, 3.0, calculate_S);
    int pass2 = (fabs(res2 - 8.0) < 1e-3);
    wprintf(L"Тест 2 (f(x) = 2x на [1, 3]): Ожидалось: 8.0000, Получено: %.4lf -> %ls\n", 
            res2, pass2 ? CGREEN L"ПРОЙДЕН" CRESET : CRED L"ОШИБКА" CRESET);

    ap->eps_2 = original_eps;
}

/**
 * @brief Единая точка запуска тестов. 
 */
void run_all_tests(struct AppProperties *ap)
{
    #if defined(_WIN32) || defined(_WIN64)
    // Включение поддержки ANSI-последовательностей для Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
            SetConsoleMode(hOut, dwMode);
        }
    }
    #endif

    wprintf(CBLUE L"\n================ Запуск тестов ================\n" CRESET);
    test_root(ap);
    test_integral(ap);
    wprintf(CBLUE L"===========================================================\n\n" CRESET);
}

