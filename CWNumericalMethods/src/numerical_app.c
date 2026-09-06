#include "numerical_app.h"
#include "draw_plots.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Ф-ция вычисления выражения math функций
 */
double calculate(wchar_t *expr, double x)
{
    // Преобразование широкой строки в обычную
    size_t len = wcstombs(NULL, expr, 0);
    if (len == (size_t)-1) 
    {
        fprintf(stderr, "Error: Failed to GGGG convert wide string\n");
        exit(EXIT_FAILURE);
    }
    char *expr_char = (char*)malloc(len + 1);
    if (!expr_char) 
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    wcstombs(expr_char, expr, len + 1);
    
    // Токенизация выражения
    struct Token tokens[MAX_TOKENS];
    struct Token rpn[MAX_TOKENS];
    size_t count = tokenize(expr_char, tokens);
    size_t rpn_count = shunting_yard_parse(tokens, count, rpn);
    
    free(expr_char);
    
    return eval_rpn(rpn, rpn_count, x);
}