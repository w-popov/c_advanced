/**
 * Парсер матем. выражений из строки. 
 * Переписан на основе статьи:
 * https://dronperminov.ru/articles/math-expressions-parsing-in-vanilla-javascript-part-three-shunting-yard-parser
 */

#include "math_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

const char* known_constants[] = { "pi", "e", "PI", "E" };
const size_t num_constants = sizeof(known_constants) / sizeof(known_constants[0]);

const struct FuncProperties known_functions[] = {
    {"sin", 1}, {"cos", 1}, {"tg", 1}, {"sqrt", 1}, {"ln", 1},
    {"max", 2}, {"min", 2}
};
const size_t num_functions = sizeof(known_functions) / sizeof(known_functions[0]);


// Является ли строка именем известной функции
int is_function(const char* name)
{
    for (size_t i = 0; i < num_functions; i++)
    {
        if (strcmp(known_functions[i].name, name) == 0) 
            return 1;
    }
    return 0;
}

// Является ли строка именем известной константы
int is_constant(const char* name)
{
    for (size_t i = 0; i < num_constants; i++)
    {
        if (strcmp(known_constants[i], name) == 0)
            return 1;
    }
    return 0;
}

// ----------------------- ТОКЕНИЗАТОР -----------------------------------------

/**
 * Основная функция токенизации
 * @param expression — исходная математическая строка
 * @param tokens_out — массив структур Token, куда запишется результат
 * @return Количество успешно распознанных токенов
 */
size_t tokenize(const char* expression, struct Token* tokens_out)
{
    size_t count = 0;                   // Счетчик найденных токенов
    size_t i = 0;                       // Текущий индекс в исходной строке
    size_t len = strlen(expression);    // Общая длина входной строки

    // Главный цикл разбора
    while (i < len) {
        
        // Пропуск пробелов tab и \n
        if (isspace((unsigned char)expression[i]))
        {
            ++i;
            continue;
        }
        if (count >= MAX_TOKENS)
        {
            fprintf(stderr, "Error: Maximum number of tokens exceeded (%d)\n", MAX_TOKENS);
            exit(EXIT_FAILURE);
        }

        struct Token* token = &tokens_out[count];
        token->start = i;

        if (expression[i] == '(') 
        {
            token->type = TOKEN_LEFT_PARENTHESIS;
            strcpy(token->value, "(");
            ++i;
        }
        else if (expression[i] == ')')
        {
            token->type = TOKEN_RIGHT_PARENTHESIS;
            strcpy(token->value, ")");
            ++i;
        }
        else if (expression[i] == ',')
        {
            token->type = TOKEN_DELIMETER;
            strcpy(token->value, ",");
            ++i;
        }
        
        // Проверка матем. операторов
        else if (strchr("+-*/^", expression[i]) != NULL)
        {
            token->type = TOKEN_OPERATOR;
            token->value[0] = expression[i];
            token->value[1] = '\0';
            ++i;
        }
        
        // Разбор чисел
        else if (isdigit((unsigned char)expression[i]))
        {
            token->type = TOKEN_NUMBER;
            size_t v_idx = 0; // Внутренний индекс для token->value
            
            // Накопление символов, пока идут цифры или точки
            while (i < len && (isdigit((unsigned char)expression[i]) || expression[i] == '.'))
            {
                if (v_idx < MAX_VAL_LEN - 1)
                {
                    token->value[v_idx++] = expression[i];
                }
                ++i;
            }
            token->value[v_idx] = '\0'; // конец строки
        }
        
        // Разбор буквенных идентификаторов (Переменные, Константы, Функции)
        else if (isalpha((unsigned char)expression[i]) || expression[i] == '_')
        {
            size_t v_idx = 0;
            
            // Считать слово целиком, пока идут буквы, цифры или подчеркивания
            while (i < len && (isalnum((unsigned char)expression[i]) || expression[i] == '_'))
            {
                if (v_idx < MAX_VAL_LEN - 1)
                {
                    token->value[v_idx++] = expression[i];
                }
                ++i;
            }
            token->value[v_idx] = '\0';

            // Классификация токена
            if (is_function(token->value))
            {
                token->type = TOKEN_FUNCTION;
            } 
            else if (is_constant(token->value))
            {
                token->type = TOKEN_CONSTANT;
            } 
            else 
            {
                // Если это не функция и не константа, значит это x, y и др
                token->type = TOKEN_VARIABLE;
            }
        }
        // Обработка неизвестных символов
        else 
        {
            fprintf(stderr, "Error: invalid tokens found: '%c' on pos: %zu\n", expression[i], i);
            exit(EXIT_FAILURE);
        }

        token->end = i;
        ++count;
    }

    return count;
}

// debug
void print_tokens(const struct Token* tokens, size_t count)
{
    const char* type_names[] = {
        "left_parenthesis", "right_parenthesis", "delimeter", 
        "operator", "function", "constant", "number", "variable"
    };

    printf("[\n");
    for (size_t i = 0; i < count; i++)
    {
        printf("  {\"type\": \"%s\", \"value\": \"%s\", \"start\": %zu, \"end\": %zu}%s\n",
               type_names[tokens[i].type], tokens[i].value, tokens[i].start, tokens[i].end,
               (i == count - 1) ? "" : ",");
    }
    printf("]\n");
}

// ------------------------ СОРТИРОВОЧНАЯ СТАНЦИЯ -----------------------------------------

// Поиск свойств оператора
struct OpProperties get_op_properties(const char* op)
{
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) 
        return (struct OpProperties){1, ASSOC_LEFT};

    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) 
        return (struct OpProperties){2, ASSOC_LEFT};

    if (strcmp(op, "^") == 0 || strcmp(op, "~") == 0) 
        return (struct OpProperties){3, ASSOC_RIGHT}; // ~ это унарный минус

    return (struct OpProperties){0, ASSOC_LEFT};
}

// Поиск количества аргументов функции (-1 если не найдена)
int get_func_args(const char* name)
{
    for (size_t i = 0; i < num_functions; i++)
    {
        if (strcmp(known_functions[i].name, name) == 0) 
            return known_functions[i].args;
    }
    return -1;
}

// Функция аварийного выхода при ошибках синтаксиса
void throw_syntax_error(const char* expected, const struct Token* tok)
{
    fprintf(stderr, "Syntaxis error: expected %s, but got \"%s\" (%zu:%zu)\n", 
            expected, tok->value, tok->start, tok->end);
    
    exit(EXIT_FAILURE);
}

// Унарный оператор?
int is_unary(const struct Token* prev)
{
    return prev == NULL || 
           prev->type == TOKEN_OPERATOR || 
           prev->type == TOKEN_DELIMETER || 
           prev->type == TOKEN_LEFT_PARENTHESIS;
}

// Определение приоритета
int is_more_precedence(struct Token top, struct OpProperties op_props)
{
    if (top.type != TOKEN_OPERATOR) 
        return 0;

    struct OpProperties top_props = get_op_properties(top.value);
    
    if (op_props.associative == ASSOC_RIGHT)
    {
        return top_props.precedence > op_props.precedence;
    }

    return top_props.precedence >= op_props.precedence;
}

/**
 * Алгоритм Сортировочной Станции
 * @param tokens        Указатель на массив исходных токенов после лексера
 * @param tokens_count  Общее количество токенов на входе
 * @param rpn_out       Выходной статический массив
 * @return              Фактическое количество токенов, записанных в массив rpn_out
 */
size_t shunting_yard_parse(const struct Token* tokens, size_t tokens_count, struct Token* rpn_out)
{
    struct Token stack[MAX_TOKENS];
    int args_count_stack[MAX_TOKENS];
    
    size_t s_top = 0; 
    size_t r_top = 0; 
    size_t a_top = 0; 

    const struct Token* prev = NULL;
    ExpectState expect = EXPECT_OPND; 

    for (size_t i = 0; i < tokens_count; ++i)
    {
        const struct Token* token = &tokens[i];

        // Число, константа или переменная
        if (token->type == TOKEN_NUMBER || token->type == TOKEN_CONSTANT || token->type == TOKEN_VARIABLE)
        {
            if (expect != EXPECT_OPND)
                throw_syntax_error("operator", token);

            rpn_out[r_top++] = *token;
            expect = EXPECT_OPTR;
        }
        // Функция
        else if (token->type == TOKEN_FUNCTION)
        {
            if (expect != EXPECT_OPND) 
                throw_syntax_error("operand", token);
            stack[s_top++] = *token;
            args_count_stack[a_top++] = 1; 
            expect = EXPECT_LPAREN;
        }
        // Разделитель аргументов (запятая)
        else if (token->type == TOKEN_DELIMETER) 
        {
            if (expect != EXPECT_OPTR) 
                throw_syntax_error("operator", token);
            
            while (s_top > 0 && stack[s_top - 1].type != TOKEN_LEFT_PARENTHESIS)
            {
                rpn_out[r_top++] = stack[--s_top];
            }
            if (s_top == 0 || a_top == 0) 
            {
                fprintf(stderr, "Error: \"%s\" outside a function or missing '(' (%zu:%zu)\n", token->value, token->start, token->end);
                exit(EXIT_FAILURE);
            }
            args_count_stack[a_top - 1]++; 
            expect = EXPECT_OPND;
        }
        // Обработка операторов
        else if (token->type == TOKEN_OPERATOR) 
        {
            if (strcmp(token->value, "-") == 0 && is_unary(prev)) 
            {
                if (expect != EXPECT_OPND) throw_syntax_error("operand", token);
                struct Token unary_token = *token;
                strcpy(unary_token.value, "~"); 
                stack[s_top++] = unary_token;
                expect = EXPECT_OPND;
            } 
            else 
            {
                if (expect != EXPECT_OPTR) 
                    throw_syntax_error("operator", token);
                
                struct OpProperties op_props = get_op_properties(token->value);
                while (s_top > 0 && is_more_precedence(stack[s_top - 1], op_props)) 
                {
                    rpn_out[r_top++] = stack[--s_top];
                }
                stack[s_top++] = *token;
                expect = EXPECT_OPND;
            }
        }
        // Открывающая скобка
        else if (token->type == TOKEN_LEFT_PARENTHESIS) {
            // Если ожидалась скобка функции или обычный операнд — это валидно
            if (expect == EXPECT_OPTR) 
                throw_syntax_error("operator", token);
            
            stack[s_top++] = *token;
            expect = EXPECT_OPND; 
        }
        // Закрывающая скобка
        else if (token->type == TOKEN_RIGHT_PARENTHESIS) 
        {
            // Защита от пустых вызовов: fun()
            if (prev && prev->type == TOKEN_LEFT_PARENTHESIS && s_top > 1 && stack[s_top - 2].type == TOKEN_FUNCTION)
            {
                struct Token func_token = stack[s_top - 2];
                int expected_args = get_func_args(func_token.value);
                if (expected_args != 0) 
                {
                    fprintf(stderr, "Syntax error: Function \"%s\" expects %d args, but the call is empty (%zu:%zu)\n", 
                            func_token.value, expected_args, token->start, token->end);
                    exit(EXIT_FAILURE);
                }
            }

            if (expect != EXPECT_OPTR) 
                throw_syntax_error("operator", token);

            while (s_top > 0 && stack[s_top - 1].type != TOKEN_LEFT_PARENTHESIS) 
            {
                rpn_out[r_top++] = stack[--s_top];
            }
            if (s_top == 0) 
            {
                fprintf(stderr, "Error: Missing opening parenthesis '(' before ')' (%zu:%zu)\n", token->start, token->end);
                exit(EXIT_FAILURE);
            }
            s_top--; 

            if (s_top > 0 && stack[s_top - 1].type == TOKEN_FUNCTION) 
            {
                struct Token func_token = stack[s_top - 1];
                int actual_args = args_count_stack[--a_top];
                int expected_args = get_func_args(func_token.value);

                if (actual_args != expected_args) 
                {
                    fprintf(stderr, "Syntax error: Invalid number of arguments for \"%s\": expected %d, got %d (%zu:%zu)\n", 
                            func_token.value, expected_args, actual_args, token->start, token->end);
                    exit(EXIT_FAILURE);
                }
                rpn_out[r_top++] = stack[--s_top]; 
            }
            expect = EXPECT_OPTR;
        }
        prev = token;
    }

    while (s_top > 0) 
    {
        struct Token last_token = stack[--s_top];
        if (expect != EXPECT_OPTR)
        {
            fprintf(stderr, "Error: The expression was terminated unexpectedly.\n");
            exit(EXIT_FAILURE);
        }
        if (last_token.type == TOKEN_LEFT_PARENTHESIS) 
        {
            fprintf(stderr, "Syntax error: no closing ')' (%zu:%zu)\n", last_token.start, last_token.end);
            exit(EXIT_FAILURE);
        }
        rpn_out[r_top++] = last_token;
    }

    return r_top; 
}

/**
 * Вычисляет результат математического выражения
 * 
 * @param rpn       Указатель на массив токенов
 * @param rpn_count Количество токенов
 * @param x_value   Числовое значение для подстановки вместо переменной X
 * @return          Финальный вещественный результат вычисления
 */
double eval_rpn(const struct Token* rpn, size_t rpn_count, double x_value)
{
    double eval_stack[MAX_TOKENS]; // Стек для хранения промежуточных числовых результатов
    size_t top = 0;                // Указатель на вершину числового стека

    for (size_t i = 0; i < rpn_count; i++)
    {
        const struct Token* t = &rpn[i];

        // Если это число, перевд строки в double и в стек
        if (t->type == TOKEN_NUMBER)
        {
            eval_stack[top++] = atof(t->value);
        }
        // Если матем константа, подстановка её значения
        else if (t->type == TOKEN_CONSTANT) 
        {
            if (strcmp(t->value, "pi") == 0)
            {
                eval_stack[top++] = M_PI;
            } 
            else if (strcmp(t->value, "e") == 0) 
            {
                eval_stack[top++] = M_E;
            }
        }
        // Если переменная, подставить переданный x_value
        else if (t->type == TOKEN_VARIABLE)
        {
            eval_stack[top++] = x_value;
        }
        // Обработка арифметических операторов
        else if (t->type == TOKEN_OPERATOR)
        {
            // Унарный минус
            if (strcmp(t->value, "~") == 0) 
            {
                if (top < 1) 
                { 
                    fprintf(stderr, "Computation error: not enough operands for unary minus\n"); 
                    exit(EXIT_FAILURE); 
                }
                // Инвертирование знака у верхнего числа в стеке
                eval_stack[top - 1] = -eval_stack[top - 1];
            } 
            // Бинарные операторы
            else 
            {
                if (top < 2) 
                { 
                    fprintf(stderr, "Computation error: Not enough operands for '%s'\n", t->value); 
                    exit(EXIT_FAILURE); 
                }
                // первый извлеченный элемент (b) шел в выражении вторым
                double b = eval_stack[--top];
                double a = eval_stack[--top];

                if (strcmp(t->value, "+") == 0)
                    eval_stack[top++] = a + b;

                else if (strcmp(t->value, "-") == 0) 
                    eval_stack[top++] = a - b;

                else if (strcmp(t->value, "*") == 0) 
                    eval_stack[top++] = a * b;

                else if (strcmp(t->value, "/") == 0)
                {
                    if (b == 0.0)
                    { 
                        fprintf(stderr, "Error: Division by zero\n"); 
                        exit(EXIT_FAILURE); 
                    }
                    eval_stack[top++] = a / b;
                }
                else if (strcmp(t->value, "^") == 0) 
                {
                    eval_stack[top++] = pow(a, b); // pow() из math.h
                }
            }
        }
        // Обработка встроенных математических функций
        else if (t->type == TOKEN_FUNCTION) 
        {
            // Функции с 1 аргументом
            if (strcmp(t->value, "sin") == 0)
            {
                if (top < 1)
                { 
                    fprintf(stderr, "Error calculating the sin function\n"); 
                    exit(EXIT_FAILURE); 
                }
                eval_stack[top - 1] = sin(eval_stack[top - 1]);
            }
            else if (strcmp(t->value, "cos") == 0)
            {
                if (top < 1) 
                { 
                    fprintf(stderr, "Error calculating the cos function\n"); 
                    exit(EXIT_FAILURE); 
                }
                eval_stack[top - 1] = cos(eval_stack[top - 1]);
            }
            else if (strcmp(t->value, "tg") == 0) 
            {
                if (top < 1) 
                { 
                    fprintf(stderr, "Error calculating the tg function\n"); 
                    exit(EXIT_FAILURE); 
                }
                eval_stack[top - 1] = tan(eval_stack[top - 1]);
            }
            else if (strcmp(t->value, "sqrt") == 0) 
            {
                if (top < 1) 
                { 
                    fprintf(stderr, "Error calculating sqrt function\n"); 
                    exit(EXIT_FAILURE); 
                }
                if (eval_stack[top - 1] < 0) 
                { 
                    fprintf(stderr, "Error: Trying to extract the square root of a negative number\n"); 
                    exit(EXIT_FAILURE); 
                }
                eval_stack[top - 1] = sqrt(eval_stack[top - 1]);
            }
            else if (strcmp(t->value, "ln") == 0) 
            {
                if (top < 1) 
                { 
                    fprintf(stderr, "Error calculating the ln function\n"); 
                    exit(EXIT_FAILURE); 
                }
                if (eval_stack[top - 1] <= 0) 
                {
                    fprintf(stderr, "Error: The natural logarithm is only defined for positive numbers.\n");
                    exit(EXIT_FAILURE);
                }
                eval_stack[top - 1] = log(eval_stack[top - 1]);
            }
            // Функции с ДВУМЯ аргументами
            else if (strcmp(t->value, "max") == 0) 
            {
                if (top < 2) 
                { 
                    fprintf(stderr, "Error calculating the max function\n"); 
                    exit(EXIT_FAILURE); 
                }
                double b = eval_stack[--top];
                double a = eval_stack[--top];
                eval_stack[top++] = (a > b) ? a : b;
            }
            else if (strcmp(t->value, "min") == 0) 
            {
                if (top < 2) 
                { 
                    fprintf(stderr, "Error calculating the min function\n"); 
                    exit(EXIT_FAILURE); 
                }
                double b = eval_stack[--top];
                double a = eval_stack[--top];
                eval_stack[top++] = (a < b) ? a : b;
            }
        }
    }

    // после выполнения всех операций в стеке должно остаться 1 число
    if (top != 1) 
    {
        fprintf(stderr, "Internal computation error: invalid stack state at exit (%zu)\n", top);
        exit(EXIT_FAILURE);
    }

    // финальный результат на дне стека
    return eval_stack[0];
}

