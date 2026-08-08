# 1. Определение операционной системы
ifeq ($(OS),Windows_NT)
	IS_WINDOWS = 1
	PLATFORM := windows
else
	IS_WINDOWS = 0
	PLATFORM := linux
endif

TARGET = snake
SRC_DIR = src
HEAD_DIR = $(SRC_DIR)/headers
EXECUTE_DIR ?= .

CC = gcc
CSTANDARD = -std=c11

# Локальные пути для Windows
LOCAL_INC_DIR = ./include
LOCAL_LIB_DIR = ./lib
URL = https://github.com/wmcbrine/PDCurses
ARCHIVE = PDCurses-3.9.tar.gz
DIR = PDCurses-3.9

.PHONY: all sys_dependencies clean

# Базовые флаги компилятора и зависимости
COMMON_FLAGS = $(CSTANDARD) -Wall -Wextra -Wpedantic -I$(HEAD_DIR) -MMD -MP
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

EXEC_TARGET = $(EXECUTE_DIR)/$(TARGET)
ifeq ($(IS_WINDOWS),1)
    EXEC_TARGET := $(EXEC_TARGET).exe
endif

# Пути к библиотекам и линковка
ifeq ($(PLATFORM),windows)
    CFLAGS = $(COMMON_FLAGS) -I$(LOCAL_INC_DIR)
    LDFLAGS = -L$(LOCAL_LIB_DIR) -lpdcurses -lwinmm
else ifeq ($(PLATFORM),linux)
    CFLAGS = $(COMMON_FLAGS)
    LDFLAGS = -lncurses -lpthread
endif

# Главная цель
all: sys_dependencies $(EXEC_TARGET)

# Автоматическое управление зависимостями в зависимости от платформы
sys_dependencies:
ifeq ($(PLATFORM),windows)
	@if not exist "$(LOCAL_LIB_DIR)\libpdcurses.a" ( \
		echo [Windows] Local library not found. Installation begins.... && \
		$(MAKE) windows_install \
	)
else ifeq ($(PLATFORM),linux)
	@if ! dpkg -s libncurses-dev >/dev/null 2>&1; then \
		echo [Linux] The ncurses system library was not found. Installation begins.... && \
		$(MAKE) linux_install; \
	fi
else
	@echo Error: Unsupported OS.
	@exit 1
endif

# Сборка исполняемого файла (Строка 69)
$(EXEC_TARGET): $(OBJS)
ifeq ($(IS_WINDOWS),1)
	@if not exist "$(EXECUTE_DIR)" mkdir "$(EXECUTE_DIR)"
else
	@mkdir -p $(EXECUTE_DIR)
endif
	$(CC) $(OBJS) -o $@ $(LDFLAGS)


# Компиляция объектных файлов (.c -> .o)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Включение файлов зависимостей генерации компилятора (-MMD -MP)
-include $(DEPS)

# Установка для WINDOWS
windows_install:
	@if not exist $(ARCHIVE) ( \
		echo Download PDCurses v3.9... && \
		curl -L -o $(ARCHIVE) $(URL) \
	)
	@if not exist $(DIR) ( \
		echo Unpacking the archive... && \
		tar -xf $(ARCHIVE) \
	)
	@echo Building the PDCurses library...
	$(MAKE) -C $(DIR)/wincon -f Makefile WIDE=Y UTF8=Y
	@echo Copying files into a project...
	@if not exist "$(LOCAL_INC_DIR)" mkdir "$(LOCAL_INC_DIR)"
	@if not exist "$(LOCAL_LIB_DIR)" mkdir "$(LOCAL_LIB_DIR)"
	copy /Y "$(DIR)\curses.h" "$(LOCAL_INC_DIR)\curses.h"
	copy /Y "$(DIR)\panel.h" "$(LOCAL_INC_DIR)\panel.h"
	copy /Y "$(DIR)\wincon\pdcurses.a" "$(LOCAL_LIB_DIR)\libpdcurses.a"
	@echo [Success] PDCurses deployed locally to the project.

# Установка для LINUX
linux_install:
	@echo Обновление пакетов и установка ncurses ...
	sudo apt-get update
	sudo apt-get install -y libncurses-dev
	@echo [Успех] ncurses успешно установлена.

# Полная очистка проекта
clean:
ifeq ($(IS_WINDOWS),1)
	@if exist src\*.o del /F /Q src\*.o
	@if exist src\*.d del /F /Q src\*.d
	@if exist $(DIR) rmdir /S /Q $(DIR)
	@if exist $(ARCHIVE) del /F /Q $(ARCHIVE)
	@if exist $(TARGET).exe del /F /Q $(TARGET).exe
else
	rm -f $(SRC_DIR)/*.o $(SRC_DIR)/*.d $(EXEC_TARGET)
	rm -rf $(DIR) $(ARCHIVE)
endif


