CC = gcc

all: y.tab.c lex.yy.c main.c helpers.c helpers.h error.c error.h memory.c memory.h tree.c tree.h pretty.c pretty.h outputhelpers.c outputhelpers.h weed.c weed.h symbol.c symbol.h type.c type.h cppType.c cppType.h codegen.c codegen.h
	$(CC) lex.yy.c y.tab.c main.c helpers.c error.c memory.c tree.c pretty.c outputhelpers.c weed.c symbol.c type.c cppType.c codegen.c -o out -ll

bison_debug: y.tab.c lex.yy.c main.c helpers.c helpers.h error.c error.h memory.c memory.h tree.c tree.h pretty.c pretty.h outputhelpers.c outputhelpers.h weed.c weed.h symbol.c symbol.h type.c type.h cppType.c cppType.h codegen.c codegen.h
	$(CC) lex.yy.c y.tab.c main.c helpers.c error.c memory.c tree.c pretty.c outputhelpers.c weed.c symbol.c type.c cppType.c codegen.c -D BISON_DEBUG -o out -ll

flex_debug: y.tab.c lex.yy.c main.c helpers.c helpers.h error.c error.h memory.c memory.h tree.c tree.h pretty.c pretty.h outputhelpers.c outputhelpers.h weed.c weed.h symbol.c symbol.h type.c type.h cppType.c cppType.h codegen.c codegen.h
	$(CC) lex.yy.c y.tab.c main.c helpers.c error.c memory.c tree.c pretty.c outputhelpers.c weed.c symbol.c type.c cppType.c codegen.c -D FLEX_DEBUG -o out -ll

y.tab.c y.tab.h: golite.y
	bison --verbose --yacc --defines golite.y

lex.yy.c: golite.l y.tab.h
	flex golite.l

flex: golite.l
	flex golite.l; $(CC) lex.yy.c -o scanner -ll

clean:
	rm -f y.tab.* lex.yy.* y.output scanner out; rm -rf ./output/*.go

## END
