SRC := src
OBJ := obj
BIN := bin
EXECUTABLE:= shell

SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
INCS := -Iinclude/
DIRS := $(OBJ)/ $(BIN)/
EXEC := $(BIN)/$(EXECUTABLE)

CC := gcc
CFLAGS := -g -Wall -std=c99 $(INCS)
LDFLAGS :=

all: $(EXEC)

bin/mytimeout: src/mytimeout/mytimeout.c 
	gcc -g -Wall -std=c99 -o bin/mytimeout src/mytimeout/mytimeout.c

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	$(EXEC)

clean:
	rm $(OBJ)/*.o $(EXEC)

$(shell mkdir -p $(DIRS))

.PHONY: run clean all
