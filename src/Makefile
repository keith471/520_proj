CC = gcc

all: y.tab.c lex.yy.c main.c error.c error.h memory.c memory.h tree.c tree.h
	$(CC) lex.yy.c y.tab.c main.c error.c memory.c tree.c -o out -ll

bison_debug: y.tab.c lex.yy.c main.c error.c error.h memory.c memory.h tree.c tree.h
	$(CC) lex.yy.c y.tab.c main.c error.c memory.c tree.c -D BISON_DEBUG -o out -ll

flex_debug: y.tab.c lex.yy.c main.c error.c error.h memory.c memory.h tree.c tree.h conversion.c conversion.h
	$(CC) lex.yy.c y.tab.c main.c error.c -D FLEX_DEBUG -o flex -ll

y.tab.c y.tab.h: golite.y
	bison --verbose --yacc --defines golite.y

lex.yy.c: golite.l y.tab.h
	flex golite.l

flex: golite.l
	flex golite.l; $(CC) lex.yy.c -o scanner -ll

clean:
	rm y.tab.* lex.yy.*; rm scanner; rm out; rm flex; rm y.output

## END
