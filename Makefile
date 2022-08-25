CC      = gcc
CFLAGS  = -Wall -g

LEX     = flex
YACC    = bison -y
YFLAGS  = -d
OBJS 		= scan.o parse.o compiler.o list.o ast.o

TESTSRCS = $(wildcard test/*.c)
TESTOBJS = $(TESTSRCS:.c=.o)

OUTPUT = output

simp: $(OBJS)
scan.o: scan.l parse.c
parse.o: parse.y
compiler.o: compiler.c

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(TESTOBJS)
	rm -f y.tab.h
	rm -f parse.c

# compile and run tests
test: simp $(TESTOBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT)/$@ $(TESTOBJS) $(OBJS) -Itest/greatest.h
	./$(OUTPUT)/$@

