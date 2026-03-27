CC     = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LIBS   = -lncurses -lsqlite3 -lm

DAEMON_CFLAGS = -Wall -Wextra -O2 -Iinclude \
                $(shell pkg-config --cflags libnotify glib-2.0)
DAEMON_LIBS   = -lsqlite3 -lm \
                $(shell pkg-config --libs libnotify glib-2.0)

# Todos los .c excepto los del daemon y main_daemon
TUI_SRC = $(shell find src -name "*.c" \
            ! -path "src/daemon/*")

DAEMON_SRC = $(shell find src/daemon -name "*.c") \
             $(shell find src/core -name "*.c") \
             $(shell find src/data -name "*.c")

OBJ           = $(TUI_SRC:.c=.o)
TARGET        = ohmio
TARGET_DAEMON = ohmio-daemon

all: $(TARGET) $(TARGET_DAEMON)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

$(TARGET_DAEMON): $(DAEMON_SRC)
	$(CC) $(DAEMON_CFLAGS) $(DAEMON_SRC) -o $@ $(DAEMON_LIBS)

$(OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET_DAEMON)

.PHONY: all clean
