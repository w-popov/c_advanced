#include "snake.h"
#include "screens.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/////////////////////////////////////////////////////////////////////
// ИГРА
////////////////////////////////////////////////////////////////////


// Д.З. Проверка корректности выбранного направления движения
int checkDirection(struct Snake *snake, int32_t key)
{
    switch (key)
    {
    case 'w':
    case 'W':
    case KEY_UP:    
        return snake->dir != DIR_DOWN;
        
    case 's':
    case 'S':
    case KEY_DOWN:  
        return snake->dir != DIR_UP;

    case 'a':
    case 'A':
    case KEY_LEFT:  
        return snake->dir != DIR_RIGHT;

    case 'd':
    case 'D':
    case KEY_RIGHT: 
        return snake->dir != DIR_LEFT;
    
    default:
        break;
    }
    return 0;
}

// Проверка занятости точки змейкой или другой видимой едой
static bool is_coordinate_busy(struct ScreenGamePlay *game, int x, int y) 
{
    // Проверка коллизии со змейкой
    for (int i = 0; i < game->snake.length; ++i) 
    {
        if (x == game->snake.body[i].x && y == game->snake.body[i].y) 
        {
            return true;
        }
    }

    // Проверка коллизии с уже сгенерированной видимой едой
    for (int i = 0; i < MAX_NUM_FOOD; ++i) 
    {
        if (game->foods[i].is_visible && x == game->foods[i].food.x && y == game->foods[i].food.y) 
        {
            return true;
        }
    }

    return false;
}

// Перегенерация одной конкретной еды по её индексу
static void generate_single_food(struct ScreenGamePlay *game, int index, time_t current_time, int max_x, int max_y) 
{
    int spawn_x, spawn_y;
    int attempts = 0; 

    do {
        spawn_x = 1 + rand() % (max_x - 2);
        spawn_y = 1 + rand() % (max_y - 2);
        attempts++;
    } while (is_coordinate_busy(game, spawn_x, spawn_y) && attempts < 100);

    game->foods[index].food.x = spawn_x;
    game->foods[index].food.y = spawn_y;
    game->foods[index].food.image = L'★';
    game->foods[index].is_enable = 1; 
    game->foods[index].is_visible = 1;
    game->foods[index].put_time = current_time;
    // Диапазон обновления от 3 до ~60 секунд
    game->foods[index].ref_time = current_time + (3 + rand() % 51);
}

// МАССИВ. Первоначальная генерация всего массива еды
void generate_foods(struct ScreenGamePlay *game) 
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

    time_t current_time = time(NULL);

    for (int i = 0; i < MAX_NUM_FOOD; ++i)
    {
        game->foods[i].is_visible = 0;
        generate_single_food(game, i, current_time, max_game_x, max_game_y);
        game->foods[i].ref_time -= rand() % 15;
        if (game->foods[i].ref_time <= current_time) 
        {
            game->foods[i].ref_time = current_time + 3; 
        }
    }
}

// ВТОРОЙ ПОТОК: Фоновое отслеживание времени обновления еды по таймерам
int food_timer_thread_func(void *arg) 
{
    struct ScreenGamePlay *game = (struct ScreenGamePlay *)arg;

    struct timespec one_second = { .tv_sec = 1, .tv_nsec = 0 };
    
    // Поток работает до тех пор, пока игра активна
    while (game->is_running) 
    {
        // sleep(1);
        thrd_sleep(&one_second, NULL);

        if (!game->is_running) break;

        int max_game_y, max_game_x;
        if (!game->subwin_game) continue;
        
        getmaxyx(game->subwin_game, max_game_y, max_game_x); 
        time_t current_time = time(NULL);
        
        // КРИТИЧЕСКАЯ СЕКЦИЯ
        mtx_lock(&game->foods_mutex);
        
        for (int i = 0; i < MAX_NUM_FOOD; ++i) 
        {
            if (game->foods[i].is_visible && current_time >= game->foods[i].ref_time) 
            {
                game->foods[i].is_visible = 0;
                generate_single_food(game, i, current_time, max_game_x, max_game_y);
            }
        }
        
        mtx_unlock(&game->foods_mutex);
    }
    
    return thrd_success;
}

#if 0

// 1ШТ. Генерация еды. ScreenGamePlay - владелец
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
#endif

// Инициализация новой игровой сессии
void init_game_session(struct ScreenGamePlay *game, int max_game_w, int max_game_h) 
{
    srand((unsigned int)time(NULL));
    game->score = 0;
    game->delay_ms = 145;    // СКОРОСТЬ ЗМЕЙКИ
    game->is_running = 1;
    game->is_pause = 0;

    mtx_init(&game->foods_mutex, mtx_plain); 

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

    // КРИТИЧЕСКАЯ СЕКЦИЯ
    mtx_lock(&game->foods_mutex);
    generate_foods(game);
    mtx_unlock(&game->foods_mutex);

    // ЗАПУСК ПОТОКА
    int result = thrd_create(&game->food_thread, food_timer_thread_func, (void *)game);
    if (result != thrd_success) 
    {
       
        mvprintw(0, 0, "КРИТИЧЕСКАЯ ОШИБКА: Поток не может быть создан! Код ошибки: %d", result);
        refresh();
        sleep(3);

        game->is_running = 0;
        mtx_destroy(&game->foods_mutex);
    }

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

    int max_game_y, max_game_x;
    if (!game->subwin_game) return;
    getmaxyx(game->subwin_game, max_game_y, max_game_x);
    time_t current_time = time(NULL);

    // КРИТИЧЕСКАЯ СЕКЦИЯ
    mtx_lock(&game->foods_mutex);
    
    for (int i = 0; i < MAX_NUM_FOOD; ++i)
    {
        if (game->foods[i].is_visible && game->foods[i].is_enable)
        {
            // Если голова наступила на текущую еду
            if (game->snake.body[0].x == game->foods[i].food.x && game->snake.body[0].y == game->foods[i].food.y)
            {
                game->score += 10;
                game->foods[i].is_enable = 0;
                game->foods[i].is_visible = 0;

                // Увеличение длины змейки
                if (game->snake.length < MAX_SNAKE_LENGTH) 
                {
                    game->snake.body[game->snake.length].x = old_tail_x;
                    game->snake.body[game->snake.length].y = old_tail_y;
                    game->snake.body[game->snake.length].image = L'○';
                    game->snake.length++;
                }
                generate_single_food(game, i, current_time, max_game_x, max_game_y);
                break;
            }
        }
    }
    
    mtx_unlock(&game->foods_mutex);
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

// Рендеринг еды
void render_foods(struct ScreenGamePlay *game)
{
    // КРИТИЧЕСКАЯ СЕКЦИЯ
    mtx_lock(&game->foods_mutex);
    
    for (int i = 0; i < MAX_NUM_FOOD; ++i)
    {
        if (game->foods[i].is_visible)
        {
            mvwaddnwstr(game->subwin_game, game->foods[i].food.y, game->foods[i].food.x, &(game->foods[i].food.image), 1);
        }
    }
    
    mtx_unlock(&game->foods_mutex);
}

// Остановка фонового потока и очистка мьютекса
void stop_game_session(struct ScreenGamePlay *game)
{
    if (!game->is_running)
        return;
    game->is_running = 0;
    int thread_result;
    thrd_join(game->food_thread, &thread_result);
    mtx_destroy(&game->foods_mutex);
}


