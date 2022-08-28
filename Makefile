CC      = gcc
CFLAGS  = -Wall -g -fno-stack-protector

INCLUDES = include lua4/include
INCLUDE = $(addprefix -I,$(INCLUDES))
OUTPUT = output
SRC = src
LIBS = -l:liblua4.a -l:liblualib4.a -lm
LIBDIR = lua4/lib/Linux24g3_64

LEX     = flex
YACC    = bison -y -t
YFLAGS  = -d

SRCS 		= $(SRC)/ast.c $(SRC)/compiler.c $(SRC)/list.c $(SRC)/simp.c
OBJS 		= $(patsubst $(SRC)/%.c, $(OUTPUT)/%.o, $(SRCS))

all: $(OUTPUT)/y.tab.c $(OUTPUT)/lex.yy.c $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT)/simp $(OUTPUT)/y.tab.c $(OUTPUT)/lex.yy.c $(OBJS) $(INCLUDE) -L$(LIBDIR) $(LIBS)

$(OUTPUT)/y.tab.c: $(SRC)/parser.y
	$(YACC) $(YFLAGS) -o $(OUTPUT)/y.tab.c $(SRC)/parser.y

$(OUTPUT)/lex.yy.c: $(SRC)/scanner.l
	$(LEX) -o $(OUTPUT)/lex.yy.c $(SRC)/scanner.l

$(OUTPUT)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE) -L$(LIBDIR) $(LIBS) 

.PHONY: clean
clean:
	rm -rf $(OUTPUT)/*

