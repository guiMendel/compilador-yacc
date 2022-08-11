CC      = gcc
CFLAGS  = -Wall

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
	$(RM) -r $(BUILD_DIR)
