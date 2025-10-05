# -----------------------------
# Makefile for ls project (v1.4.0)
# -----------------------------

CC = gcc
CFLAGS = -Wall -g

SRC = src/ls-v1.4.0.c
OBJ = obj/ls-v1.4.0.o
BIN = bin/ls

# Default rule
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

# Compile source to object
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Clean rule
clean:
	rm -f $(OBJ) $(BIN)

