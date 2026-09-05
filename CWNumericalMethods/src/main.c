#include <stdio.h>
#include <stdlib.h>
#include "numerical_app.h"
#include "draw_plots.h"
#include <locale.h>

int main(void)
{
    setlocale(LC_ALL, "");

    struct PlotDrawFun pdf[NUMS_PLOTS] = {
        {
            .color = {.r = 0.8, .g = 0, .b = 0},
            .points = calculate_function(-6.0, 6.0, 300, f1),
            .title = L"f1 = 0.6x + 3 "
        },
        {
            .color = {.r = 0., .g = 0.7, .b = 0},
            .points = calculate_function(-6.0, 6.0, 300, f2),
            .title = L"f2 = (x-2)^3 - 1"
        },
        {
            .color = {.r = 0, .g = 0.6, .b = 1.},
            .points = calculate_function(-6.0, 6.0, 300, f3),
            .title = L"f3 = 3 / x"
        }
    };    
    
    draw_plots(pdf, NUMS_PLOTS, "plot.png");

    free_array_points(pdf, NUMS_PLOTS);

    return 0;
}
