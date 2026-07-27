/**
 * ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
 * А5: На вход программе подается беззнаковое 32-битное целое число N. 
 * Требуется найти количество единичных битов в двоичном представлении данного числа. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (void)
{
    uint32_t n;
    scanf("%u", &n);
    
    int count = 0;
    while (n > 0) 
    {
        n &= (n - 1); 
        count++;  
    }

    printf("%d\n", count);
    return EXIT_SUCCESS;
}