simp: lex.yy.c y.tab.c
	gcc y.tab.c lex.yy.c -o simp

lex.yy.c: simp_lex.l
	lex simp_lex.l

y.tab.c: simp_syntax.y
	yacc -t -d simp_syntax.y

clean:
	rm -f lex.yy.c 
	rm -f y.tab.c
	rm -f y.tab.h
	rm -f simp