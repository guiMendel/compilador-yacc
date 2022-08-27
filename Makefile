CC      = gcc
CFLAGS  = -Wall -g

INCLUDE = include
OUTPUT = output
SRC = src
TEST = test

LEX     = flex
YACC    = bison -y -t
YFLAGS  = -d

SRCS 		= $(SRC)/ast.c $(SRC)/compiler.c $(SRC)/list.c $(SRC)/printer.c
OBJS 		= $(patsubst $(SRC)/%.c, $(OUTPUT)/%.o, $(SRCS))

TESTSRCS = $(wildcard $(TEST)/*.c)
TESTOBJS = $(patsubst $(TEST)/%.c, $(OUTPUT)/%.o, $(TESTSRCS))

all: $(OUTPUT)/y.tab.c $(OUTPUT)/lex.yy.c $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT)/compiler $(OUTPUT)/y.tab.c $(OUTPUT)/lex.yy.c $(OBJS) -I$(INCLUDE)

$(OUTPUT)/y.tab.c: $(SRC)/parser.y
	$(YACC) $(YFLAGS) -o $(OUTPUT)/y.tab.c $(SRC)/parser.y

$(OUTPUT)/lex.yy.c: $(SRC)/scanner.l
	$(LEX) -o $(OUTPUT)/lex.yy.c $(SRC)/scanner.l

$(OUTPUT)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE)

$(OUTPUT)/%.o: $(TEST)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE)

.PHONY: clean
clean:
	rm -rf $(OUTPUT)/*

# compile and run tests
test: $(OUTPUT)/y.tab.c $(OUTPUT)/lex.yy.c $(OBJS) $(TESTOBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT)/$@ $(TESTOBJS) $(OBJS)
	./$(OUTPUT)/$@ | greenest

