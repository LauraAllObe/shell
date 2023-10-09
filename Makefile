SRC := src
OBJ := obj
BIN := bin
EXECUTABLE:= shell

SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
INCS := -Iinclude/
DIRS := $(OBJ)/ $(BIN)/
EXEC := $(BIN)/$(EXECUTABLE)
MYTIMEOUT_OBJ := $(BIN)/mytimeout

CC := gcc
CFLAGS := -g -Wall -std=c99 $(INCS)
LDFLAGS :=

all: $(EXEC)

$(BIN)/mytimeout: $(SRC)/mytimeout/mytimeout.c
	$(CC) $(CFLAGS) -c $< -o $@
	chmod +x $@

$(EXEC): $(OBJS) $(MYTIMEOUT_OBJ)
	$(CC) $(CFLAGS) $(OBJS) $(MYTIMEOUT_OBJ) -o $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	$(EXEC)

clean:
	rm $(OBJ)/*.o $(EXEC) $(MYTIMEOUT)
	rm $(OBJ)/*.o $(MYTIMEOUT)

$(shell mkdir -p $(DIRS))

.PHONY: run clean all
