#include "snake.h"
#include "screens.h"
#include <stdlib.h>
#include <time.h>

/////////////////////////////////////////////////////////////////////
// ИГРА
////////////////////////////////////////////////////////////////////

// Генерация еды
void generate_food(struct ScreenGamePlay *game) 
{
    int max_game_y, max_game_x;
    if (game->subwin_game) 
    {
        getmaxyx(game->subwin_game, max_game_y, max_game_x);
    } 
    else 
    {
        return;
    }

    bool on_snake;
    do {
        on_snake = false;
        game->food.x = 1 + rand() % (max_game_x - 2);
        game->food.y = 1 + rand() % (max_game_y - 2);

        for (int i = 0; i < game->snake.length; ++i) 
        {
            if (game->food.x == game->snake.body[i].x && game->food.y == game->snake.body[i].y) 
            {
                on_snake = true;
                break;
            }
        }
    } while (on_snake);

    game->food.image = L'★';
}

// Инициализация новой игровой сессии
void init_game_session(struct ScreenGamePlay *game, int max_game_w, int max_game_h) 
{
    srand((unsigned int)time(NULL));
    game->score = 0;
    game->delay_ms = 145;    // СКОРОСТЬ ЗМЕЙКИ

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int status_h = 3;
    int game_h = max_y - status_h - 2;
    int win_w = max_x - 4;

    if (!game->subwin_status) 
    {
        game->subwin_status = newwin(status_h, win_w, 1, 2);
        game->subwin_game   = newwin(game_h, win_w, 1 + status_h, 2);
    }

    int start_x = win_w / 2;
    int start_y = game_h / 2;

    game->snake.length = 3;
    game->snake.dir = DIR_RIGHT;
    game->snake.body[0].x = start_x;
    game->snake.body[0].y = start_y;
    
    game->snake.body[0].image = L'●';

    for (int i = 1; i < game->snake.length; ++i) 
    {
        game->snake.body[i].x = start_x - i;
        game->snake.body[i].y = start_y;
        game->snake.body[i].image = L'○';
    }

    generate_food(game);
}

// Физический шаг движения змейки
void update_snake_step(struct ScreenGamePlay *game) 
{
    int old_tail_x = game->snake.body[game->snake.length - 1].x;
    int old_tail_y = game->snake.body[game->snake.length - 1].y;

    // Сдвиг сегментов назад
    for (int i = game->snake.length - 1; i > 0; --i) 
    {
        game->snake.body[i].x = game->snake.body[i - 1].x;
        game->snake.body[i].y = game->snake.body[i - 1].y;
    }

    // Сдвиг головы вперед
    switch (game->snake.dir) 
    {
        case DIR_UP:    game->snake.body[0].y -= 1; break;
        case DIR_DOWN:  game->snake.body[0].y += 1; break;
        case DIR_LEFT:  game->snake.body[0].x -= 1; break;
        case DIR_RIGHT: game->snake.body[0].x += 1; break;
    }

    // Обработка съедания яблока
    if (game->snake.body[0].x == game->food.x && game->snake.body[0].y == game->food.y) 
    {
        game->score += 10;

        if (game->snake.length < MAX_SNAKE_LENGTH) 
        {
            game->snake.body[game->snake.length].x = old_tail_x;
            game->snake.body[game->snake.length].y = old_tail_y;
            game->snake.body[game->snake.length].image = L'○';
            game->snake.length++;
        }

        generate_food(game);
    }
}

// Проверка столкновений со стеной / хвостом
bool check_collisions(struct ScreenGamePlay *game) 
{
    int max_game_y, max_game_x;
    if (game->subwin_game) 
    {
        getmaxyx(game->subwin_game, max_game_y, max_game_x);
    } 
    else 
    {
        return false;
    }

    int head_x = game->snake.body[0].x;
    int head_y = game->snake.body[0].y;

    if (head_x <= 0 || head_x >= max_game_x - 1 || head_y <= 0 || head_y >= max_game_y - 1) 
    {
        return true; 
    }

    for (int i = 4; i < game->snake.length; ++i) 
    {
        if (head_x == game->snake.body[i].x && head_y == game->snake.body[i].y) 
        {
            return true; 
        }
    }

    return false;
}


