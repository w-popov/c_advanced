#include "screens.h"
#include "snake.h"
#include <locale.h>
#include <time.h> 

int main(void) 
{
    setlocale(LC_ALL, "");
    initscr();
    clear();
    start_color();
    use_default_colors();
    assume_default_colors(COLOR_WHITE, COLOR_BLACK);
    // Цвет пары:
    init_pair(1, COLOR_GREEN,  COLOR_BLACK); // Зеленый для 1 змейки
    init_pair(2, COLOR_RED,    COLOR_BLACK); // Красный для пуль
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Желтый для 2 змейки

    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    double time_speed = 1000.0;

    #ifndef _WIN32
    set_escdelay(50); 
    time_speed = 10000.0;
    #endif

    struct AppContext app;
    app_init(&app);

    // Мьютекс нажатия клавиш
    mtx_init(&app.ch_key_mutex, mtx_plain);
    app.shared_ch_key = 0;

    clock_t last_snake_move_time = clock();

    while (app.is_running) 
    {
        // Проверка изменения размеров терминала
        #ifdef _WIN32
        if (is_termresized()) 
        {
            resize_term(0, 0); 
            if (app.screens.current_screen && app.screens.current_screen->clean) 
            {
                app.screens.current_screen->clean(&app);
            }
            erase(); 
            continue; 
        }
        #endif

        if (app.screens.current_screen && app.screens.current_screen->render) 
        {
            app.screens.current_screen->render(&app);   // ОТРИСОВКА ЭКРАНОВ
        }
        
        // Отрисовка оверлея поверх экрана
        draw_overlay_menu(&app); 
        
        doupdate();

        int ch = wgetch(stdscr);
        
        // КРИТИЧЕСКАЯ СЕКЦИЯ для передачи символа в поток 2 змейки
        mtx_lock(&app.ch_key_mutex);
        if (ch != ERR) 
        {
            app.shared_ch_key = ch; // Передача символа
        } 
        else 
        {
            // DEBUG
            // app.shared_ch_key = 0;  // 0 если клавиша не нажата
        }
        mtx_unlock(&app.ch_key_mutex);
        
        // Глобальный перехват кнопки ESC (код 27)
        if (ch == 27) 
        {
            if (app.screens.current_screen == (struct I_GameScreen*)&app.screens.gameplay_screen) 
            {
                app.overlay.is_visible = !app.overlay.is_visible;
                app.overlay.highlight = 0;
                erase(); 
                last_snake_move_time = clock(); 
            }
            
        }
        else if (app.overlay.is_visible) 
        {
            // Обработка ввода внутри всплывающего меню паузы
            switch (ch) 
            {
                case KEY_UP:
                    app.overlay.highlight = (app.overlay.highlight == 0) 
                        ? OVERLAY_CHOICES_COUNT - 1 : app.overlay.highlight - 1;
                    break;
                case KEY_DOWN:
                    app.overlay.highlight = (app.overlay.highlight == OVERLAY_CHOICES_COUNT - 1) 
                        ? 0 : app.overlay.highlight + 1;
                    break;
                case 10: // Enter
                    if (app.overlay.highlight == 0) 
                    {
                        app.overlay.is_visible = false; 
                        erase();
                        last_snake_move_time = clock(); 
                    } 
                    else if (app.overlay.highlight == 1) 
                    {
                        app.overlay.is_visible = false;
                        stop_game_session(&app.screens.gameplay_screen);
                        app_switch_screen(&app, (struct I_GameScreen*)&app.screens.menu_screen); 
                    } 
                    else if (app.overlay.highlight == 2) 
                    {
                        stop_game_session(&app.screens.gameplay_screen);
                        app.is_running = 0; 
                    }
                    break;
            }
        } 
        else 
        {
            if (ch == KEY_RESIZE) 
            {
                if (app.screens.current_screen && app.screens.current_screen->clean) 
                {
                    app.screens.current_screen->clean(&app);
                }
                erase();
                continue; 
            }
            if (ch != ERR && ch != 27 && app.screens.current_screen && app.screens.current_screen->handle_input) 
            {
                app.screens.current_screen->handle_input(&app, ch);
            }
        }

        // Игровой процесс
        if (app.screens.current_screen == (struct I_GameScreen*)&app.screens.gameplay_screen) 
        {
            if (app.overlay.is_visible) 
            {
                app.screens.gameplay_screen.is_pause = 1;
            }
            else 
            {
                app.screens.gameplay_screen.is_pause = 0;

                clock_t current_time = clock();
                
                // скорость пули
                static clock_t last_bullet_move_time = 0; 
                if (last_bullet_move_time == 0) last_bullet_move_time = clock();
                double elapsed_bullet_seconds = (double)(current_time - last_bullet_move_time) / CLOCKS_PER_SEC;
                double bullet_delay_seconds = 0.065; // скорость
                
                if (elapsed_bullet_seconds >= bullet_delay_seconds)
                {
                    update_bullets(&app.screens.gameplay_screen);
                    if (!app.screens.gameplay_screen.snake.is_alive)
                    {
                        stop_game_session(&app.screens.gameplay_screen);
                        app_switch_screen(&app, (struct I_GameScreen*)&app.screens.menu_screen); // В меню
                        continue;
                    }
                    last_bullet_move_time = current_time;
                }

                // скорость обновления основной змейки
                double elapsed_seconds = (double)(current_time - last_snake_move_time) / CLOCKS_PER_SEC;
                double required_delay_seconds = (double)app.screens.gameplay_screen.snake.delay_ms / time_speed;

                // скорость змейки
                if (elapsed_seconds >= required_delay_seconds) 
                { 
                    update_snake_step(&app.screens.gameplay_screen);    // шаг змейки
                    update_bullets(&app.screens.gameplay_screen);
                    if (check_collisions(&app.screens.gameplay_screen)) // столкновения
                    {
                        stop_game_session(&app.screens.gameplay_screen); // Остановка потоков при проигрыше
                        app_switch_screen(&app, (struct I_GameScreen*)&app.screens.menu_screen);
                        continue;
                    }

                    last_snake_move_time = current_time; 
                }
            }
        }

        struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 }; 
        thrd_sleep(&ts, NULL);  // главный поток на паузу
    }

    stop_game_session(&app.screens.gameplay_screen);
    app_destroy(&app);
    endwin();
    return 0;
}

