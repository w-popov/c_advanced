CC                  = gcc
CSTANDARD           = -std=c99
CFLAGS              = $(CSTANDARD) -Wall -Wextra -Wpedantic

SRC_DIR             = $(CURDIR)
BUILD_BASE_DIR      = $(CURDIR)/build
BUILD_DIR           = $(BUILD_BASE_DIR)
OBJ_DIR             = $(BUILD_DIR)/obj

SRCS                = $(wildcard $(SRC_DIR)/*.c)
TARGETS             = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%, $(SRCS))

.PHONY: all clean

all: $(TARGETS)

$(BUILD_DIR)/%: $(OBJ_DIR)/%.o
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BUILD_BASE_DIR) B0 B1
