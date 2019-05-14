# Executavel
BINFOLDER := bin/
# .hpp
INCFOLDER := inc/
# .cpp
SRCFOLDER := src/
# .o
OBJFOLDER := obj/
CC := gcc
CFLAGS := -lrt -pthread
SRCFILES := $(wildcard src/*.c)

all: $(SRCFILES:src/%.c=obj/%.o)
	$(CC) obj/*.o -o bin/prog $(CFLAGS)
obj/%.o: src/%.c
	$(CC) -c $< -o $@ -I./inc $(CFLAGS)

run: bin/prog
	bin/prog

.PHONY: clean
clean:
	rm -rf obj/*
	rm -rf bin/*

run-all: clean all run
