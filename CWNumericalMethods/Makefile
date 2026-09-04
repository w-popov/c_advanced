# # Курсовая работа. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
# # Сборка консольной программы по численным методам
# # debug режим команда:   make
# # release режим команда: make DEBUG=0

ifeq ($(OS),Windows_NT)
    IS_WINDOWS 		= 1
    EXE 			= .exe
else
    IS_WINDOWS 		= 0
    EXE 			=
endif

# Имя исполняемого файла
TARGET_NAME        = numerical
TARGET             = $(TARGET_NAME)$(EXE)

# Исходные каталоги проекта
SRC_DIR            = src
HEAD_DIR           = $(SRC_DIR)/headers
BUILD_BASE_DIR     = build

CC                 = gcc
CSTANDARD          = -std=c11

# Режим сборки по умолчанию (1 - Debug, 0 - Release)
DEBUG ?= 1

# Базовые флаги компилятора
COMMON_FLAGS 		= $(CSTANDARD) -Wall -Wextra -Wpedantic -pthread -I$(HEAD_DIR) -MMD -MP

ifeq ($(DEBUG), 1)
    CFLAGS  		= $(COMMON_FLAGS) -g -O0 -DDEBUG
    LDFLAGS 		= 
    LIBS    		= -lm
    
    BUILD_DIR   	= $(BUILD_BASE_DIR)/debug
    OBJ_DIR     	= $(BUILD_DIR)/obj
    EXECUTE_DIR 	= $(BUILD_DIR)/bin
else
    CFLAGS  		= $(COMMON_FLAGS) -O3 -flto -DNDEBUG
    LDFLAGS 		= -flto -s
    LIBS    		= -lm
    BUILD_DIR   	= $(BUILD_BASE_DIR)/release
    OBJ_DIR     	= $(BUILD_DIR)/obj
    EXECUTE_DIR 	= $(BUILD_DIR)/bin
endif

ifeq ($(IS_WINDOWS), 1)
    LDFLAGS 		+= -static -static-libgcc
else
    LIBS    		+= -pthread
endif

# Поиск исходников проекта (.c)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Автоматическая генерация зависимостей (.d)
DEPS = $(OBJS:.o=.d)

# Финальный путь к исполняемому файлу
EXEC_TARGET = $(EXECUTE_DIR)/$(TARGET)

# Флаги подавления варнингов для библиотеки pbPlots
PBPLOTS_NO_WARN = -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-overlength-strings -Wno-unused-variable

# ---------------------------------------------------------
all: $(EXEC_TARGET)

# Правило для компиляции pbPlots.c
$(OBJ_DIR)/pbPlots.o: $(SRC_DIR)/pbPlots.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PBPLOTS_NO_WARN) -c $< -o $@

# Общее правило компиляции
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Линковка
$(EXEC_TARGET): $(OBJS) | $(EXECUTE_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(EXEC_TARGET) $(LIBS)
	cp $(EXEC_TARGET) ./$(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(EXECUTE_DIR):
	mkdir -p $(EXECUTE_DIR)

clean:
	rm -rf $(BUILD_BASE_DIR)
	rm -f ./$(TARGET) *.png

-include $(DEPS)

.PHONY: all clean run
