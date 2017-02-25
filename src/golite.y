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
%token <stringLiteral> tSTRINGLITERAL tIDENTIFIER

%type <package> packageDecl
%type <topLevelDecl> topLevelDecls topLevelDecl decl
%type <varDecl> varDecl varDeclRest regularVarDecl typeOnlyVarDecl expOnlyVarDecl
                typeAndExpVarDecl distributedVarDecl varDeclList
%type <typeDecl> typeDecl typeDeclRest regularTypeDecl distributedTypeDecl typeDeclList
%type <functionDecl> functionDecl
%type <parameter> parameterList neParameterList parameter
%type <statement> statements statement simpleStatement printStatement printlnStatement
                  returnStatement ifStatement switchStatement forStatement breakStatement
                  continueStatement postStatement shortDeclStatement expStatement
                  incDecStatement assignmentStatement infiniteLoop whileLoop threePartLoop
                  block
%type <switchCase> switchClauses switchClause
%type <exp> exp returnValue unaryExp binaryExp expList neExpList primaryExp
            basicLiteral operand
//%type <cast> conversion
%type <type> type typeLit arrayType structType sliceType returnType
%type <id> identifier idList
%type <field> fields field neFields
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
        { theprogram = makePROGRAM($1, $2); }
    ;

packageDecl:
      tPACKAGE identifier ';'
        { $$ = makePACKAGE($2); }
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
      tFUNC identifier '(' parameterList ')' returnType block
        { $$ = makeFUNCTIONDECLARATION($2, $4, $6, $7); }
    ;

parameterList:
      //epsilon
        { $$ = NULL; }
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
        { $$ = makePARAMETER($1, $2); }
    ;

returnType:
      //epsilon
        { $$ = NULL; }
    | type
        { $$ = $1; }
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
        { $$ = makeTYPEDECLARATION($1, $2); }
    ;

distributedTypeDecl:
      '(' typeDeclList ')' ';'
        { $$ = $2; }
    ;

typeDeclList:
      regularTypeDecl
        { $$ = markAsDistributedTypeDecl($1); }
    | typeDeclList regularTypeDecl
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
        { $$ = makeVARDECLARATIONtypeonly($1, $2); }
    ;

expOnlyVarDecl:
      // this is good - no weeding of idList necessary
      // however we need to weed to ensure the lists are of equal length
      idList '=' neExpList ';'
        { $$ = makeVARDECLARATIONexponly($1, $3); }
    ;

typeAndExpVarDecl:
      // this is good - no weeding of idList necessary
      // however we need to weed to ensure the lists are of equal length
      idList type '=' neExpList ';'
        { $$ = makeVARDECLARATIONtypeandexp($1, $2, $4); }
    ;

distributedVarDecl:
      '(' varDeclList ')' ';'
        { $$ = $2; }
    ;

varDeclList:
      regularVarDecl
        { $$ = markAsDistributedVarDecl($1); }
    | varDeclList regularVarDecl
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
        { $$ = $1; }
    | switchStatement ';'
        { $$ = $1; }
    | forStatement ';'
        { $$ = $1; }
    | breakStatement
        { $$ = $1; }
    | continueStatement
        { $$ = $1; }
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
        { $$ = makeSTATEMENTempty(); }
    | expStatement
        { $$ = $1; }
    | incDecStatement
        { $$ = $1; }
    | assignmentStatement
        { $$ = $1; }
    ;

/* exp statements */

expStatement:
      // we accept anything here but really this can only be a function call or receive operation
      // we make sure of this during the weeding phase
      exp
        { $$ = makeSTATEMENTexp($1); }
    ;

/* assignment statements */

assignmentStatement:
      // weeding ensures these are of equal length
      // and that the left neExpList consists of all lvalues
      neExpList '=' neExpList ';'
        { $$ = makeSTATEMENTassign($1, $3); }
      // need to weed to ensure that primaryExp is an lvalue
    | primaryExp binOp exp ';'
        { $$ = makeSTATEMENTbinopassign($1, $2, $3); }
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
      primaryExp tINC ';'   // need to weed to ensure that primaryExp is an lvalue
        { $$ = makeSTATEMENTinc($1); }
    | primaryExp tDEC ';'   // need to weed to ensure that primaryExp is an lvalue
        { $$ = makeSTATEMENTdec($1); }
    ;

/* short declaration statements */

shortDeclStatement:
      // weeding needed to ensure that the left neExpList consists of only identifiers
      // and that the lists are of equal length
      neExpList ':' '=' neExpList ';'
        { $$ = makeSTATEMENTshortdecl($1, $4); }
    ;

/* print and println statements */

printStatement:
      // if expList is empty, does nothing
      tPRINT '(' expList ')' ';'
        { $$ = makeSTATEMENTprint($3); }
    ;

printlnStatement:
      // if explist is empty, just prints a new line
      tPRINTLN '(' expList ')' ';'
        { $$ = makeSTATEMENTprintln($3); }
    ;

/* return statements */

returnStatement:
      tRETURN returnValue ';'
        { $$ = makeSTATEMENTreturn($2); }
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
    ;

switchClauses:
      //epsilon
        { $$ = NULL; }
    | switchClauses switchClause
        { $$ = appendSWITCHCASE($1, $2); }
    ;

switchClause:
      tCASE expList ':' statements
        { $$ = makeSWITCHCASEcase($2, $4); }
    | tDEFAULT ':' statements
        { $$ = makeSWITCHCASEdefault($3); }
    ;

/* for statements */

forStatement:
      infiniteLoop
        { $$ = $1; }
    | whileLoop
        { $$ = $1; }
    | threePartLoop
        { $$ = $1; }
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
      tFOR simpleStatement ';' exp ';' postStatement block
        { $$ = makeSTATEMENTfor($2, $4, $6, $7); }
    ;

/* break and continue statements */

breakStatement:
      tBREAK ';'
        { $$ = makeSTATEMENTbreak(); }
    ;

continueStatement:
      tCONTINUE ';'
        { $$ = makeSTATEMENTcontinue(); }
    ;

/* expressions */

exp:
      unaryExp
        { $$ = $1; }
    | binaryExp
        { $$ = $1; }
    | tAPPEND '(' exp ',' exp ')'
        { $$ = makeEXPappend($3, $5); }
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
        { $$ = makeEXPuplus($2); }
    | '-' unaryExp %prec UNARY
        { $$ = makeEXPuminus($2); }
    | '!' unaryExp %prec UNARY
        { $$ = makeEXPunot($2); }
    | '^' unaryExp %prec UNARY
        { $$ = makeEXPuxor($2); }
    | tLARROW unaryExp %prec UNARY
        { $$ = makeEXPureceive($2); }
    ;

binaryExp:
      exp '+' exp
        { $$ = makeEXPplus($1, $3); }
    | exp '-' exp
        { $$ = makeEXPminus($1, $3); }
    | exp '*' exp
        { $$ = makeEXPtimes($1, $3); }
    | exp '/' exp
        { $$ = makeEXPdiv($1, $3); }
    | exp '%' exp
        { $$ = makeEXPmod($1, $3); }
    | exp '|' exp
        { $$ = makeEXPbitwiseor($1, $3); }
    | exp '&' exp
        { $$ = makeEXPbitwiseand($1, $3); }
    | exp '^' exp
        { $$ = makeEXPxor($1, $3); }
    | exp '<' exp
        { $$ = makeEXPlt($1, $3); }
    | exp '>' exp
        { $$ = makeEXPgt($1, $3); }
    | exp tEQ exp
        { $$ = makeEXPeq($1, $3); }
    | exp tNEQ exp
        { $$ = makeEXPneq($1, $3); }
    | exp tLEQ exp
        { $$ = makeEXPleq($1, $3); }
    | exp tGEQ exp
        { $$ = makeEXPgeq($1, $3); }
    | exp tOR exp
        { $$ = makeEXPor($1, $3); }
    | exp tAND exp
        { $$ = makeEXPand($1, $3); }
    | exp tLEFTSHIFT exp
        { $$ = makeEXPleftshift($1, $3); }
    | exp tRIGHTSHIFT exp
        { $$ = makeEXPrightshift($1, $3); }
    | exp tBITCLEAR exp
        { $$ = makeEXPbitclear($1, $3); }
    ;

primaryExp:
      operand
        { $$ = $1; }
    //| conversion // a cast
    //    { $$ = makeEXPcast($1); }
    | primaryExp '.' identifier // field access
        { $$ = makeEXPselector($1, $3); }
    | primaryExp '[' exp ']' // array indexing
        { $$ = makeEXPindex($1, $3); }
    | primaryExp '(' expList ')' // function call
        { $$ = makeEXParguments($1, $3); }
    ;

operand:
      basicLiteral
        { $$ = $1; }
    | identifier
        { $$ = makeEXPid($1); }
    | '(' exp ')'
        { $$ = $2; }
    ;

/*
conversion:
      tINT '(' exp ')'
        { $$ = makeCAST($3); }
    | tFLOAT '(' exp ')'
        { $$ = makeCAST($3); }
    | tRUNE '(' exp ')'
        { $$ = makeCAST($3); }
    | tSTRING '(' exp ')'
        { $$ = makeCAST($3); }
    ;
*/

basicLiteral:
      tINTDECLITERAL
        { $$ = makeEXPintdecliteral($1); }
    | tINTOCTLITERAL
        { $$ = makeEXPintoctliteral($1); }
    | tINTHEXLITERAL
        { $$ = makeEXPinthexliteral($1); }
    | tFLOAT64LITERAL
        { $$ = makeEXPfloatliteral($1); }
    | tRUNELITERAL
        { $$ = makeEXPruneliteral($1); }
    | tSTRINGLITERAL
        { $$ = makeEXPstringliteral($1); }
    ;

/*
exp:
      unaryExp
    | binaryExp
    ;

expList:
      //epsilon
    | neExpList
    ;

neExpList:
      exp
    | neExpList ',' exp
    ;

unaryExp:
      primaryExp
    | '+' unaryExp %prec UNARY
    | '-' unaryExp %prec UNARY
    | '!' unaryExp %prec UNARY
    | '^' unaryExp %prec UNARY
    | tLARROW unaryExp %prec UNARY
    ;

binaryExp:
      exp '+' exp
    | exp '-' exp
    | exp '*' exp
    | exp '/' exp
    | exp '%' exp
    | exp '|' exp
    | exp '&' exp
    | exp '^' exp
    | exp '<' exp
    | exp '>' exp
    | exp tEQ exp
    | exp tNEQ exp
    | exp tLEQ exp
    | exp tGEQ exp
    | exp tOR exp
    | exp tAND exp
    | exp tLEFTSHIFT exp
    | exp tRIGHTSHIFT exp
    | exp tBITCLEAR exp
    ;

primaryExp:
      operand
    | conversion // a cast
    | primaryExp selector // field access
    | primaryExp index // array indexing
    | primaryExp slice // slice access
    | primaryExp arguments // function call
    ;

arguments:
      '(' expList ')'
    ;

selector:
      '.' identifier
    ;

index:
      '[' exp ']'
    ;

slice:
      '[' optionalExp ':' optionalExp ']'
    | '[' optionalExp ':' exp ':' exp ']'
    ;

optionalExp:
      //epsilon
    | exp
    ;

operand:
      literal
    | identifier
    | '(' exp ')'
    ;

conversion:
      type '(' exp ')'
    ;

literal:
      // we don't support composite or function literals
      basicLiteral
    ;

basicLiteral:
      tINTDECLITERAL
    | tINTOCTLITERAL
    | tINTHEXLITERAL
    | tFLOAT64LITERAL
    | tRUNELITERAL
    | tSTRINGLITERAL
    ;
*/


/*
unaryExp:
      '+' exp %prec UNARY
    | '-' exp %prec UNARY
    | '!' exp %prec UNARY
    | '^' exp %prec UNARY
    ;

binaryExp:
      exp '+' exp
    | exp '-' exp
    | exp '*' exp
    | exp '/' exp
    | exp '%' exp
    | exp '|' exp
    | exp '&' exp
    | exp '^' exp
    | exp '<' exp
    | exp '>' exp
    | exp tEQ exp
    | exp tNEQ exp
    | exp tLEQ exp
    | exp tGEQ exp
    | exp tOR exp
    | exp tAND exp
    | exp tLEFTSHIFT exp
    | exp tRIGHTSHIFT exp
    | exp tBITCLEAR exp
    ;
*/

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
    ;

typeLit:
      arrayType
        { $$ = $1; }
    | structType
        { $$ = $1; }
    | sliceType
        { $$ = $1; }
    ;

arrayType:
      '[' exp ']' type
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
        { $$ = NULL; }
    | neFields
        { $$ = $1; }
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
