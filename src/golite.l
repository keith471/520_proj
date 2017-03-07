%{
#include <stdio.h>  // for printf
#include <stdlib.h> // for strtol
#include "y.tab.h"  // to get the token types that we return to Bison
#include "error.h"
int shouldInsertBeforeNewLine = 0;
int shouldInsertBeforeRightBrace = 0;
%}
%option yylineno
%%

break           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'break'\n");
                    #endif
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tBREAK;
                }
case            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'case'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tCASE;
                }
chan            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'chan'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tCHAN;
                }
const           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'const'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tCONST;
                }
continue        {
                    #ifdef FLEX_DEBUG
                        printf("found a 'continue'\n");
                    #endif
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tCONTINUE;
                }
default         {
                    #ifdef FLEX_DEBUG
                        printf("found a 'default'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tDEFAULT;
                }
defer           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'defer'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tDEFER;
                }
else            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'else'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tELSE;
                }
fallthrough     {
                    #ifdef FLEX_DEBUG
                        printf("found a 'fallthrough'\n");
                    #endif
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tFALLTHROUGH;
                }
for             {
                    #ifdef FLEX_DEBUG
                        printf("found a 'for'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tFOR;
                }
func            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'func'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tFUNC;
                }
go              {
                    #ifdef FLEX_DEBUG
                        printf("found a 'go'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tGO;
                }
goto            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'goto'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tGOTO;
                }
if              {
                    #ifdef FLEX_DEBUG
                        printf("found a 'if'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tIF;
                }
import          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'import'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tIMPORT;
                }
interface       {
                    #ifdef FLEX_DEBUG
                        printf("found a 'interface'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tINTERFACE;
                }
map             {
                    #ifdef FLEX_DEBUG
                        printf("found a 'map'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tMAP;
                }
package         {
                    #ifdef FLEX_DEBUG
                        printf("found a 'package'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tPACKAGE;
                }
range           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'range'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tRANGE;
                }
return          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'return'\n");
                    #endif
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tRETURN;
                }
select          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'select'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tSELECT;
                }
struct          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'struct'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tSTRUCT;
                }
switch          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'switch'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tSWITCH;
                }
type            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'type'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tTYPE;
                }
var             {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tVAR;
                }
print           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tPRINT;
                }
println         {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tPRINTLN;
                }
append          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsertBeforeNewLine = 0;
                    shouldInsertBeforeRightBrace = 1;
                    return tAPPEND;
                }

"="             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '='; }
">"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '>'; }
"<"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '<'; }
"!"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '!'; }
"=="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tEQ; }
"<="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tLEQ; }
">="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tGEQ; }
"!="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tNEQ; }
":="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tSHORT; }
"&&"            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tAND; }
"||"            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tOR; }
"&"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '&'; }
"|"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '|'; }
"^"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '^'; }
"<<"            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tLEFTSHIFT; }
">>"            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tRIGHTSHIFT; }
"&^"            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tBITCLEAR; }
"+"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '+'; }
"-"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '-'; }
"*"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '*'; }
"/"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '/'; }
"%"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '%'; }
"{"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 0; return '{'; }
"}"             {
                    if (shouldInsertBeforeRightBrace) {
                        unput('}');
                        shouldInsertBeforeRightBrace = 0;
                        return ';';
                    } else {
                        shouldInsertBeforeNewLine = 1;
                        return '}';
                    }
                }
";"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 0;  return ';'; }
":"             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return ':'; }
"("             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '('; }
")"             { shouldInsertBeforeNewLine = 1; shouldInsertBeforeRightBrace = 1; return ')'; }
"["             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '['; }
"]"             { shouldInsertBeforeNewLine = 1; shouldInsertBeforeRightBrace = 1; return ']'; }
","             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return ','; }
"."             { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return '.'; }
"+="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tPLUSEQ; }
"-="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tMINUSEQ; }
"*="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tTIMESEQ; }
"/="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tDIVEQ; }
"%="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tMODEQ; }
"&="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tANDEQ; }
"|="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tOREQ; }
"^="            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tXOREQ; }
"<<="           { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tLEFTSHIFTEQ; }
">>="           { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tRIGHTSHIFTEQ; }
"&^="           { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tBITCLEAREQ; }
"++"            { shouldInsertBeforeNewLine = 1; shouldInsertBeforeRightBrace = 1; return tINC; }
"--"            { shouldInsertBeforeNewLine = 1; shouldInsertBeforeRightBrace = 1; return tDEC; }
"<-"            { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tLARROW; }
"..."           { shouldInsertBeforeNewLine = 0; shouldInsertBeforeRightBrace = 1; return tELLIPSIS; }

[ \t\r]         { ; }
\n              {
                    if (shouldInsertBeforeNewLine) {
                        unput('\n');
                        shouldInsertBeforeNewLine = 0;
                        shouldInsertBeforeRightBrace = 0;
                        return ';';
                    }
                }
\/\/.*          {
                    #ifdef FLEX_DEBUG
                        printf("found a single-line comment: %s\n", yytext);
                    #endif
                }
[a-zA-Z_][a-zA-Z_0-9]* {
                    #ifdef FLEX_DEBUG
                        printf("found an identifier: %s\n", yytext);
                    #endif
                    yylval.stringLiteral = strdup(yytext);
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tIDENTIFIER;
                }
((\.[0-9]+)|((0|[1-9][0-9]*)\.[0-9]*)) {
                    #ifdef FLEX_DEBUG
                        printf("found a float literal: %s\n", yytext);
                    #endif
                    yylval.floatLiteral = atof(yytext);
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tFLOAT64LITERAL;
                }
0|[1-9][0-9]*   {
                    #ifdef FLEX_DEBUG
                        printf("found a decimal int literal: %s\n", yytext);
                    #endif
                    yylval.intLiteral = atoi(yytext);
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tINTDECLITERAL;
                }
0[0-7]*         {
                    #ifdef FLEX_DEBUG
                        printf("found an octal int literal: %s\n", yytext);
                    #endif
                    yylval.intLiteral = (int) strtol(yytext, NULL, 8);
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tINTOCTLITERAL;
                }
0[xX][a-fA-F0-9]+ {
                    #ifdef FLEX_DEBUG
                        printf("found an octal int literal: %s\n", yytext);
                    #endif
                    yylval.intLiteral = (int) strtol(yytext, NULL, 16);
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tINTHEXLITERAL;
                }
'(\\[abfnrtv'\\]|[a-zA-Z 0123456789~`@#$%^&*()-_=+{}[\]|:;"\/<>,.])' {
                    #ifdef FLEX_DEBUG
                        printf("found a rune literal: %s\n", yytext);
                    #endif
                    yylval.runeLiteral = yytext[0];
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tRUNELITERAL;
                }
\"(?:\\[abfnrtv"\\]|[a-zA-Z0-9 ~#$%^&*\-+\/`<>=_|';:{}\[\]()@!?.,])*\" {
                    #ifdef FLEX_DEBUG
                        printf("found a string literal: %s\n", yytext);
                    #endif
                    yylval.stringLiteral = strdup(yytext);
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tSTRINGLITERAL;
                }
`[a-zA-Z0-9 \\~#$%^&*\-+\/<>=_|'";:{}\[\]()@!?.,]*` {
                    #ifdef FLEX_DEBUG
                        printf("found a raw string literal: %s\n", yytext);
                    #endif
                    yylval.stringLiteral = strdup(yytext);
                    shouldInsertBeforeNewLine = 1;
                    shouldInsertBeforeRightBrace = 1;
                    return tRAWSTRINGLITERAL;
                }
.               {
                    #ifdef FLEX_DEBUG
                        printf("found an unrecognized symbol -- throwing an error!\n");
                    #endif
                    yyerror(yytext);
                }
\/\*[^*]*\*+(?:[^\/*][^*]*\*+)*\/ {
                    #ifdef FLEX_DEBUG
                        printf("found a multi-line comment: %s\n", yytext);
                    #endif
                }
%%

/*
int             { shouldInsertBeforeNewLine = 1; yylval.stringLiteral = strdup(yytext); return tINT; }
float64         { shouldInsertBeforeNewLine = 1; yylval.stringLiteral = strdup(yytext); return tFLOAT; }
rune            { shouldInsertBeforeNewLine = 1; yylval.stringLiteral = strdup(yytext); return tRUNE; }
string          { shouldInsertBeforeNewLine = 1; yylval.stringLiteral = strdup(yytext); return tSTRING; }
*/

/*
int             { shouldInsertBeforeNewLine = 1; return tINT; }
float64         { shouldInsertBeforeNewLine = 1; return tFLOAT; }
bool            { shouldInsertBeforeNewLine = 1; return tBOOL; }
rune            { shouldInsertBeforeNewLine = 1; return tRUNE; }
string          { shouldInsertBeforeNewLine = 1; return tSTRING; }
true            { shouldInsertBeforeNewLine = 1; yylval.boolliteral = 1; return tBOOLLITERAL; }
false           { shouldInsertBeforeNewLine = 1; yylval.boolliteral = 0; return tBOOLLITERAL; }
*/
