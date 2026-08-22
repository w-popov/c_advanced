/**
 * HW5, Cи продвинутый. гр.Д01-134 Попов В.Г
 * C2: Необходимо вычислить выражение написанное в обратной польской записи. 
 * На вход подается строка состоящая из целых, неотрицательных чисел и 
 * арифметических символов. В ответ единственное целое число - результат. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK       100
#define MAX_TOKEN       64

long long stack[MAX_STACK];
int top = 0;

void push(long long val) 
{
    stack[top++] = val;
}

long long pop(void) 
{
    return stack[--top];
}

int main(void) 
{
    char token[MAX_TOKEN];

    while (scanf("%63s", token) == 1) 
    {
        if (strcmp(token, ".") == 0) 
        {
            break;
        }

        size_t len = strlen(token);
        int has_dot = 0;

        if (len > 0 && token[len - 1] == '.') 
        {
            token[len - 1] = '\0';
            has_dot = 1;
            len--;
        }

        if (len == 0) 
        {
            if (has_dot) break;
            continue;
        }
        // если число
        if (isdigit((unsigned char)token[0])) 
        {
            push(atoll(token));
        } 
        else 
        {
            // если оператор
            // b — это верхний элемент (второй операнд), элемент под ним (первый операнд)
            long long b = pop();
            long long a = pop();

            if (strcmp(token, "+") == 0) 
            {
                push(a + b);
            } 
            else if (strcmp(token, "-") == 0) 
            {
                push(a - b);
            } 
            // × - не икс
            else if (strcmp(token, "×") == 0 || strcmp(token, "x") == 0 || strcmp(token, "*") == 0 ) 
            { 
                push(a * b);
            } 
            else if (strcmp(token, "/") == 0) 
            {
                push(a / b);
            }
        }
        
        if (has_dot) break;
    }

    // финальный результат, оставшийся на вершине стека
    if (top > 0) 
    {
        printf("%lld\n", pop());
    } 
    else 
    {
        printf("0\n");
    }

    return 0;
}