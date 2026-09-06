#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "numerical_app.h"
#include "draw_plots.h"
#include "math_parser.h"

int main(void)
{
    setlocale(LC_ALL, ".UTF-8");

    // struct PlotDrawFun pdf[NUMS_PLOTS] = {
    //     {
    //         .color = {.r = 0.8, .g = 0, .b = 0},
    //         .points = calculate_function(-6.0, 6.0, 300, f1),
    //         .title = L"f1 = 0.6x + 3 "
    //     },
    //     {
    //         .color = {.r = 0., .g = 0.7, .b = 0},
    //         .points = calculate_function(-6.0, 6.0, 300, f2),
    //         .title = L"f2 = (x-2)^3 - 1"
    //     },
    //     {
    //         .color = {.r = 0, .g = 0.6, .b = 1.},
    //         .points = calculate_function(-6.0, 6.0, 300, f3),
    //         .title = L"f3 = 3 / x"
    //     }
    // };    
    
    // draw_plots(pdf, NUMS_PLOTS, "plot.png");

    // free_array_points(pdf, NUMS_PLOTS);





    // --------------------------------------------------------------------
    const char* expr = "sin(pi / 2) *  sqrt(x) - min(5, 10)";
    double x_value = 100.0;

    struct Token tokens[MAX_TOKENS];
    struct Token rpn[MAX_TOKENS];
    
    printf("Входная строка 1: %s\n", expr);
    printf("Заданное значение x = %.2f\n\n", x_value);

    size_t count = tokenize(expr, tokens); 
    size_t rpn_count = shunting_yard_parse(tokens, count, rpn);

    double result = eval_rpn(rpn, rpn_count, x_value);
    printf("Финальный ответ:   %.6lf\n", result);

    // ---------------------------------------------------------------------

    return 0;
}
