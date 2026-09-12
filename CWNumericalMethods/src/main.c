#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <getopt.h>
#else
    #include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <getopt.h>
#include <wchar.h>
#include "numerical_app.h"
#include "draw_plots.h"
#include "math_parser.h"


extern char *optarg;
extern int optind, opterr, optopt;


int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");

    #if defined(_WIN32) || defined(_WIN64)
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
        // Включение поддержки ANSI-последовательностей для цветной консоли в Windows
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
                SetConsoleMode(hOut, dwMode);
            }
        }
    #endif

    // Переменные-флаги для сохранения параметров командной строки
    int run_plots = 0;
    int run_roots = 0;
    int run_integral = 0;
    int show_help_flag = 0;
    int run_tests = 0;

    // Описание соответствия длинных ключей коротким символам
    static struct option long_options[] = {
        {"help",     no_argument, 0, 'h'},
        {"plots",    no_argument, 0, 'p'},
        {"roots",    no_argument, 0, 'r'},
        {"integral", no_argument, 0, 'i'},
        {"test",     no_argument, 0, 't'},
        {0, 0, 0, 0} 
    };

    int key = -1;
    int option_index = 0;
    opterr = 0;

    // Разбор длинных и коротких флагов
    while ((key = getopt_long(argc, argv, "thpri", long_options, &option_index)) != -1)
    {
        switch (key)
        {
        case 'h':
            show_help_flag = 1;
            break;
        case 'p':
            run_plots = 1;
            break;
        case 'r':
            run_roots = 1;
            break;
        case 'i':
            run_integral = 1;
            break;
        case 't':
            run_tests = 1;
            break;
        case '?':
        default:
            wprintf(L"[ERR] Ошибка: Неизвестный флаг. Используйте -help или -h для справки.\n");
            show_help();
        #if defined(_WIN32) || defined(_WIN64)
            wprintf(L"\nНажмите Enter для выхода...\n");
            system("pause");
        #endif
            return EXIT_FAILURE;
        }
    }

    if (argc == 1 || optind == 1 || show_help_flag)
    {
        show_help();
        #if defined(_WIN32) || defined(_WIN64)
                wprintf(L"\nНажмите Enter для выхода...\n");
                system("pause");
        #endif
        return EXIT_SUCCESS;
    }

    
    struct AppProperties props = parse_json("properties.json");
    props.pdf = make_properties(&props); 
    
    // Если передан флаг -t / --test, запуск тестов
    if (run_tests)
    {
        wprintf(L"[INFO] Запуск тестов...\n");
        run_all_tests(&props);
        return EXIT_SUCCESS;
    }
    
    // Если передан флаг -p / -plots
    if (run_plots && !run_roots && !run_integral)
    {
        wprintf(L"[INFO] Отрисовка графиков функций...\n");
        draw_plots(&props, props.size_prors_array, "plot.png");
    }

    // Если передан флаг -r / -roots
    if (run_roots)
    {
        wprintf(L"[INFO] Поиск корней уравнений (метод деления отрезка пополам)...\n");
        root(&props);  
        
        if (run_plots && !run_integral) 
        {
            wprintf(L"[INFO] Обновление графиков с учетом найденных корней...\n");
            draw_plots(&props, props.size_prors_array, "plot.png");
        }
    }

    // Если передан флаг -i / -integral
    if (run_integral)
    {
        wprintf(L"[INFO] Вычисление интеграла площади (метод трапеций)...\n\n");
        wprintf(L"[INFO] Поиск корней уравнений (метод деления отрезка пополам)...\n");
        root(&props);
        if (run_plots) 
        {
            wprintf(L"[INFO] Обновление графиков с учетом найденных корней...\n");
            draw_plots(&props, props.size_prors_array, "plot.png");
        }
        integral(&props);
    }

    // Если аргументы не валидны
    if (!run_plots && !run_roots && !run_integral)
    {
        wprintf(L"[WARN] Предупреждение: Не выбрано ни одно действие. Используйте флаги -p, -r или -i.\n");
        show_help();
        #if defined(_WIN32) || defined(_WIN64)
                wprintf(L"\nНажмите Enter для выхода...\n");
                system("pause");
        #endif
        return EXIT_SUCCESS;
    }

    // Освобождение динамической памяти приложения
    free_array_points(&props, props.size_prors_array);

    return EXIT_SUCCESS;
}

/*
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./numerical -r -i -p
*/
