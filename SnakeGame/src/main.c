#ifdef _WIN32
    #include <curses.h>
    #if defined(__STDC_NO_THREADS__) || defined(__MINGW32__)
        #include "tinycthread.h" 
    #else
        #include <threads.h>
    #endif
#else
    #include <ncurses.h>
    #include <threads.h>
#endif

#include "screens.h"
#include "snake.h"
#include <locale.h>
#include <time.h> 

int main(void) 
{
    setlocale(LC_ALL, "");
    initscr();
    clear();
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
            clear(); 
            continue; 
        }
        #endif

        if (app.screens.current_screen && app.screens.current_screen->render) 
        {
            app.screens.current_screen->render(&app);   // ОТРИСОВКА ЭКРАНОВ
        }
        
        // Отрисовка оверлея поверх экрана
        draw_overlay_menu(&app);     

        int ch = wgetch(stdscr);

        // Глобальный перехват кнопки ESC (код 27)
        if (ch == 27) 
        {
            if (app.screens.current_screen == (struct I_GameScreen*)&app.screens.gameplay_screen) 
            {
                app.overlay.is_visible = !app.overlay.is_visible;
                app.overlay.highlight = 0;
                clear(); 
                last_snake_move_time = clock(); 
            }
            continue;
        }

        if (app.overlay.is_visible) 
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
                        clear();
                        last_snake_move_time = clock(); 
                    } 
                    else if (app.overlay.highlight == 1) 
                    {
                        app.overlay.is_visible = false;
                        app_switch_screen(&app, (struct I_GameScreen*)&app.screens.menu_screen); 
                    } 
                    else if (app.overlay.highlight == 2) 
                    {
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
                clear();
                continue; 
            }
            if (ch != ERR && app.screens.current_screen && app.screens.current_screen->handle_input) 
            {
                app.screens.current_screen->handle_input(&app, ch);
            }
        }
        // игровой процесс
        if (app.screens.current_screen == (struct I_GameScreen*)&app.screens.gameplay_screen && !app.overlay.is_visible) 
        {
            
            clock_t current_time = clock();
            double elapsed_seconds = (double)(current_time - last_snake_move_time) / CLOCKS_PER_SEC;
            double required_delay_seconds = (double)app.screens.gameplay_screen.delay_ms / time_speed;

            if (elapsed_seconds >= required_delay_seconds) 
            { 
                update_snake_step(&app.screens.gameplay_screen);    // шаг змейки
                
                if (check_collisions(&app.screens.gameplay_screen)) // столкновения
                {
                    app_switch_screen(&app, (struct I_GameScreen*)&app.screens.menu_screen);
                    continue;
                }
                
                last_snake_move_time = current_time; 
            }
        }

        struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 }; 
        thrd_sleep(&ts, NULL);  // поток в сон
    }

    app_destroy(&app);
    endwin();
    return 0;
}

// git pull origin main