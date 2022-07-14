CFLAGS = -Wall -Werror -std=c11 -fsanitize=address
CC = gcc
BIN = fifth

$(BIN): fifth.o
	$(CC) $(CFLAGS) -o $(BIN) $^

clean:
	rm -f *.o $(BIN)
