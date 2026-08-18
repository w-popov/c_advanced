#ifndef _SNAKE_GAME_H_
#define _SNAKE_GAME_H_

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

struct ScreenGamePlay;

/* Максимально возможная длина змейки */
#define MAX_SNAKE_LENGTH 100

/* Структура для представления объекта на экране */
struct Pixel
{
    int x;          
    int y;          
    wchar_t image;  
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

// Проверка, съела ли змейка еду
// bool check_food_collision(struct ScreenGamePlay *game);

// Генерация новой еды в случайной позиции
void generate_food(struct ScreenGamePlay *game);

// Д.З добавить проверку корректного направления движения змейки
int checkDirection(struct Snake*, int32_t);



#endif
