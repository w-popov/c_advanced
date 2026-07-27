/**
 * ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
 * А10: Необходимо реализовать программу, которой на входе 
 * передаётся вещественное число в типе float, 
 * она возвращает порядок который хранится в поле EXP в виде 
 * десятичного целого числа. 
 * Функция должна строго соответствовать прототипу:
 * int extractExp(float)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

union floatbit
{ 
    float value; 
    struct 
    { 
        uint32_t MAN : 23; 
        uint32_t EXP : 8; 
        uint32_t SIGN : 1;
    } bit;
};

int extractExp(float fnum)
{
    union floatbit fb;
    fb.value = fnum;
    return fb.bit.EXP;
}

int main (void)
{
    float input = 0.0f;
    scanf("%f", &input);
    printf("%d\n", extractExp(input));

    return EXIT_SUCCESS;
}