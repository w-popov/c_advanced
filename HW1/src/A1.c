/**
 * ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
 * А1: На стандартном потоке ввода задается целое неотрицательное число N и 
 * последовательность допустимых символов в кодировке ASCII, оканчивающаяся точкой. 
 * Допустимые символы - латинские буквы 'a' ... 'z', 'A' ... 'Z' и пробел. 
 * Требуется закодировать латинские буквы ('a' ... 'z', 'A' ... 'Z') шифром Цезаря, 
 * пробелы вывести без изменения. Число N задает сдвиг в шифре. 
 * Указание: использовать массивы запрещается. 
 */
#include <stdio.h>
#include <stdlib.h>

int main (void)
{
    int n = 0, symbol = 0;
    int is_begin = 1;
    scanf("%d", &n);
    n = n % 26;
    while ((symbol = getchar()) != EOF)
    {
        if (symbol >= 'a' && symbol <= 'z')
        {
            symbol = 'a' + (symbol - 'a' + n) % 26;
            is_begin = 0;
            printf("%c", symbol);
        } else if (symbol >= 'A' && symbol <= 'Z')
        {
            symbol = 'A' + (symbol - 'A' + n) % 26;
            is_begin = 0;
            printf("%c", symbol);
        } 
        else if (symbol == ' ' || symbol == '.')
        {
            if (symbol == ' ' && is_begin)
            {
                continue;
            }
            printf("%c", symbol);
            if (symbol == '.')
            {
                break;
            }
        }
    }
    printf("\n");
    return EXIT_SUCCESS;
}