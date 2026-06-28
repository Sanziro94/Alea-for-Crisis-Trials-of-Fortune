CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lraylib

SRC=$(wildcard *.c)

NAME=alea-for-crisis

all: build

build: $(NAME)
$(NAME): $(SRC)
	gcc $(CFLAGS) -o $(NAME) $(SRC) $(LDFLAGS)

clean:
	rm -f $(NAME)

.PHONY: all build clean
