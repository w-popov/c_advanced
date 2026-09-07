#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "numerical_app.h"
#include "draw_plots.h"
#include "math_parser.h"

int main(void)
{
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");

    // Структура приложения, заполнить данными
    struct AppProperties props = parse_json("properties.json");
    props.pdf = make_properties(&props);   
    
    // Отрисовка и открытие файла графиков
    draw_plots(props.pdf, props.size_prors_array, "plot.png");

    // Освобождение памяти
    free_array_points(&props, props.size_prors_array);

    return 0;
}

/*
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./numerical
*/
