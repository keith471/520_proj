%{
// General
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "tree.h"

// flex may complain if this is not here
extern int yylex();

// defined in main, but built up here!
extern PROGRAM* theprogram;

%}

%union {
	int intLiteral;
	float floatLiteral;
    char runeLiteral;
	char *stringLiteral;
    struct PACKAGE* package;
    struct TOPLEVELDECLARATION* topLevelDecl;
    struct VARDECLARATION* varDecl;
    struct TYPEDECLARATION* typeDecl;
    struct FUNCTIONDECLARATION* functionDecl;
    struct PARAMETER* parameter;
    struct STATEMENT* statement;
    struct SWITCHCASE* switchCase;
    struct EXP* exp;
    //struct CAST* cast;
    struct TYPE* type;
    struct STRUCTTYPE* structType;
    struct ID* id;
    struct FIELD* field;
    int op;
}

// define the constant-string tokens
%token tBREAK tCASE tCHAN tCONST tCONTINUE tDEFAULT tDEFER tELSE tFALLTHROUGH tFOR
       tFUNC tGO tGOTO tIF tIMPORT tINTERFACE tMAP tPACKAGE tRANGE tRETURN tSELECT
       tSTRUCT tSWITCH tTYPE tVAR tEQ tLEQ tGEQ tNEQ tSHORT tAND tOR tLEFTSHIFT
       tRIGHTSHIFT tPLUSEQ tMINUSEQ tTIMESEQ tDIVEQ tMODEQ tANDEQ tOREQ tXOREQ
       tLEFTSHIFTEQ tRIGHTSHIFTEQ tINC tDEC tLARROW tELLIPSIS tPRINT tPRINTLN tAPPEND
       tBITCLEAR tBITCLEAREQ

%token <intLiteral> tINTDECLITERAL tINTOCTLITERAL tINTHEXLITERAL
%token <floatLiteral> tFLOAT64LITERAL
%token <runeLiteral> tRUNELITERAL
%token <stringLiteral> tSTRINGLITERAL tRAWSTRINGLITERAL tIDENTIFIER

%type <package> packageDecl
%type <topLevelDecl> topLevelDecls topLevelDecl decl
%type <varDecl> varDecl varDeclRest regularVarDecl typeOnlyVarDecl expOnlyVarDecl
                typeAndExpVarDecl distributedVarDecl varDeclList neVarDeclList
%type <typeDecl> typeDecl typeDeclRest regularTypeDecl distributedTypeDecl typeDeclList neTypeDeclList
%type <functionDecl> functionDecl
%type <parameter> parameterList neParameterList parameter
%type <statement> statements statement simpleStatement printStatement printlnStatement
                  returnStatement ifStatement switchStatement forStatement breakStatement
                  continueStatement postStatement shortDeclStatement expStatement
                  incDecStatement assignmentStatement infiniteLoop whileLoop threePartLoop
                  block
%type <switchCase> switchClauses switchClause
%type <exp> exp returnValue unaryExp binaryExp expList neExpList primaryExp
            basicLiteral intLiteral operand optionalExp
//%type <cast> conversion
%type <type> type typeLit arrayType structType sliceType returnType
%type <id> identifier idList
%type <structType> fields
%type <field> field neFields
%type <op> binOp

%start program

/*
Precedence    Operator
    5             *  /  %  <<  >>  &  &^
    4             +  -  |  ^
    3             ==  !=  <  <=  >  >=
    2             &&
    1             ||
*/
// We handle precedence as follows
%left tOR
%left tAND
%left tEQ tNEQ '<' '>' tLEQ tGEQ
%left '+' '-' '|' '^'
%left '*' '/' '%' tLEFTSHIFT tRIGHTSHIFT '&' tBITCLEAR
%left UNARY

%%

program:
      packageDecl topLevelDecls
        {
            #ifdef BISON_DEBUG
                printf("done with a golite file!\n");
            #endif
            theprogram = makePROGRAM($1, $2);
        }
    ;

packageDecl:
      tPACKAGE identifier ';'
        {
            #ifdef BISON_DEBUG
                printf("found the package declaration\n");
            #endif
            $$ = makePACKAGE($2);
        }
    ;

topLevelDecls:
      /*epsilon*/
        { $$ = NULL; }
    | topLevelDecls topLevelDecl
        { $$ = appendTOPLEVELDECLARATION($1, $2); }
    ;

topLevelDecl:
      decl
        { $$ = $1; }
    | functionDecl
        { $$ = makeTOPLEVELDECLARATIONfunction($1); }
    ;

decl:
      typeDecl
        { $$ = makeTOPLEVELDECLARATIONtype($1); }
    | varDecl
        { $$ = makeTOPLEVELDECLARATIONvar($1); }
    ;

/* function declarations */

functionDecl:
      tFUNC identifier '(' parameterList ')' returnType block ';'
        {
            #ifdef BISON_DEBUG
                printf("matched a function declaration\n");
            #endif
            $$ = makeFUNCTIONDECLARATION($2, $4, $6, $7);
        }
    ;

parameterList:
      //epsilon
        {
            #ifdef BISON_DEBUG
                printf("found empty parameters\n");
            #endif
            $$ = NULL;
        }
    | neParameterList
        { $$ = $1; }
    ;

neParameterList:
      parameter
        { $$ = $1; }
    | neParameterList ',' parameter
        { $$ = appendPARAMETER($1, $3); }
    ;

parameter:
      // this is good - no weeding necessary
      idList type
        {
            #ifdef BISON_DEBUG
                printf("found a parameter\n");
            #endif
            $$ = makePARAMETER($1, $2);
        }
    ;

returnType:
      //epsilon
        {
            #ifdef BISON_DEBUG
                printf("found a void return type\n");
            #endif
            $$ = NULL;
        }
    | type
        {
            #ifdef BISON_DEBUG
                printf("found a non-void return type\n");
            #endif
            $$ = $1;
        }
    ;

/* Type declarations */

typeDecl:
      tTYPE typeDeclRest
        { $$ = $2; }
    ;

typeDeclRest:
      regularTypeDecl
        { $$ = $1; }
    | distributedTypeDecl
        { $$ = $1; }
    ;

regularTypeDecl:
      identifier type ';'
        {
            #ifdef BISON_DEBUG
                printf("found a type declaration\n");
            #endif
            $$ = makeTYPEDECLARATION($1, $2);
        }
    ;

distributedTypeDecl:
      '(' typeDeclList ')' ';'
        { $$ = $2; }
    ;

typeDeclList:
      // epsilon
        { $$ = makeTYPEDECLARATIONempty(); }
    | neTypeDeclList
        { $$ = $1; }
    ;

neTypeDeclList:
      regularTypeDecl
        { $$ = markAsDistributedTypeDecl($1); }
    | neTypeDeclList regularTypeDecl
        { $$ = appendTYPEDECLARATION($1, $2); }
    ;

/* variable declarations */

varDecl:
      tVAR varDeclRest
        { $$ = $2; }
    ;

varDeclRest:
      regularVarDecl
        { $$ = $1; }
    | distributedVarDecl
        { $$ = $1; }
    ;

regularVarDecl:
      typeOnlyVarDecl
        { $$ = $1; }
    | expOnlyVarDecl
        { $$ = $1; }
    | typeAndExpVarDecl
        { $$ = $1; }
    ;

typeOnlyVarDecl:
      // this is good - no weeding of idList necessary
      idList type ';'
        {
            #ifdef BISON_DEBUG
                printf("found a type-only variable declaration\n");
            #endif
            $$ = makeVARDECLARATIONtypeonly($1, $2);
        }
    ;

expOnlyVarDecl:
      // this is good - no weeding of idList necessary
      // however we need to weed to ensure the lists are of equal length
      idList '=' neExpList ';'
        {
            #ifdef BISON_DEBUG
                printf("found an expression-only variable declaration\n");
            #endif
            // weed to ensure the lists are of equal length
            $$ = makeVARDECLARATIONexponly($1, $3);
        }
    ;

typeAndExpVarDecl:
      // this is good - no weeding of idList necessary
      // however we need to weed to ensure the lists are of equal length
      idList type '=' neExpList ';'
        {
            #ifdef BISON_DEBUG
                printf("found a type-and-exp variable declaration\n");
            #endif
            // weed to ensure the lists are of equal length
            $$ = makeVARDECLARATIONtypeandexp($1, $2, $4);
        }
    ;

distributedVarDecl:
      '(' varDeclList ')' ';'
        { $$ = $2; }
    ;

varDeclList:
      //epsilon
        { $$ = makeVARDECLARATIONempty(); }
    | neVarDeclList
        { $$ = $1; }
    ;

neVarDeclList:
      regularVarDecl
        { $$ = markAsDistributedVarDecl($1); }
    | neVarDeclList regularVarDecl
        { $$ = appendVARDECLARATION($1, $2); }
    ;

/* statements */

statements:
      //epsilon
        { $$ = NULL; }
    | statements statement
        { $$ = appendSTATEMENT($1, $2); }
    ;

statement:
      simpleStatement ';'
        { $$ = $1; }
    | varDecl
        { $$ = makeSTATEMENTvardecl($1); }
    | typeDecl
        { $$ = makeSTATEMENTtypedecl($1); }
    | printStatement
        { $$ = $1; }
    | printlnStatement
        { $$ = $1; }
    | returnStatement
        { $$ = $1; }
    | ifStatement
        {
            #ifdef BISON_DEBUG
                printf("found an if statement\n");
            #endif
            $$ = $1;
        }
    | switchStatement ';'
        {
            #ifdef BISON_DEBUG
                printf("found a switch statement\n");
            #endif
            $$ = $1;
        }
    | forStatement ';'
        { $$ = $1; }
    | breakStatement
        { $$ = $1; }
    | continueStatement
        { $$ = $1; }
    | block ';'
        { $$ = makeSTATEMENTblock($1); }
    ;

simpleStatement:
      postStatement
        { $$ = $1; }
    | shortDeclStatement
        { $$ = $1; }
    ;

// can't be a short variable declaration
postStatement:
      //epsilon
        {
            #ifdef BISON_DEBUG
                printf("found an empty statement\n");
            #endif
            $$ = makeSTATEMENTempty();
        }
    | expStatement
        { $$ = $1; }
    | incDecStatement
        { $$ = $1; }
    | assignmentStatement
        { $$ = $1; }
    ;

/* exp statements */

expStatement:
      // we accept anything here but really this can only be a function call
      // we make sure of this during the weeding phase
      exp
        {
            #ifdef BISON_DEBUG
                printf("found an expression statement\n");
            #endif
            // check that this is a function call or receive operation
            // will NOT catch casts, since we think they are function calls in the parsing stage
            // thus, casts must be dealt with later on
            $$ = makeSTATEMENTexp($1);
        }
    ;

/* assignment statements */

assignmentStatement:
      // weeding ensures these are of equal length
      // and that the left neExpList consists of all lvalues
      neExpList '=' neExpList
        {
            #ifdef BISON_DEBUG
                printf("found an assignment statement\n");
            #endif
            $$ = makeSTATEMENTassign($1, $3);
        }
      // need to weed to ensure that primaryExp is an lvalue
    | primaryExp binOp exp
        {
            #ifdef BISON_DEBUG
                printf("found a binary operation assignment statement\n");
            #endif
            $$ = makeSTATEMENTbinopassign($1, $2, $3);
        }
    ;

binOp:
      tPLUSEQ
        { $$ = plusEqOp; }
    | tMINUSEQ
        { $$ = minusEqOp; }
    | tTIMESEQ
        { $$ = timesEqOp; }
    | tDIVEQ
        { $$ = divEqOp; }
    | tMODEQ
        { $$ = modEqOp; }
    | tANDEQ
        { $$ = andEqOp; }
    | tOREQ
        { $$ = orEqOp; }
    | tXOREQ
        { $$ = xorEqOp; }
    | tLEFTSHIFTEQ
        { $$ = leftShiftEqOp; }
    | tRIGHTSHIFTEQ
        { $$ = rightShiftEqOp; }
    | tBITCLEAREQ
        { $$ = bitClearEqOp; }
    ;

/* inc and dec statements */

incDecStatement:
      primaryExp tINC   // need to weed to ensure that primaryExp is an lvalue
        {
            #ifdef BISON_DEBUG
                printf("found an inc statement\n");
            #endif
            $$ = makeSTATEMENTinc($1);
        }
    | primaryExp tDEC   // need to weed to ensure that primaryExp is an lvalue
        {
            #ifdef BISON_DEBUG
                printf("found a dec statement\n");
            #endif
            $$ = makeSTATEMENTdec($1);
        }
    ;

/* short declaration statements */

shortDeclStatement:
      // weeding needed to ensure that the left neExpList consists of only identifiers
      // and that the lists are of equal length
      neExpList tSHORT neExpList
        {
            #ifdef BISON_DEBUG
                printf("found a short decl statement\n");
            #endif
            $$ = makeSTATEMENTshortdecl($1, $3);
        }
    ;

/* print and println statements */

printStatement:
      // if expList is empty, does nothing
      tPRINT '(' expList ')' ';'
        {
            #ifdef BISON_DEBUG
                printf("found a print statement\n");
            #endif
            $$ = makeSTATEMENTprint($3);
        }
    ;

printlnStatement:
      // if explist is empty, just prints a new line
      tPRINTLN '(' expList ')' ';'
        {
            #ifdef BISON_DEBUG
                printf("found a println statement\n");
            #endif
            $$ = makeSTATEMENTprintln($3);
        }
    ;

/* return statements */

returnStatement:
      tRETURN returnValue ';'
        {
            #ifdef BISON_DEBUG
                printf("found a return statement\n");
            #endif
            $$ = makeSTATEMENTreturn($2);
        }
    ;

returnValue:
      //epsilon
        { $$ = NULL; }
    | exp
        { $$ = $1; }
    ;

/* if statements */

ifStatement:
      tIF exp block ';'
        { $$ = makeSTATEMENTif(NULL, $2, $3); }
    | tIF simpleStatement ';' exp block ';'
        { $$ = makeSTATEMENTif($2, $4, $5); }
    | tIF exp block tELSE block ';'
        { $$ = makeSTATEMENTifelse(NULL, $2, $3, $5); }
    | tIF simpleStatement ';' exp block tELSE block ';'
        { $$ = makeSTATEMENTifelse($2, $4, $5, $7); }
    | tIF exp block tELSE ifStatement
        { $$ = makeSTATEMENTifelse(NULL, $2, $3, $5); }
    | tIF simpleStatement ';' exp block tELSE ifStatement
        { $$ = makeSTATEMENTifelse($2, $4, $5, $7); }
    ;

/* switch statements */

switchStatement:
      tSWITCH simpleStatement ';' exp '{' switchClauses '}'
        { $$ = makeSTATEMENTswitch($2, $4, $6); }
    | tSWITCH exp '{' switchClauses '}'
        { $$ = makeSTATEMENTswitch(NULL, $2, $4); }
    | tSWITCH '{' switchClauses '}'
        { $$ = makeSTATEMENTswitch(NULL, NULL, $3); }
    | tSWITCH simpleStatement ';' '{' switchClauses '}'
        { $$ = makeSTATEMENTswitch($2, NULL, $5); }
    ;

switchClauses:
      //epsilon
        { $$ = NULL; }
    | switchClauses switchClause
        { $$ = appendSWITCHCASE($1, $2); }
    ;

switchClause:
      tCASE neExpList ':' statements
        { $$ = makeSWITCHCASEcase($2, $4); }
    | tDEFAULT ':' statements
        { $$ = makeSWITCHCASEdefault($3); }
    ;

/* for statements */

forStatement:
      infiniteLoop
        {
            #ifdef BISON_DEBUG
                printf("found an infinite for loop\n");
            #endif
            $$ = $1;
        }
    | whileLoop
        {
            #ifdef BISON_DEBUG
                printf("found a while loop\n");
            #endif
            $$ = $1;
        }
    | threePartLoop
        {
            #ifdef BISON_DEBUG
                printf("found a three-part for statement\n");
            #endif
            $$ = $1;
        }
    ;

infiniteLoop:
      tFOR block
        { $$ = makeSTATEMENTinfiniteloop($2); }
    ;

whileLoop:
      tFOR exp block
        { $$ = makeSTATEMENTwhile($2, $3); }
    ;

threePartLoop:
      tFOR simpleStatement ';' optionalExp ';' postStatement block
        { $$ = makeSTATEMENTfor($2, $4, $6, $7); }
    ;

optionalExp:
      //epsilon
        { $$ = NULL; }
    | exp
        { $$ = $1; }
    ;

/* break and continue statements */

breakStatement:
      tBREAK ';'
        {
            #ifdef BISON_DEBUG
                printf("found a break statement\n");
            #endif
            $$ = makeSTATEMENTbreak();
        }
    ;

continueStatement:
      tCONTINUE ';'
        {
            #ifdef BISON_DEBUG
                printf("found a continue statement\n");
            #endif
            $$ = makeSTATEMENTcontinue();
        }
    ;

/* expressions */

exp:
      unaryExp
        { $$ = $1; }
    | binaryExp
        { $$ = $1; }
    | tAPPEND '(' exp ',' exp ')'
        {
            #ifdef BISON_DEBUG
                printf("found an append expression\n");
            #endif
            $$ = makeEXPappend($3, $5);
        }
    ;

expList:
      //epsilon
        { $$ = NULL; }
    | neExpList
        { $$ = $1; }
    ;

neExpList:
      exp
        { $$ = $1; }
    | neExpList ',' exp
        { $$ = appendEXP($1, $3); }
    ;

unaryExp:
      primaryExp
        { $$ = $1; }
    | '+' unaryExp %prec UNARY
        {
            #ifdef BISON_DEBUG
                printf("found a unary plus expression\n");
            #endif
            $$ = makeEXPuplus($2);
        }
    | '-' unaryExp %prec UNARY
        {
            #ifdef BISON_DEBUG
                printf("found a unary minus expression\n");
            #endif
            $$ = makeEXPuminus($2);
        }
    | '!' unaryExp %prec UNARY
        {
            #ifdef BISON_DEBUG
                printf("found a unary not expression\n");
            #endif
            $$ = makeEXPunot($2);
        }
    | '^' unaryExp %prec UNARY
        {
            #ifdef BISON_DEBUG
                printf("found a unary xor expression\n");
            #endif
            $$ = makeEXPuxor($2);
        }
    ;

binaryExp:
      exp '+' exp
        {
            #ifdef BISON_DEBUG
                printf("found a plus exp\n");
            #endif
            $$ = makeEXPplus($1, $3);
        }
    | exp '-' exp
        {
            #ifdef BISON_DEBUG
                printf("found a minus exp\n");
            #endif
            $$ = makeEXPminus($1, $3);
        }
    | exp '*' exp
        {
            #ifdef BISON_DEBUG
                printf("found a times exp\n");
            #endif
            $$ = makeEXPtimes($1, $3);
        }
    | exp '/' exp
        {
            #ifdef BISON_DEBUG
                printf("found a div exp\n");
            #endif
            $$ = makeEXPdiv($1, $3);
        }
    | exp '%' exp
        {
            #ifdef BISON_DEBUG
                printf("found a mod exp\n");
            #endif
            $$ = makeEXPmod($1, $3);
        }
    | exp '|' exp
        {
            #ifdef BISON_DEBUG
                printf("found a bitwise or exp\n");
            #endif
            $$ = makeEXPbitwiseor($1, $3);
        }
    | exp '&' exp
        {
            #ifdef BISON_DEBUG
                printf("found a bitwise and exp\n");
            #endif
            $$ = makeEXPbitwiseand($1, $3);
        }
    | exp '^' exp
        {
            #ifdef BISON_DEBUG
                printf("found an xor exp\n");
            #endif
            $$ = makeEXPxor($1, $3);
        }
    | exp '<' exp
        {
            #ifdef BISON_DEBUG
                printf("found a less-than exp\n");
            #endif
            $$ = makeEXPlt($1, $3);
        }
    | exp '>' exp
        {
            #ifdef BISON_DEBUG
                printf("found a greater than exp\n");
            #endif
            $$ = makeEXPgt($1, $3);
        }
    | exp tEQ exp
        {
            #ifdef BISON_DEBUG
                printf("found an equality exp\n");
            #endif
            $$ = makeEXPeq($1, $3);
        }
    | exp tNEQ exp
        {
            #ifdef BISON_DEBUG
                printf("found a not-equal-to exp\n");
            #endif
            $$ = makeEXPneq($1, $3);
        }
    | exp tLEQ exp
        {
            #ifdef BISON_DEBUG
                printf("found a leq exp\n");
            #endif
            $$ = makeEXPleq($1, $3);
        }
    | exp tGEQ exp
        {
            #ifdef BISON_DEBUG
                printf("found a geq exp\n");
            #endif
            $$ = makeEXPgeq($1, $3);
        }
    | exp tOR exp
        {
            #ifdef BISON_DEBUG
                printf("found an or exp\n");
            #endif
            $$ = makeEXPor($1, $3);
        }
    | exp tAND exp
        {
            #ifdef BISON_DEBUG
                printf("found an and exp\n");
            #endif
            $$ = makeEXPand($1, $3);
        }
    | exp tLEFTSHIFT exp
        {
            #ifdef BISON_DEBUG
                printf("found a left-shift exp\n");
            #endif
            $$ = makeEXPleftshift($1, $3);
        }
    | exp tRIGHTSHIFT exp
        {
            #ifdef BISON_DEBUG
                printf("found a right-shift exp\n");
            #endif
            $$ = makeEXPrightshift($1, $3);
        }
    | exp tBITCLEAR exp
        {
            #ifdef BISON_DEBUG
                printf("found a bit-clear exp\n");
            #endif
            $$ = makeEXPbitclear($1, $3);
        }
    ;

primaryExp:
      operand
        { $$ = $1; }
    | primaryExp '.' identifier // field access
        {
            #ifdef BISON_DEBUG
                printf("found a selector\n");
            #endif
            $$ = makeEXPselector(makeRECEIVER($1), $3);
        }
    | primaryExp '[' exp ']' // array indexing
        {
            #ifdef BISON_DEBUG
                printf("found an index into an array\n");
            #endif
            $$ = makeEXPindex($1, $3);
        }
    | primaryExp '(' expList ')' // function call
        { $$ = makeEXParguments($1, $3); }
    ;

operand:
      basicLiteral
        { $$ = $1; }
    | identifier
        {
            #ifdef BISON_DEBUG
                printf("found an identifier operand: %s\n", ($1)->name);
            #endif
            $$ = makeEXPid($1);
        }
    | '(' exp ')'
        {
            #ifdef BISON_DEBUG
                printf("found a parenthesized expression operand\n");
            #endif
            $$ = markEXPparenthesized($2);
        }
    ;

basicLiteral:
      intLiteral
        { $$ = $1; }
    | tFLOAT64LITERAL
        {
            #ifdef BISON_DEBUG
                printf("found a float literal\n");
            #endif
            $$ = makeEXPfloatliteral($1);
        }
    | tRUNELITERAL
        {
            #ifdef BISON_DEBUG
                printf("found a rune literal\n");
            #endif
            $$ = makeEXPruneliteral($1);
        }
    | tSTRINGLITERAL
        {
            #ifdef BISON_DEBUG
                printf("found an interpretted string literal\n");
            #endif
            $$ = makeEXPstringliteral($1);
        }
    | tRAWSTRINGLITERAL
        {
            #ifdef BISON_DEBUG
                printf("found a raw string literal\n");
            #endif
            $$ = makeEXPrawstringliteral($1);
        }
    ;

intLiteral:
      tINTDECLITERAL
        {
            #ifdef BISON_DEBUG
                printf("found an int dec literal\n");
            #endif
            $$ = makeEXPintdecliteral($1);
        }
    | tINTOCTLITERAL
        {
            #ifdef BISON_DEBUG
                printf("found an int oct literal\n");
            #endif
            $$ = makeEXPintoctliteral($1);
        }
    | tINTHEXLITERAL
        {
            #ifdef BISON_DEBUG
                printf("found an int hex literal\n");
            #endif
            $$ = makeEXPinthexliteral($1);
        }
    ;

/* identifiers */

identifier:
      tIDENTIFIER
        { $$ = makeID($1); }
    ;

idList:
      identifier
        { $$ = $1; }
    | idList ',' identifier
        { $$ = appendID($1, $3); }
    ;

/* types */

type:
      identifier
        { $$ = makeTYPEid($1); }
    | typeLit
        { $$ = $1; }
    | '(' type ')'
        { $$ = $2; }
    ;

typeLit:
      arrayType
        {
            #ifdef BISON_DEBUG
                printf("found an array type\n");
            #endif
            $$ = $1;
        }
    | structType
        {
            #ifdef BISON_DEBUG
                printf("found a struct type\n");
            #endif
            $$ = $1;
        }
    | sliceType
        {
            #ifdef BISON_DEBUG
                printf("found a slice type\n");
            #endif
            $$ = $1;
        }
    ;

arrayType:
      '[' intLiteral ']' type
        { $$ = makeTYPEarray($2, $4); }
    ;

structType:
      tSTRUCT '{' fields '}'
        { $$ = makeTYPEstruct($3); }
    ;

sliceType:
      '[' ']' type
        { $$ = makeTYPEslice($3); }
    ;

/* fields */

fields:
      //epsilon
        { $$ = makeSTRUCTTYPE(NULL); }
    | neFields
        { $$ = makeSTRUCTTYPE($1); }
    ;

neFields:
      field
        { $$ = $1; }
    | neFields field
        { $$ = appendFIELD($1, $2); }
    ;

field:
      // this is good - no weeding of the idList needed
      idList type ';'
        { $$ = makeFIELD($1, $2); }
    ;

/* block */

block:
      '{' statements '}'
        { $$ = $2; }
    ;

%%

//--END--
