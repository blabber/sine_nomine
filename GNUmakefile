CC ?= clang
LDFLAGS = -lncurses -lm
CFLAGS ?= -std=c11 -Wall -I./include

TARGET = sn
SRC=$(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
