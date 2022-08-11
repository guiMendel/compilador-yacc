CC      = gcc
CFLAGS  = -Wall -g

LEX     = flex
YACC    = bison -y
YFLAGS  = -d
OBJS 		= scan.o parse.o simp.o list.o

simp: $(OBJS)
scan.o: scan.l parse.c
parse.o: parse.y
simp.o: simp.c

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f y.tab.h
	rm -f parse.c
