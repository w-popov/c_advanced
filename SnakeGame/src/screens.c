#include "screens.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////////
// ЭКРАНЫ
/////////////////////////////////////////////////////////////////////

// Элементы меню
const wchar_t* menu_choices[MENU_CHOICES_COUNT] = {
    L"Играть",
    L"Выход"
};

// Элементы всплывающего ESC-меню
const wchar_t* overlay_choices[OVERLAY_CHOICES_COUNT] = {
    L"Продолжить",
    L"В главное меню",
    L"Выход"
};

// Отрисовка всплывающего ESC-меню
void draw_overlay_menu(struct AppContext *app) 
{
    if (!app->overlay.is_visible) 
    {
        if (app->overlay.win) 
        {
            delwin(app->overlay.win);
            app->overlay.win = NULL;
            touchwin(stdscr);
        }
        return;
    }

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int start_y = (max_y - OVERLAY_MENU_HEIGHT) / 2;
    int start_x = (max_x - MENU_WIDTH) / 2;

    if (!app->overlay.win) 
    {
        app->overlay.win = newwin(OVERLAY_MENU_HEIGHT, MENU_WIDTH, start_y, start_x);
        keypad(app->overlay.win, TRUE);
    }

    werase(app->overlay.win);
    box(app->overlay.win, 0, 0);
    
    wmove(app->overlay.win, 0, 2);
    waddwstr(app->overlay.win, L" Пауза [ESC] ");

    for (int i = 0; i < OVERLAY_CHOICES_COUNT; ++i) 
    {
        wmove(app->overlay.win, i + 2, 2);
        if (app->overlay.highlight == i) 
        {
            wattron(app->overlay.win, A_REVERSE);
            waddwstr(app->overlay.win, overlay_choices[i]);
            wattroff(app->overlay.win, A_REVERSE);
        } 
        else 
        {
            waddwstr(app->overlay.win, overlay_choices[i]);
        }
    }
    wnoutrefresh(app->overlay.win);
}

// ОБРАБОТКА КЛАВИШ ЭКРАНА ГЛАВНОГО МЕНЮ
void menu_handle_input(struct AppContext *app, int ch) 
{
    struct ScreenMenu *menu = &app->screens.menu_screen;
    switch(ch) 
    {
        case KEY_UP:
            menu->highlight = (menu->highlight == 0) ? MENU_CHOICES_COUNT - 1 : menu->highlight - 1;
            break;
        case KEY_DOWN:
            menu->highlight = (menu->highlight == MENU_CHOICES_COUNT - 1) ? 0 : menu->highlight + 1;
            break;
        case 10: // Enter
            if (menu->highlight == 0) 
            {
                int max_y, max_x;
                getmaxyx(stdscr, max_y, max_x);
                stop_game_session(&app->screens.gameplay_screen);
                init_game_session(&app->screens.gameplay_screen, max_x - 4, max_y - 1);
                app_switch_screen(app, (struct I_GameScreen*)&app->screens.gameplay_screen);
            } 
            else if (menu->highlight == 1) 
            {
                stop_game_session(&app->screens.gameplay_screen);
                app->is_running = 0;
            }
            break;
    }
}

// ОТРИСОВКА ЭКРАНА ГЛАВНОГО МЕНЮ
void menu_render(struct AppContext *app) 
{
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int start_y = (max_y - MENU_HEIGHT) / 2;
    int start_x = (max_x - MENU_WIDTH) / 2;

    struct ScreenMenu *menu = &app->screens.menu_screen;

    werase(stdscr);
    if (!menu->win) 
    {
        menu->win = newwin(MENU_HEIGHT, MENU_WIDTH, start_y, start_x);
        keypad(menu->win, TRUE); 
    }

    werase(menu->win);
    box(menu->win, 0, 0);
    wmove(menu->win, 0, (MENU_WIDTH - 8) / 2);
    waddwstr(menu->win, L" ЗМЕЙКА ");

    for (int i = 0; i < MENU_CHOICES_COUNT; ++i) 
    {
        wmove(menu->win, 3 + i, 5);
        
        if (menu->highlight == i) 
        {
            wattron(menu->win, A_REVERSE);
            waddwstr(menu->win, menu_choices[i]);
            wattroff(menu->win, A_REVERSE);
        } 
        else 
        {
            waddwstr(menu->win, menu_choices[i]);
        }
    }

    wnoutrefresh(stdscr);
    wnoutrefresh(menu->win);
}

// Очистка ресурсов экрана меню
void menu_clean(struct AppContext *app) 
{ 
    struct ScreenMenu *menu = &app->screens.menu_screen;
    if (menu->win) 
    {
        delwin(menu->win);
        menu->win = NULL;
    } 
}

// ИГРОВОЙ ЭКРАН обработка клавиш
void game_handle_input(struct AppContext *app, int ch) 
{
    struct ScreenGamePlay *game = &app->screens.gameplay_screen;
    int check_dir = checkDirection(&game->snake, (int32_t)ch);

    switch (ch) 
    {
        case 'w':
        case 'W':
        case KEY_UP:    
            if (check_dir) 
            {
                game->snake.dir = DIR_UP;
            }    
            break;
        case 's':
        case 'S':
        case KEY_DOWN:  
            if (check_dir) 
            {
                game->snake.dir = DIR_DOWN;
            }  
            break;
        case 'a':
        case 'A':
        case KEY_LEFT:  
            if (check_dir) 
            {
                game->snake.dir = DIR_LEFT;
            }  
            break;
        case 'd':
        case 'D':
        case KEY_RIGHT: 
            if (check_dir) 
            {
                game->snake.dir = DIR_RIGHT;
            } 
            break;
        default:
            break;
    }
}

// ОТРИСОВКА ЭКРАНА ИГРЫ
void game_render(struct AppContext *app) 
{
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    struct ScreenGamePlay *game = &app->screens.gameplay_screen;

    int status_h = 3;
    int game_h = max_y - status_h - 2;
    int win_w = max_x - 4;

    if (!game->subwin_status || !game->subwin_game) 
    {
        if (game->subwin_status) delwin(game->subwin_status);
        if (game->subwin_game) delwin(game->subwin_game);

        game->subwin_status = newwin(status_h, win_w, 1, 2);
        game->subwin_game   = newwin(game_h, win_w, 1 + status_h, 2);
        
        keypad(game->subwin_game, TRUE);
        nodelay(game->subwin_game, TRUE);
    }

    werase(stdscr); 

    // Верхнее окно: статус бар
    werase(game->subwin_status);
    box(game->subwin_status, 0, 0);
    wmove(game->subwin_status, 1, 2);
    wchar_t status_buf[128];

    swprintf(status_buf, 128, L"Очки: %04d  |  [ESC] - Пауза |", game->snake.score);
    waddwstr(game->subwin_status, status_buf);

    // Нижнее окно: поле игры
    werase(game->subwin_game);
    box(game->subwin_game, 0, 0);

    render_foods(game);

    // Основная змейка - ОБНОВИТЬ
    wattron(game->subwin_game, COLOR_PAIR(1));
    for (int i = 0; i < game->snake.length; ++i) 
    {
        int sx = game->snake.body[i].x;
        int sy = game->snake.body[i].y;
        unsigned snake_valid_position = sx > 0 && sx < win_w - 1 && sy > 0 && sy < game_h - 1;

        if (snake_valid_position && game->snake.is_alive) 
        {
            wmove(game->subwin_game, sy, sx);
            if (i == 0) 
            {
                wattron(game->subwin_game, A_BOLD);
                waddnwstr(game->subwin_game, &game->snake.body[i].image, 1);
                wattroff(game->subwin_game, A_BOLD);
            } 
            else 
            {
                waddnwstr(game->subwin_game, &game->snake.body[i].image, 1);
            }
        }
    }
    wattroff(game->subwin_game, COLOR_PAIR(1));
    // 2 змейка ОБНОВИТЬ
    wattron(game->subwin_game, COLOR_PAIR(3));
    for (int i = 0; i < game->two_snake.length; ++i)
    {
        int sx2 = game->two_snake.body[i].x;
        int sy2 = game->two_snake.body[i].y;
        unsigned snake_2_valid_position = sx2 > 0 && sx2 < win_w - 1 && sy2 > 0 && sy2 < game_h - 1;

        if (snake_2_valid_position && game->two_snake.is_alive) 
        {
            wmove(game->subwin_game, sy2, sx2);
            if (i == 0) 
            {
                wattron(game->subwin_game, A_BOLD);
                waddnwstr(game->subwin_game, &game->two_snake.body[i].image, 1);
                wattroff(game->subwin_game, A_BOLD);
            } 
            else 
            {
                waddnwstr(game->subwin_game, &game->two_snake.body[i].image, 1);
            }

        }
    }
    wattroff(game->subwin_game, COLOR_PAIR(3));

    // КРИТИЧЕСКАЯ СЕКЦИЯ полет пули
    wattron(game->subwin_game, COLOR_PAIR(2));
    mtx_lock(&game->bullets_mutex);
    for (int i = 0; i < MAX_BULLETS; ++i) 
    {
        if (game->bullets[i].is_active) 
        {
            wmove(game->subwin_game, game->bullets[i].bullet.y, game->bullets[i].bullet.x);
            waddnwstr(game->subwin_game, &game->bullets[i].bullet.image, 1);
        }
    }
    mtx_unlock(&game->bullets_mutex);
    wattroff(game->subwin_game, COLOR_PAIR(2));

    wnoutrefresh(stdscr);
    wnoutrefresh(game->subwin_status);
    wnoutrefresh(game->subwin_game);
}

// Очистка ресурсов экрана игры
void game_clean(struct AppContext *app) 
{
    struct ScreenGamePlay *game = &app->screens.gameplay_screen;
    if (game->subwin_status) 
    { 
        delwin(game->subwin_status); 
        game->subwin_status = NULL; 
    }
    if (game->subwin_game)
    { 
        delwin(game->subwin_game);   
        game->subwin_game = NULL;
    }
}

// ИНИЦИАЛИЗАЦИЯ
void app_init(struct AppContext *app) 
{
    // Настройка интерфейсных методов для экрана Меню
    app->screens.menu_screen.game_screen.render       = menu_render;
    app->screens.menu_screen.game_screen.clean        = menu_clean;
    app->screens.menu_screen.game_screen.handle_input = menu_handle_input;
    app->screens.menu_screen.highlight = 0;
    app->screens.menu_screen.win = NULL;

    // Настройка интерфейсных методов для экрана Игры
    app->screens.gameplay_screen.game_screen.render       = game_render;
    app->screens.gameplay_screen.game_screen.clean        = game_clean;
    app->screens.gameplay_screen.game_screen.handle_input = game_handle_input;
    app->screens.gameplay_screen.subwin_status            = NULL;
    app->screens.gameplay_screen.subwin_game              = NULL;

    app->overlay.win = NULL;
    app->overlay.is_visible = false;
    app->overlay.highlight = 0;

    app->screens.gameplay_screen.is_pause = 0;
    app->is_running = 1;

    // Передача указателя на нажатую клавишу для 2 потоков змеек
    app->screens.gameplay_screen.ch_key = &(app->shared_ch_key);
    // Передача указателя на мьютекс клавиш управления
    app->screens.gameplay_screen.ptr_ch_key_mutex = &(app->ch_key_mutex);

    // Старт с экрана меню
    app->screens.current_screen = (struct I_GameScreen*)&app->screens.menu_screen;
}

// Переключение между экранами
void app_switch_screen(struct AppContext *app, struct I_GameScreen *new_screen) 
{
    if (app->screens.current_screen && app->screens.current_screen->clean) 
    {
        app->screens.current_screen->clean(app);
    }
    app->screens.current_screen = new_screen;
}

// Завершение работы приложения
void app_destroy(struct AppContext *app) 
{
    if (app->screens.current_screen && app->screens.current_screen->clean) 
    {
        app->screens.current_screen->clean(app);
    }
    if (app->overlay.win) 
    {
        delwin(app->overlay.win);
    }
}