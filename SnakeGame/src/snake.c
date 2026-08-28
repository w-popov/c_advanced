#include "snake.h"
#include "screens.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

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

////////////////////////////////////////////////////////////////////////
// ВТОРОЙ ПОТОК: Фоновое отслеживание времени обновления еды по таймерам
////////////////////////////////////////////////////////////////////////
int food_timer_thread_func (void *arg) 
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

// вычисляет количество ходов до еды
static int distance(const struct Snake *snake, const struct Food *f)
{
    return (abs(snake->body[0].x - f->food.x) + abs(snake->body[0].y - f->food.y));
}

// Автопилот 2 змейки
void auto_change_direction(struct ScreenGamePlay *game)
{
    int pointer = 0; 
    for (int i = 1; i < MAX_NUM_FOOD; ++i) 
    { 
        pointer = (distance(&game->two_snake, &game->foods[i]) < distance(&game->two_snake, &game->foods[pointer])) ? i : pointer; 
    } 
    if ((game->two_snake.dir == DIR_RIGHT || game->two_snake.dir == DIR_LEFT) && (game->two_snake.body[0].y != game->foods[pointer].food.y)) 
    {  // горизонтальное движение 
        game->two_snake.dir = (game->foods[pointer].food.y > game->two_snake.body[0].y) ? DIR_DOWN : DIR_UP; 
    } 
    else if ((game->two_snake.dir == DIR_DOWN || game->two_snake.dir == DIR_UP) && (game->two_snake.body[0].x != game->foods[pointer].food.x)) 
    {  // вертикальное движение 
        game->two_snake.dir = (game->foods[pointer].food.x > game->two_snake.body[0].x) ? DIR_RIGHT : DIR_LEFT; 
    } 
}

//////////////////////////////////////////////////////////////////
// ТРЕТИЙ ПОТОК 2 змейка
//////////////////////////////////////////////////////////////////
int two_snake_thread_func (void *arg)
{
    struct ScreenGamePlay *game = (struct ScreenGamePlay *)arg;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 }; // 10 мс

    double time_speed = 1000.0;

    #ifndef _WIN32 
    time_speed = 10000.0;
    #endif
    clock_t last_snake_move_time = clock();
    clock_t last_shoot_time = clock();
    double next_shoot_interval = 0.1 + (double)(rand() % 41) / 100.0;

    while (game->is_running) 
    {
        if (!game->is_running || !game->two_snake.is_alive) break;

        // int key = 0;
        // // КРИТИЧЕСКАЯ СЕКЦИЯ получить нажатую клавишу с гл. потока
        // mtx_lock(game->ptr_ch_key_mutex);
        // if (game->ch_key)
        // {
        //     key = *(game->ch_key); 
        // }
        // mtx_unlock(game->ptr_ch_key_mutex);

        clock_t current_time = clock();
        double elapsed_seconds = (double)(current_time - last_snake_move_time) / CLOCKS_PER_SEC;
        double required_delay_seconds = (double)game->two_snake.delay_ms / time_speed;

        double elapsed_shoot_seconds = (double)(current_time - last_shoot_time) / CLOCKS_PER_SEC;
        if (elapsed_shoot_seconds >= next_shoot_interval)
        {
            fire(game);

            last_shoot_time = current_time;
            next_shoot_interval = 0.1 + (double)(rand() % 41) / 100.0;
        }

        if (elapsed_seconds >= required_delay_seconds)
        {
            auto_change_direction(game);    // автопилот
            update_two_snake_step(game);    // сделать шаг
            last_snake_move_time = current_time;
        }

        thrd_sleep(&ts, NULL);
    }
    
    return thrd_success;
}

// Инициализация новой игровой сессии, запуск потоков
void init_game_session(struct ScreenGamePlay *game, int max_game_w, int max_game_h) 
{
    srand((unsigned int)time(NULL));
    game->snake.score = 0;
    game->snake.delay_ms = 80;      // СКОРОСТЬ 1 ЗМЕЙКИ
    game->two_snake.delay_ms = 150; // СКОРОСТЬ 2 ЗМЕЙКИ
    
    game->is_running = 1;
    game->is_pause = 0;

    /* Мьютекс потока еды */
    mtx_init(&game->foods_mutex, mtx_plain);
    /* Мьютекс потока 2 змейки */
    mtx_init(&game->two_snake_mutex, mtx_plain);
    /* Мьютекс пуль */
    mtx_init(&game->bullets_mutex, mtx_plain);

    // Очистка массива пуль
    for (int i = 0; i < MAX_BULLETS; ++i) 
    {
        game->bullets[i].is_active = 0;
    }
    
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

    // Основная змейка
    game->snake.is_alive = 1;
    game->snake.length = 3;
    game->snake.dir = DIR_RIGHT;
    game->snake.body[0].x = start_x;
    game->snake.body[0].y = start_y;
    game->snake.is_main = 1;

    // 2 змейка
    game->two_snake.is_alive = 1;
    game->two_snake.length = 3;
    game->two_snake.dir = DIR_RIGHT;
    game->two_snake.body[0].x = start_x - 2;
    game->two_snake.body[0].y = start_y - 2;
    game->two_snake.is_main = 0;
    
    game->snake.body[0].image = L'●';
    game->two_snake.body[0].image = L'■';

    for (int i = 1; i < game->snake.length; ++i) 
    {
        game->snake.body[i].x = start_x - i;
        game->snake.body[i].y = start_y;
        game->snake.body[i].image = L'○';

        game->two_snake.body[i].x = start_x - i - 2;
        game->two_snake.body[i].y = start_y - 2;
        game->two_snake.body[i].image = L'□';
    }

    // КРИТИЧЕСКАЯ СЕКЦИЯ
    mtx_lock(&game->foods_mutex);
    generate_foods(game);
    mtx_unlock(&game->foods_mutex);

    // ЗАПУСК ПОТОКА ЕДЫ
    int result = thrd_create(&game->food_thread, food_timer_thread_func, (void *)game);
    if (result != thrd_success) 
    {
       
        mvprintw(0, 0, "КРИТИЧЕСКАЯ ОШИБКА: Поток не может быть создан! Код ошибки: %d", result);
        refresh();
        sleep(3);

        game->is_running = 0;
        mtx_destroy(&game->foods_mutex);
    }

    // ЗАПУСК ПОТОКА 2 ЗМЕЙКИ
    int res = thrd_create(&game->two_snake_thrd, two_snake_thread_func, (void *)game);
    if (res != thrd_success) 
    {
       
        mvprintw(0, 0, "КРИТИЧЕСКАЯ ОШИБКА: Поток не может быть создан! Код ошибки: %d", result);
        refresh();
        sleep(3);
        mtx_destroy(&game->two_snake_mutex);
    }
}

// Шаг змейки
static void snake_step(struct ScreenGamePlay *game, struct Snake *snake)
{
    if (!game || !snake || !game->subwin_game) return;
    if (snake->length <= 0 || snake->length > MAX_SNAKE_LENGTH) return;

    int old_tail_x = snake->body[snake->length - 1].x;
    int old_tail_y = snake->body[snake->length - 1].y;

    // Сдвиг сегментов назад
    for (int i = snake->length - 1; i > 0; --i) 
    {
        snake->body[i].x = snake->body[i - 1].x;
        snake->body[i].y = snake->body[i - 1].y;
    }

    // Сдвиг головы вперед
    switch (snake->dir) 
    {
        case DIR_UP:    snake->body[0].y -= 1; break;
        case DIR_DOWN:  snake->body[0].y += 1; break;
        case DIR_LEFT:  snake->body[0].x -= 1; break;
        case DIR_RIGHT: snake->body[0].x += 1; break;
    }

    int max_game_y, max_game_x;
    getmaxyx(game->subwin_game, max_game_y, max_game_x);
    time_t current_time = time(NULL);

    // КРИТИЧЕСКАЯ СЕКЦИЯ
    mtx_lock(&game->foods_mutex);
    for (int i = 0; i < MAX_NUM_FOOD; ++i)
    {
        if (game->foods[i].is_visible && game->foods[i].is_enable)
        {
            // Если голова наступила на текущую еду
            if (snake->body[0].x == game->foods[i].food.x && snake->body[0].y == game->foods[i].food.y)
            {
                snake->score += 10;
                game->foods[i].is_enable = 0;
                game->foods[i].is_visible = 0;

                // Увеличение длины змейки
                if (snake->length < MAX_SNAKE_LENGTH) 
                {
                    snake->body[snake->length].x = old_tail_x;
                    snake->body[snake->length].y = old_tail_y;
                    snake->body[snake->length].image = snake->is_main ? L'○' : L'□';
                    snake->length += 1;
                }
                generate_single_food(game, i, current_time, max_game_x, max_game_y);
                break;
            }
        }
    }
    mtx_unlock(&game->foods_mutex);
}


// Физический шаг движения 2 змейки
void update_two_snake_step(struct ScreenGamePlay *game)
{
    snake_step(game, &game->two_snake);
}


// Физический шаг движения 1 змейки
void update_snake_step(struct ScreenGamePlay *game) 
{
    snake_step(game, &game->snake);

    /* Если набрано SCORE_DEAD_SNAKE очков, то убить 2 змейку */
    if (game->two_snake.is_alive)
    {
        // КРИТИЧЕСКАЯ СЕКЦИЯ скрыть 2 змейку
        mtx_lock(&game->two_snake_mutex);
        if (game->snake.score > SCORE_DEAD_SNAKE)
        {
            game->two_snake.is_alive = 0;
            game->snake.delay_ms += 5;
        }
        mtx_unlock(&game->two_snake_mutex);
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

// Стрелять
void fire(struct ScreenGamePlay *game)
{
    mtx_lock(&game->bullets_mutex);
    for (int i = 0; i < MAX_BULLETS; ++i) 
    {
        if (!game->bullets[i].is_active) 
        {
            game->bullets[i].bullet.x = game->two_snake.body[0].x;
            game->bullets[i].bullet.y = game->two_snake.body[0].y;
            game->bullets[i].bullet.image = L'*';
            game->bullets[i].dir = game->two_snake.dir;
            game->bullets[i].is_active = 1;
            break;
        }
    }
    mtx_unlock(&game->bullets_mutex);
}

// Движение пуль
void update_bullets(struct ScreenGamePlay *game)
{
    if (!game || !game->subwin_game) return;

    int max_x, max_y;
    getmaxyx(game->subwin_game, max_y, max_x);

    mtx_lock(&game->bullets_mutex);
    for (int i = 0; i < MAX_BULLETS; ++i) 
    {
        if (!game->bullets[i].is_active) continue;
        mvwaddch(game->subwin_game, game->bullets[i].bullet.y, game->bullets[i].bullet.x, ' ');

        switch (game->bullets[i].dir) 
        {
            case DIR_UP:    game->bullets[i].bullet.y -= 1; break;
            case DIR_DOWN:  game->bullets[i].bullet.y += 1; break;
            case DIR_LEFT:  game->bullets[i].bullet.x -= 1; break;
            case DIR_RIGHT: game->bullets[i].bullet.x += 1; break;
        }

        // проверка попадания в основную змейку
        if (game->snake.is_alive)
        {
            for (int j = 0; j < game->snake.length; ++j)
            {
                if (game->bullets[i].bullet.x == game->snake.body[j].x &&
                    game->bullets[i].bullet.y == game->snake.body[j].y)
                {
                    game->snake.is_alive = 0;
                    game->bullets[i].is_active = 0;
                    break;
                }
            }
        }

        if (game->bullets[i].bullet.x <= 0 || game->bullets[i].bullet.x >= max_x - 1 ||
            game->bullets[i].bullet.y <= 0 || game->bullets[i].bullet.y >= max_y - 1) 
        {
            game->bullets[i].is_active = 0;
        }
    }
    mtx_unlock(&game->bullets_mutex);
}

// Остановка фоновых потоков и очистка мьютекса
void stop_game_session(struct ScreenGamePlay *game)
{
    if (!game->is_running)
        return;
    game->is_running = 0;
    int food_thread_result, snake_thread_result;
    thrd_join(game->food_thread, &food_thread_result);
    thrd_join(game->two_snake_thrd, &snake_thread_result);
    mtx_destroy(&game->foods_mutex);
    mtx_destroy(&game->two_snake_mutex);
}


