CC = gcc
CFLAGS = -Wall -g
SRC = src/ls-v1.6.0.c
OBJ = obj/ls-v1.6.0.o
BIN = bin/ls

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)

