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

