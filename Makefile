CC      = gcc
CFLAGS  = -Wall -g
CXX 		= g++
CXXFLAGS = -Wall -g

LEX     = flex
YACC    = bison -y
YFLAGS  = -d
OBJS 		= scan.o parse.o compiler.o ast.o

simp: $(OBJS)
scan.o: scan.l parse.c
parse.o: parse.y
compiler.o: compiler.cpp

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f y.tab.h
	rm -f parse.c
