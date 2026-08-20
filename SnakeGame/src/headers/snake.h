#ifndef _SNAKE_GAME_H_
#define _SNAKE_GAME_H_

#ifdef _WIN32
    #define PDC_WIDE
    #define PDC_FORCE_UTF8
    #include <curses.h>
#else
    #define _XOPEN_SOURCE_EXTENDED 1
    #include <ncurses.h>
    #endif
    
#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>
#include <time.h>
#include "tinycthread.h"

struct ScreenGamePlay;

/* Максимально возможная длина змейки */
#define MAX_SNAKE_LENGTH        100

/* Макс. кол-во единиц еды */
#define MAX_NUM_FOOD            24

/* Структура для представления объекта на экране */
struct Pixel
{
    int x;          
    int y;          
    wchar_t image;  
};

/* Еда */
struct Food 
{ 
    struct Pixel food;  // Координаты, внеш. вид    
    time_t put_time;    // Время создания
    time_t ref_time;    // Время обновления
    uint8_t is_enable;  // Съедена?
    uint8_t is_visible; // Показывать?
};

/* Направления движения змейки */
typedef enum 
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

/* Структура змейки */
struct Snake
{
    struct Pixel body[MAX_SNAKE_LENGTH]; 
    int length;                          
    Direction dir;                       
};

// Обновление состояния змейки на каждом шаге
void update_snake_step(struct ScreenGamePlay *game);

// Проверка столкновений змейки с границами и самой собой
bool check_collisions(struct ScreenGamePlay *game);

// Генерация новой еды в случайной позиции
// void generate_food(struct ScreenGamePlay *game);

// Генерация массива новой еды в случайных позициях
void generate_foods(struct ScreenGamePlay *game);

// Рендеринг еды
void render_foods(struct ScreenGamePlay *game);

// Д.З добавить проверку корректного направления движения змейки
int checkDirection(struct Snake*, int32_t);

// Остановка фонового потока и очистка мьютекса
void stop_game_session(struct ScreenGamePlay *game);


#endif
