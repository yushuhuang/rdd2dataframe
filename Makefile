#!/usr/bin/make

all: transform

CFLAGS = $(shell pkg-config --cflags glib-2.0)
LFLAGS = $(shell pkg-config --libs glib-2.0)

transform: transform.l transform.y transform.h transform_func.c
	@bison -d transform.y
	@flex transform.l transform.tab.h
	@cc $(CFLAGS) transform.tab.c lex.yy.c transform_func.c -lm -o $@ $(LFLAGS)

clean:
	@rm -f transform.tab.c transform.tab.h lex.yy.c transform.c transform