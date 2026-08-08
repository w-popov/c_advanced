TARGET = snake
SRC_DIR = src
CC = gcc
CFLAGS = -std=c11 -Wall -Isrc/headers

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
    EXEC = $(TARGET).exe
    CFLAGS += -Iinclude
    LDFLAGS = -Llib -lpdcurses -lwinmm
    DEPENDENCY = lib/libpdcurses.a
else
    EXEC = $(TARGET)
    LDFLAGS = -lncurses -lpthread
    DEPENDENCY = linux_dep
endif

# 1. Главная цель
all: $(DEPENDENCY) $(EXEC)

# 2. Линковка программы
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# 3. Компиляция файлов .c в .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Для Windows
lib/libpdcurses.a:
	curl -L -o pdcurses.tar.gz https://github.com/wmcbrine/PDCurses
	tar -xf pdcurses.tar.gz
	mingw32-make -C PDCurses-3.9/wincon -f Makefile WIDE=Y UTF8=Y
	mkdir include lib
	cp PDCurses-3.9/curses.h include/curses.h
	cp PDCurses-3.9/panel.h include/panel.h
	cp PDCurses-3.9/wincon/pdcurses.a lib/libpdcurses.a
	rm -rf pdcurses.tar.gz PDCurses-3.9

# Установка ncurses для Linux
linux_dep:
	@if ! dpkg -s libncurses-dev >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y libncurses-dev; \
	fi

# Очистка
clean:
	rm -f src/*.o $(EXEC)
