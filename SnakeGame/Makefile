## C_advanced Змейка
##

TARGET = snake
SRC_DIR = src
OBJ_DIR = obj
CC = gcc
CFLAGS = -std=c11 -Wall -pthread -Isrc/headers -MMD -MP

SRCS = $(wildcard $(SRC_DIR)/*.c)
# Перенаправление .o и .d файлов в obj
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

ifeq ($(OS),Windows_NT)
    EXEC = $(TARGET).exe
    CFLAGS += -Iinclude
    LDFLAGS = -static -Llib -lpdcurses -lwinmm -lm
    DEPENDENCY = lib/libpdcurses.a
else
    EXEC = $(TARGET)
    LDFLAGS = -pthread -lncursesw -lm
    DEPENDENCY = linux_dep
endif

# Главная цель
all: $(DEPENDENCY) $(EXEC)

# Линковка
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Компиляция
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Зависимости
-include $(DEPS)

# Для Windows
lib/libpdcurses.a:
	@echo "Downloading PDCurses..."
	curl -L -o pdcurses.tar.gz https://github.com/wmcbrine/PDCurses/archive/refs/tags/3.9.tar.gz || \
	wget -O pdcurses.tar.gz https://github.com/wmcbrine/PDCurses/archive/refs/tags/3.9.tar.gz
	tar -xf pdcurses.tar.gz
	cd PDCurses-3.9/wincon && $(MAKE) -f Makefile WIDE=Y UTF8=Y
	mkdir -p include lib
	cp PDCurses-3.9/curses.h include/curses.h
	cp PDCurses-3.9/panel.h include/panel.h
	cp PDCurses-3.9/wincon/pdcurses.a lib/libpdcurses.a
	rm -rf pdcurses.tar.gz PDCurses-3.9

# Linux
linux_dep:
	@if ! dpkg -s libncurses-dev >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y libncurses-dev; \
	fi

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

.PHONY: all clean linux_dep
