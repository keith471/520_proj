CC = gcc

all: y.tab.c lex.yy.c main.c error.c error.h memory.c memory.h tree.c tree.h pretty.c pretty.h outputhelpers.c outputhelpers.h weed.c weed.h symbol.c symbol.h
	$(CC) lex.yy.c y.tab.c main.c error.c memory.c tree.c pretty.c outputhelpers.c weed.c symbol.c -o out -ll

bison_debug: y.tab.c lex.yy.c main.c error.c error.h memory.c memory.h tree.c tree.h pretty.c pretty.h outputhelpers.c outputhelpers.h weed.c weed.h
	$(CC) lex.yy.c y.tab.c main.c error.c memory.c tree.c pretty.c outputhelpers.c weed.c -D BISON_DEBUG -o out -ll

flex_debug: y.tab.c lex.yy.c main.c error.c error.h memory.c memory.h tree.c tree.h pretty.c pretty.h outputhelpers.c outputhelpers.h weed.c weed.h
	$(CC) lex.yy.c y.tab.c main.c error.c memory.c tree.c pretty.c outputhelpers.c weed.c -D FLEX_DEBUG -o out -ll

y.tab.c y.tab.h: golite.y
	bison --verbose --yacc --defines golite.y

lex.yy.c: golite.l y.tab.h
	flex golite.l

flex: golite.l
	flex golite.l; $(CC) lex.yy.c -o scanner -ll

clean:
	rm -f y.tab.* lex.yy.* y.output scanner out; rm -rf ./output/*.go

## END
