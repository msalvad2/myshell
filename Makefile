CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

myshell: src/builtins.o src/executor.o src/main.o src/parser.o
	$(CC) $(CFLAGS)  -o $@ $^

src/builtins.o: src/builtins.c 
	$(CC) $(CFLAGS) -c -o $@ $<

src/executor.o: src/executor.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/main.o: src/main.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/parser.o: src/parser.c
	$(CC) $(CFLAGS) -c -o $@ $<


.PHONY: clean valgrind


clean:
	rm -f src/*.o myshell

valgrind:
	valgrind --leak-check=full --track-origins=yes ./myshell

