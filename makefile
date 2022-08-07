CC=gcc

BINDIR=bin
BIN=$(BINDIR)/leopard

SRC=src
SRCS=$(wildcard $(SRC)/*.c)

INC=$(SRC)/include

OBJ=obj
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

ASM=asm

GEN=gen

CFLAGS = -O1 -Wall -I$(INC)

all:$(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm $(BINDIR)/* $(OBJ)/* $(GEN)/*

run-asm:
	./bin/leopard $(ASM)/foo.asm $(GEN)/foo.s

run-c:
	./bin/leopard $(ASM)/foo.asm $(GEN)/foo.c
	$(CC) -Wall -o foo $(GEN)/foo.c -S