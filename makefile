lexer: lex.yy.c
	gcc lex.yy.c -o lexer

lex.yy.c: lexer.l
	flex lexer.l

clean:
	rm lex.yy.c 
	rm lexer

test: 
	./lexer test.c test.txt