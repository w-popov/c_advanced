/**
 * ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
 * А3: На вход программе подается беззнаковое 32-битное целое число N и 
 * натуральное число K (1 ≤ K ≤ 31). Требуется взять K младших битов числа N 
 * и вывести полученное таким образом число.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (void)
{
    uint32_t n = 0, k = 0;
    scanf("%u %u", &n, &k);
    uint32_t mask = (1 << k) - 1;
    uint32_t result = n & mask;
    printf("%u\n", result);

    return EXIT_SUCCESS;
}