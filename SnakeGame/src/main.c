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

#include "snake.h"
#include <locale.h>

int main(void) 
{
    setlocale(LC_ALL, "");
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    #ifndef _WIN32
    set_escdelay(50); 
    #endif

    struct AppContext app;
    app_init(&app);

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
            app.screens.current_screen->render(&app);
        }
        draw_overlay_menu(&app);
        int ch = (app.overlay.is_visible && app.overlay.win) 
                 ? wgetch(app.overlay.win) 
                 : wgetch(stdscr);

        // Глобальный перехват кнопки ESC (код 27)
        if (ch == 27) 
        {
            if (app.screens.current_screen == (struct I_GameScreen*)&app.screens.gameplay_screen) 
            {
                app.overlay.is_visible = !app.overlay.is_visible;
                app.overlay.highlight = 0;
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
                        // Продолжить
                        app.overlay.is_visible = false; 
                    } 
                    else if (app.overlay.highlight == 1) 
                    {
                        app.overlay.is_visible = false;
                        // В меню
                        app_switch_screen(&app, (struct I_GameScreen*)&app.screens.menu_screen); 
                    } 
                    else if (app.overlay.highlight == 2) 
                    {
                        // Выход
                        app.is_running = 0; 
                    }
                    break;
            }
        } 
        else 
        {
            // изменение размеров терминала
            if (ch == KEY_RESIZE) 
            {
                if (app.screens.current_screen && app.screens.current_screen->clean) 
                {
                    app.screens.current_screen->clean(&app);
                }
                continue; 
            }
            // проброс ввода в активный экран
            if (app.screens.current_screen && app.screens.current_screen->handle_input) 
            {
                app.screens.current_screen->handle_input(&app, ch);
            }
        }
    }

    app_destroy(&app);
    endwin();
    return 0;
}

// git pull origin main