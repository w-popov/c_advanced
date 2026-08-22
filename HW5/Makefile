##	HW5 C_advanced
##

CC                  = gcc
CSTANDARD           = -std=c11
CFLAGS              = $(CSTANDARD) -Wall -Wextra -Wpedantic
SRC_DIR				= $(CURDIR)/src
SRCS                = $(wildcard $(SRC_DIR)/*.c)
TARGETS             = $(patsubst $(SRC_DIR)/%.c, %, $(SRCS))

all: $(TARGETS)

%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean
