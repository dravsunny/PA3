CFLAGS = -Wall -Werror -std=c11 -fsanitize=address
CC = gcc
BIN = sixth

$(BIN): sixth.o
	$(CC) $(CFLAGS) -o $(BIN) $^

clean:
	rm -f *.o $(BIN)
