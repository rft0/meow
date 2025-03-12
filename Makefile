CC = clang
CFLAGS = -std=c99

TARGET_DIR = bin
ifeq ($(OS), Windows_NT)
    TARGET_NAME = meow.exe
else
    TARGET_NAME = meow
endif

TARGET = $(TARGET_DIR)/$(TARGET_NAME)

SRCS = $(wildcard src/*.c) $(wildcard src/utils/*.c) $(wildcard src/parser/*.c) $(wildcard src/meow/*.c)

all: dev

dev:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)