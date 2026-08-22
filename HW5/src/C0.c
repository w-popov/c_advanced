/**
 * HW5, Cи продвинутый. гр.Д01-134 Попов В.Г
 * 
 * C0: Сколько различных трехзначных чисел можно получить 
 * из заданного натурального N, вычеркивая цифры из его десятичной записи?  
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LEN 1001 

int main(void)
{
    char darr[MAX_LEN] = {0};
    long long dp[4] = {1, 0, 0, 0};
    long long last_dp[10][4] = {0};

    scanf("%1001s", darr);

    for (int i = 0; darr[i] != '\0'; ++i) 
    {
        int d = darr[i] - '0';
        for (int len = 3; len >= 1; --len) 
        {
            if (len == 1 && d == 0) continue; 
            long long new_comb = dp[len - 1] - last_dp[d][len - 1];
            dp[len] += new_comb;
            last_dp[d][len - 1] += new_comb;
        }
    }

    printf("%lld\n", dp[3]);

    return 0;
}