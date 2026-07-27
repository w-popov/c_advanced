/**
 * ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
 * А9: В программе реализована структура данных:
    struct pack_array {
    uint32_t array; // поле для хранения упакованного массива из 0 и 1
    uint32_t count0 : 8; // счетчик нулей в array
    uint32_t count1 : 8; // счетчик единиц в array
    }
    Необходимо реализовать программу, которая упаковывает переданный ей 
    массив из 32-ух элементов 0 и 1 в указанную структуру данных.
    Функция должна строго соответствовать прототипу:
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SIZE 32

struct pack_array 
{
    uint32_t array; // поле для хранения упакованного массива из 0 и 1
    uint32_t count0 : 8; // счетчик нулей в array
    uint32_t count1 : 8; // счетчик единиц в array
} pack_s;

void array2struct(int arr[], struct pack_array *pack)
{
    pack->array = 0;
    pack->count0 = 0;
    pack->count1 = 0;
    for (int i = 0; i < SIZE; ++i)
    {
        pack->array = (pack->array << 1) | (arr[i] & 1);

        if (arr[i]) 
        {
            pack->count1++;
        } 
        else 
        {
            pack->count0++;
        }
    }
}

int main (void)
{
    int arr[SIZE] = {0};
    
    for (int i = 0; i < SIZE; ++i)
    {
        scanf("%d", &arr[i]);
    }
    array2struct(arr, &pack_s);
    printf("%u %u %u\n", pack_s.array, pack_s.count0, pack_s.count1);
    return EXIT_SUCCESS;
}