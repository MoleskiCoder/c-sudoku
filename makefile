SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o

CFLAGS += -std=c99 -O3 -march=native -S

all : sudoku

.PHONY : clean
clean : 
	rm -fv *.exe *.o
