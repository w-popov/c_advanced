#ifndef _MATH_PARSER_H_
#define _MATH_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#ifndef M_E
    #define M_E 2.71828182845904523536
#endif

// ======================== ТОКЕНИЗАТОР ================================

// Максимальное количество токенов в выражении
#define MAX_TOKENS      256

// Максимальная длина одного токена
#define MAX_VAL_LEN     128

// Перечисление типов токенов
typedef enum 
{
    TOKEN_LEFT_PARENTHESIS,  // (
    TOKEN_RIGHT_PARENTHESIS, // )
    TOKEN_DELIMETER,         // , (разделитель аргументов)
    TOKEN_OPERATOR,          // Операторы: +, -, *, /, ^
    TOKEN_FUNCTION,          // Функции: sin, cos, max..
    TOKEN_CONSTANT,          // Константы: pi, e
    TOKEN_NUMBER,            // Числа: 2, 3.14
    TOKEN_VARIABLE           // Переменные: x, y
} TokenType;

// Структура токена
struct Token
{
    TokenType type;          // Тип токена
    char value[MAX_VAL_LEN]; // Строковое значение
    size_t start;            // Индекс начала токена в исходной строке
    size_t end;              // Индекс конца токена
};


// Является ли строка именем известной функции
int is_function(const char* name);

// Является ли строка именем известной константы
int is_constant(const char* name);

/**
 * Основная функция токенизации
 * @param expression — исходная математическая строка
 * @param tokens_out — массив структур Token, куда запишется результат
 * @return Количество успешно распознанных токенов
 */
size_t tokenize(const char* expression, struct Token* tokens_out);

// Для отладочного вывода токенов в консоль
void print_tokens(const struct Token* tokens, size_t count);

// ========================= СОРТИРОВОЧНАЯ СТАНЦИЯ ===============================

// Ошибки
typedef enum 
{
    EXPECT_OPND,        // Ожидание операнда (число, функция, - ...)
    EXPECT_OPTR,        // Ожидание оператора (+, -, *, ), /)
    EXPECT_LPAREN       // Ожидание скобку функции (
} ExpectState;

// Ассоциативность
typedef enum 
{ 
    ASSOC_LEFT, 
    ASSOC_RIGHT 
} Associativity;

// Свойства оператора
struct OpProperties
{
    int precedence;             // приоритет
    Associativity associative;
};

// Свойства функции 
struct FuncProperties
{
    char name[MAX_VAL_LEN];
    int args;
};

// Поиск свойств оператора
struct OpProperties get_op_properties(const char* op);

// Поиск количества аргументов функции (-1 если не найдена)
int get_func_args(const char* name);

// Функция аварийного выхода при ошибках синтаксиса
void throw_syntax_error(const char* expected, const struct Token* tok);

int is_unary(const struct Token* prev);

// Определение приоритета
int is_more_precedence(struct Token top, struct OpProperties op_props);

// Алгоритм
size_t shunting_yard_parse(const struct Token* tokens, size_t tokens_count, struct Token* rpn_out);

// Компиляция выражения обратной польсой записи
size_t compile_to_rpn(wchar_t *expr, struct Token* rpn_out);

// Вычислить rpn (eval)
double eval_rpn(const struct Token* rpn, size_t rpn_count, double x_value);

#ifdef __cplusplus
}
#endif

#endif