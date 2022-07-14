CFLAGS = -Wall -Werror -std=c11   -fsanitize=address
CC = gcc
BIN = second

$(BIN): second.o
	$(CC) $(CFLAGS) -o $(BIN) $^

clean:
	rm -f *.o $(BIN)
