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
#define MAX_NUM_FOOD            14

/* Убить 2 змейку набрав очков */
#define SCORE_DEAD_SNAKE        30

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
    struct Pixel body[MAX_SNAKE_LENGTH];    // Тело
    unsigned is_alive;                      // Жива, показывать?
    unsigned is_main;
    int length;                             // Длина змейки
    int delay_ms;                           // Задержка движения
    int score;                              // Очки
    Direction dir;                          // Направление движения                      
};

// Обновление состояния 1 змейки на каждом шаге
void update_snake_step(struct ScreenGamePlay *game);

// Обновление состояния 2 змейки на каждом шаге
void update_two_snake_step(struct ScreenGamePlay *game);

// Проверка столкновений змейки с границами и самой собой
bool check_collisions(struct ScreenGamePlay *game);

// Автопилот 2 змейки
void auto_change_direction(struct ScreenGamePlay *game);

// Генерация массива новой еды в случайных позициях
void generate_foods(struct ScreenGamePlay *game);

// Рендеринг еды
void render_foods(struct ScreenGamePlay *game);

// Д.З добавить проверку корректного направления движения змейки
int checkDirection(struct Snake*, int32_t);

// Остановка фонового потока и очистка мьютекса
void stop_game_session(struct ScreenGamePlay *game);


#endif
