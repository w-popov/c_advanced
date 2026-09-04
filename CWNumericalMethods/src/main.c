#include <stdio.h>
#include <stdlib.h>
#include "numerical_app.h"
#include "pbPlots.h"
#include "supportLib.h"
#include <math.h>

int main(void)
{
    const int num_points = 260; 
    double x[num_points];
    double y[num_points];
    double current_x = -2.3;

    for(int i = 0; i < num_points; i++) 
    {
        x[i] = current_x;
        y[i] = f(current_x);
        current_x += 0.01;
    }

    // Выделение памяти под объект изображения
    RGBABitmapImageReference *imageRef = CreateRGBABitmapImageReference();
    StringReference str_ref_err = {.string = L"Err"};

    // Отрисовка графика (Ширина: 800px, Высота: 600px)
    DrawScatterPlot(imageRef, 800, 600, x, num_points, y, num_points, &str_ref_err);

    ByteArray *pngData = ConvertToPNG(imageRef->image);
    WriteToFile(pngData, "plot.png");

    // Освобождение ресурсов
    FreeAllocations();
    
    #if defined(_WIN32) || defined(_WIN64)
        system("start plot.png");
    #else
        system("xdg-open plot.png");
    #endif
    
    return 0;
}
