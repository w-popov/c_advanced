#include <stdio.h>
#include <math.h>
#include "numerical_app.h"

double f(double x) 
{
    return 8 * pow(x, 4) + 32 * pow(x, 3) + 40 * pow(x, 2) + 16 * x + 1;
}