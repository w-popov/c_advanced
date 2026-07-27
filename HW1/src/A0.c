/**
 * ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
 * А0: На стандартном потоке ввода задаётся натуральное число N (N > 0), 
 * после которого следует последовательность из N целых чисел.
 * На стандартный поток вывода напечатайте, сколько раз в этой последовательности 
 * встречается максимум. Указание: использовать массивы запрещается.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (void)
{
    int N = 0, max = INT32_MIN;
    int number = 0, count_max = 0;
    scanf("%d", &N);
    for (int i = 0; i < N; ++i)
    {
        scanf("%d", &number);
        if (number > max)
        {
            max = number;
            count_max = 1;
        }
        else if (number == max)
        {
            ++count_max;
        }
    }
    
    printf("%d\n", count_max);
    return EXIT_SUCCESS;
}
