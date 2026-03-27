CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude
LIBS    = -lncurses -lsqlite3 -lm

SRC     = $(wildcard src/**/*.c src/*.c)
OBJ     = $(SRC:.c=.o)
TARGET  = ohmio

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean
