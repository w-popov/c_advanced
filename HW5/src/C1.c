/**
 * HW5, Cи продвинутый. гр.Д01-134 Попов В.Г
 * C1: На стандартном потоке ввода задаются две символьные строки, 
 * разделённые символом перевода строки. Каждая из строк не превышает 
 * по длине 10000 символов. В строках не встречаются пробельные символы. 
 * На стандартный поток вывода напечатайте два числа, разделённых пробелом: 
 * первое число — длина наибольшего префикса первой строки, являющегося 
 * суффиксом второй; второе число — наоборот, длина наибольшего суффикса 
 * первой строки, являющегося префиксом второй. Сравнение символов проводите 
 * с учётом регистра (т.е. символы 'a' и 'A' различны).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_LEN 10001
#define TOTAL_LEN (MAX_LEN * 2)

void func_z(const char *str, size_t n, int *z) {
    z[0] = 0;
    size_t l = 0, r = 0;

    for (size_t i = 1; i < n; ++i) 
    {
        if (i <= r) 
        {
            size_t k = i - l;
            z[i] = (z[k] < (int)(r - i + 1)) ? z[k] : (int)(r - i + 1);
        } 
        else 
        {
            z[i] = 0;
        }
        while (i + z[i] < n && str[z[i]] == str[i + z[i]]) 
        {
            z[i]++;
        }

        if (i + z[i] - 1 > r) 
        {
            l = i;
            r = i + z[i] - 1;
        }
    }
}

size_t find_overlap(const char *s1, const char *s2) 
{
    char concat[TOTAL_LEN];
    int z[TOTAL_LEN];
    size_t len1 = strlen(s1);
    int written = sprintf(concat, "%s#%s", s1, s2);

    if (written <= 0) return 0;
    
    size_t total_len = (size_t)written;
    func_z(concat, total_len, z);

    for (size_t i = len1 + 1; i < total_len; ++i) 
    {
        size_t suffix_len = total_len - i;
        if ((size_t)z[i] == suffix_len) 
        {
            return suffix_len;
        }
    }
    return 0;
}

int main(void)
{
    char s1[MAX_LEN];
    char s2[MAX_LEN];

    scanf("%10001s", s1);
    scanf("%10001s", s2);

    size_t ans1 = find_overlap(s1, s2);
    size_t ans2 = find_overlap(s2, s1);

    printf("%zu %zu\n", ans1, ans2);

    return 0;
}