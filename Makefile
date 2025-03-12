CC = clang
CFLAGS = -std=c99

TARGET_DIR = bin
TARGET_NAME = meow
TARGET = $(TARGET_DIR)/$(TARGET_NAME)

SRCS = $(wildcard src/*.c) $(wildcard src/utils/*.c) $(wildcard src/parser/*.c) $(wildcard src/meow/*.c)

all: dev

dev:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)