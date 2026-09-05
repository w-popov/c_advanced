#include <stdio.h>
#include <stdlib.h>
#include "numerical_app.h"
#include "draw_plots.h"
#include <locale.h>

int main(void)
{
    setlocale(LC_ALL, "");

    struct PlotDrawFun pdf[2] = {
        {
            .color = {.r = 0.8, .g = 0, .b = 0},
            .points = calculate_function(-2.5, 260, f),
            .title = L"8x^4 + 32x^3 + 40x^2 + 16x + 1"
        },
        {
            .color = {.r = 0, .g = 0.8, .b = 0},
            .points = calculate_function(-1.1, 260, f2),
            .title = L"(x-2)^3 - 1"
        },
    };    
    
    draw_plots(pdf, 2, "plot.png");

    free_array_points(pdf, 2);

    return 0;
}
