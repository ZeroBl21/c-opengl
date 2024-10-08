# Makefile for C program

# Compiler and flags
CC = gcc
CFLAGS = -ggdb -Wall -Wextra -std=c11
CLINKS = -lglfw -lGLEW -lGL -lm

# Directories
BIN_DIR = ./bin
INC_DIR = -I./include/ -I./src

# Source file and output file
SOURCE = main.c $(wildcard ./src/*.c)
OUTPUT = $(BIN_DIR)/a

# Targets
all: build

build: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC_DIR) -o $@ $(SOURCE) $(CLINKS)

run: build
	$(OUTPUT)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all build run clean
