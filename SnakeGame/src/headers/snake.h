#ifndef _SNAKE_GAME_H_
#define _SNAKE_GAME_H_

#ifdef _WIN32
    #define PDC_WIDE
    #define PDC_FORCE_UTF8
    #include <curses.h>
    #if defined(__STDC_NO_THREADS__) || defined(__MINGW32__)
        #include "tinycthread.h" 
    #else
        #include <threads.h>
    #endif
#else
    #define _XOPEN_SOURCE_EXTENDED 1
    #include <ncurses.h>
    #include <threads.h>
#endif

#include <wchar.h>
#include <stdbool.h>

// Константы для размеров меню
#define MENU_WIDTH  30
#define MENU_HEIGHT 8
#define OVERLAY_MENU_HEIGHT 7

#define MENU_CHOICES_COUNT 2
#define OVERLAY_CHOICES_COUNT 3

extern const wchar_t* menu_choices[MENU_CHOICES_COUNT];
extern const wchar_t* overlay_choices[OVERLAY_CHOICES_COUNT];

struct AppContext;

/* Сигнатуры паттерна Стратегия */
typedef void (*I_render)        (struct AppContext *cntx);
typedef void (*I_clean)         (struct AppContext *cntx);
typedef void (*I_handle_input)  (struct AppContext *cntx, int ch);

struct I_GameScreen
{
    I_render render;
    I_clean  clean;
    I_handle_input handle_input;
};

/* Экраны */
struct ScreenMenu
{
    struct I_GameScreen game_screen;
    WINDOW *win;
    int highlight;
};

struct ScreenGamePlay
{
    struct I_GameScreen game_screen;
    WINDOW *subwin_status;
    WINDOW *subwin_game; 
};

/* Всплывающее ESC-оверлей меню */
struct OverlayMenu
{
    WINDOW *win;
    bool is_visible;
    int highlight;
};

/* Структура для хранения экранов */
struct GameScreens
{
    struct I_GameScreen *current_screen;
    struct ScreenMenu menu_screen;
    struct ScreenGamePlay gameplay_screen;
};

/* Структура для представления объекта на экране */
struct Pixel
{
    int x;          
    int y;          
    wchar_t image;  
};

/* Главный контекст приложения */
struct AppContext
{
    struct GameScreens screens;     
    struct OverlayMenu overlay;
    int is_running;                 
};

// Инициализация приложения
void app_init(struct AppContext *app);

// Переключение между экранами
void app_switch_screen(struct AppContext *app, struct I_GameScreen *new_screen);

// Завершение работы приложения
void app_destroy(struct AppContext *app);

// Отрисовка оверлейного меню
void draw_overlay_menu(struct AppContext *app);

#endif
