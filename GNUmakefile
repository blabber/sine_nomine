CC = clang
LDFLAGS=-lncurses -lm
CFLAGS = -std=c11 -Wall

TARGET = sn
SRC=$(wildcard *.c)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
