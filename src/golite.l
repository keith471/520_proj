%{
#include <stdio.h>  // for printf
#include <stdlib.h> // for strtol
#include "y.tab.h"  // to get the token types that we return to Bison
#include "error.h"
int shouldInsert = 0;
%}
%option yylineno
%%

break           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'break'\n");
                    #endif
                    shouldInsert = 1;
                    return tBREAK;
                }
case            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'case'\n");
                    #endif
                    shouldInsert = 0;
                    return tCASE;
                }
chan            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'chan'\n");
                    #endif
                    shouldInsert = 0;
                    return tCHAN;
                }
const           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'const'\n");
                    #endif
                    shouldInsert = 0;
                    return tCONST;
                }
continue        {
                    #ifdef FLEX_DEBUG
                        printf("found a 'continue'\n");
                    #endif
                    shouldInsert = 1;
                    return tCONTINUE;
                }
default         {
                    #ifdef FLEX_DEBUG
                        printf("found a 'default'\n");
                    #endif
                    shouldInsert = 0;
                    return tDEFAULT;
                }
defer           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'defer'\n");
                    #endif
                    shouldInsert = 0;
                    return tDEFER;
                }
else            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'else'\n");
                    #endif
                    shouldInsert = 0;
                    return tELSE;
                }
fallthrough     {
                    #ifdef FLEX_DEBUG
                        printf("found a 'fallthrough'\n");
                    #endif
                    shouldInsert = 1;
                    return tFALLTHROUGH;
                }
for             {
                    #ifdef FLEX_DEBUG
                        printf("found a 'for'\n");
                    #endif
                    shouldInsert = 0;
                    return tFOR;
                }
func            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'func'\n");
                    #endif
                    shouldInsert = 0;
                    return tFUNC;
                }
go              {
                    #ifdef FLEX_DEBUG
                        printf("found a 'go'\n");
                    #endif
                    shouldInsert = 0;
                    return tGO;
                }
goto            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'goto'\n");
                    #endif
                    shouldInsert = 0;
                    return tGOTO;
                }
if              {
                    #ifdef FLEX_DEBUG
                        printf("found a 'if'\n");
                    #endif
                    shouldInsert = 0;
                    return tIF;
                }
import          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'import'\n");
                    #endif
                    shouldInsert = 0;
                    return tIMPORT;
                }
interface       {
                    #ifdef FLEX_DEBUG
                        printf("found a 'interface'\n");
                    #endif
                    shouldInsert = 0;
                    return tINTERFACE;
                }
map             {
                    #ifdef FLEX_DEBUG
                        printf("found a 'map'\n");
                    #endif
                    shouldInsert = 0;
                    return tMAP;
                }
package         {
                    #ifdef FLEX_DEBUG
                        printf("found a 'package'\n");
                    #endif
                    shouldInsert = 0;
                    return tPACKAGE;
                }
range           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'range'\n");
                    #endif
                    shouldInsert = 0;
                    return tRANGE;
                }
return          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'return'\n");
                    #endif
                    shouldInsert = 1;
                    return tRETURN;
                }
select          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'select'\n");
                    #endif
                    shouldInsert = 0;
                    return tSELECT;
                }
struct          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'struct'\n");
                    #endif
                    shouldInsert = 0;
                    return tSTRUCT;
                }
switch          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'switch'\n");
                    #endif
                    shouldInsert = 0;
                    return tSWITCH;
                }
type            {
                    #ifdef FLEX_DEBUG
                        printf("found a 'type'\n");
                    #endif
                    shouldInsert = 0;
                    return tTYPE;
                }
var             {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsert = 0;
                    return tVAR;
                }
print           {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsert = 0;
                    return tPRINT;
                }
println         {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsert = 0;
                    return tPRINTLN;
                }
append          {
                    #ifdef FLEX_DEBUG
                        printf("found a 'var'\n");
                    #endif
                    shouldInsert = 0;
                    return tAPPEND;
                }

"="             { shouldInsert = 0; return '='; }
">"             { shouldInsert = 0; return '>'; }
"<"             { shouldInsert = 0; return '<'; }
"!"             { shouldInsert = 0; return '!'; }
"=="            { shouldInsert = 0; return tEQ; }
"<="            { shouldInsert = 0; return tLEQ; }
">="            { shouldInsert = 0; return tGEQ; }
"!="            { shouldInsert = 0; return tNEQ; }
":="            { shouldInsert = 0; return tSHORT; }
"&&"            { shouldInsert = 0; return tAND; }
"||"            { shouldInsert = 0; return tOR; }
"&"             { shouldInsert = 0; return '&'; }
"|"             { shouldInsert = 0; return '|'; }
"^"             { shouldInsert = 0; return '^'; }
"<<"            { shouldInsert = 0; return tLEFTSHIFT; }
">>"            { shouldInsert = 0; return tRIGHTSHIFT; }
"&^"            { shouldInsert = 0; return tBITCLEAR; }
"+"             { shouldInsert = 0; return '+'; }
"-"             { shouldInsert = 0; return '-'; }
"*"             { shouldInsert = 0; return '*'; }
"/"             { shouldInsert = 0; return '/'; }
"%"             { shouldInsert = 0; return '%'; }
"{"             { shouldInsert = 0; return '{'; }
"}"             { shouldInsert = 1; return '}'; }
";"             { shouldInsert = 0; return ';'; }
":"             { shouldInsert = 0; return ':'; }
"("             { shouldInsert = 0; return '('; }
")"             { shouldInsert = 1; return ')'; }
"["             { shouldInsert = 0; return '['; }
"]"             { shouldInsert = 1; return ']'; }
","             { shouldInsert = 0; return ','; }
"."             { shouldInsert = 0; return '.'; }
"+="            { shouldInsert = 0; return tPLUSEQ; }
"-="            { shouldInsert = 0; return tMINUSEQ; }
"*="            { shouldInsert = 0; return tTIMESEQ; }
"/="            { shouldInsert = 0; return tDIVEQ; }
"%="            { shouldInsert = 0; return tMODEQ; }
"&="            { shouldInsert = 0; return tANDEQ; }
"|="            { shouldInsert = 0; return tOREQ; }
"^="            { shouldInsert = 0; return tXOREQ; }
"<<="           { shouldInsert = 0; return tLEFTSHIFTEQ; }
">>="           { shouldInsert = 0; return tRIGHTSHIFTEQ; }
"&^="           { shouldInsert = 0; return tBITCLEAREQ; }
"++"            { shouldInsert = 1; return tINC; }
"--"            { shouldInsert = 1; return tDEC; }
"<-"            { shouldInsert = 0; return tLARROW; }
"..."           { shouldInsert = 0; return tELLIPSIS; }

[ \t\r]         { shouldInsert = 0; }
\n              {
                    if (shouldInsert) {
                        unput('\n');
                        shouldInsert = 0;
                        return ';';
                    }
                }
\/\/.*          {
                    #ifdef FLEX_DEBUG
                        printf("found a single-line comment: %s\n", yytext);
                    #endif
                }
[a-zA-z_][a-zA-z_0-9]* {
                    #ifdef FLEX_DEBUG
                        printf("found an identifier: %s\n", yytext);
                    #endif
                    yylval.stringLiteral = strdup(yytext);
                    shouldInsert = 1;
                    return tIDENTIFIER;
                }
(0|[1-9][0-9]*)\.[0-9]* {
                    #ifdef FLEX_DEBUG
                        printf("found a float literal: %s\n", yytext);
                    #endif
                    yylval.floatLiteral = atof(yytext);
                    shouldInsert = 1;
                    return tFLOAT64LITERAL;
                }
0|[1-9][0-9]*   {
                    #ifdef FLEX_DEBUG
                        printf("found a decimal int literal: %s\n", yytext);
                    #endif
                    yylval.intLiteral = atoi(yytext);
                    shouldInsert = 1;
                    return tINTDECLITERAL;
                }
0[0-7]*         {
                    #ifdef FLEX_DEBUG
                        printf("found an octal int literal: %s\n", yytext);
                    #endif
                    yylval.intLiteral = (int) strtol(yytext, NULL, 8);
                    shouldInsert = 1;
                    return tINTOCTLITERAL;
                }
0[xX][a-fA-F0-9]+ {
                    #ifdef FLEX_DEBUG
                        printf("found an octal int literal: %s\n", yytext);
                    #endif
                    yylval.intLiteral = (int) strtol(yytext, NULL, 16);
                    shouldInsert = 1;
                    return tINTHEXLITERAL;
                }
'(\\[abfnrtv'\\]|[a-zA-Z 0123456789~`@#$%^&*()-_=+{}[\]|:;"\/<>,.])' {
                    #ifdef FLEX_DEBUG
                        printf("found a rune literal: %s\n", yytext);
                    #endif
                    yylval.runeLiteral = yytext[0];
                    shouldInsert = 1;
                    return tRUNELITERAL;
                }
\"(?:\\[abfnrtv"\\]|[a-zA-Z0-9 ~#$%^&*\-+\/`<>=_|';:{}\[\]()@!?.,])*\" {
                    #ifdef FLEX_DEBUG
                        printf("found a string literal: %s\n", yytext);
                    #endif
                    yylval.stringLiteral = strdup(yytext);
                    shouldInsert = 1;
                    return tSTRINGLITERAL;
                }
\'(?:\\[abfnrtv'\\]|[a-zA-Z0-9 ~#$%^&*\-+\/`<>=_|";:{}\[\]()@!?.,])*\' {
                    #ifdef FLEX_DEBUG
                        printf("found a string literal: %s\n", yytext);
                    #endif
                    yylval.stringLiteral = strdup(yytext);
                    shouldInsert = 1;
                    return tSTRINGLITERAL;
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
int             { shouldInsert = 1; yylval.stringLiteral = strdup(yytext); return tINT; }
float64         { shouldInsert = 1; yylval.stringLiteral = strdup(yytext); return tFLOAT; }
rune            { shouldInsert = 1; yylval.stringLiteral = strdup(yytext); return tRUNE; }
string          { shouldInsert = 1; yylval.stringLiteral = strdup(yytext); return tSTRING; }
*/

/*
int             { shouldInsert = 1; return tINT; }
float64         { shouldInsert = 1; return tFLOAT; }
bool            { shouldInsert = 1; return tBOOL; }
rune            { shouldInsert = 1; return tRUNE; }
string          { shouldInsert = 1; return tSTRING; }
true            { shouldInsert = 1; yylval.boolliteral = 1; return tBOOLLITERAL; }
false           { shouldInsert = 1; yylval.boolliteral = 0; return tBOOLLITERAL; }
*/
