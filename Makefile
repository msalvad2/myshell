CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

myshell: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lreadline

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean valgrind


clean:
	rm -f src/*.o myshell

valgrind:
	valgrind --leak-check=full --track-origins=yes ./myshell

