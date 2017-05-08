# Makefile

TARGET := plasma

CC=clang
CC_FLAGS = -Wall -O3


default: plasma

plasma: plasma.c
	$(CC) $(CC_FLAGS) plasma.c -framework SDL2 -o $(TARGET)

.PHONY: clean
clean:
	rm $(TARGET)
