CFLAGS = -Wall -Werror -std=c11  -fsanitize=address
CC = gcc
BIN = first

$(BIN): first.o
	$(CC) $(CFLAGS) -o $(BIN) $^

clean:
	rm -f *.o $(BIN)
