/**
 * ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
 * А4: На вход программе подается беззнаковое 32-битное целое число N 
 * и натуральное число K (1 ≤ K ≤ 31). Требуется взять K подряд идущих битов 
 * числа N так, чтобы полученное число было максимальным. 
 * Программа должна вывести полученное число.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static uint32_t get_max (uint32_t a, uint32_t b)
{
    // все биты 1 либо все биты 0
    uint32_t mask = -(uint32_t)(a < b);

    return a ^ ((a ^ b) & mask);
}

int main (void)
{
    uint32_t n = 0, k = 0;
    scanf("%u %u", &n, &k);
    uint32_t mask = (1 << k) - 1;
    uint32_t max_val = 0;

    for (uint32_t shift = 0; shift <= 32 - k; ++shift)
    {
        uint32_t current_val = (n >> shift) & mask;
        max_val = get_max(current_val, max_val);
    }

    printf("%u\n", max_val);
    return EXIT_SUCCESS;
}