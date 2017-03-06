# TODOS

https://github.com/Sable/comp520-2017-04

Reference compiler:
ssh kstric@teaching.cs.mcgill.ca
~cs520/golitec

## Now

## Later

## Scanner

## Parser

## AST
- do we need to record the signature as a string in FUNCTIONDECLARATION?

## Pretty printing

## Weeding
- continue with regAssignK and below (line 113 in weed.c), making sure that _ is dealt with properly
- then finish writing weedTYPEDECLARATION

- blank identifiers:
- I would take a look at the areas we disallow in the invalid tests, that might be easier than scanning the specs (although I do recommend taking a brief look).
From an intuition pov, it is disallowed in any place which needs to be evaluated - RHS of assignments, if/for conditions, etc.
- My takeaway was primary 'only on the LHS of declarations and assignments'.

## Symbol table
- write symbol.c
- somehow deal with TYPE
- copy functions over to symbol.h
- update tree.h and tree.c
- update main.c
- update Makefile
- redeclaration: Unlike regular variable declarations, a short variable declaration may redeclare variables provided they were originally declared earlier in the same block (or the parameter lists if the block is the function body) with the same type, and at least one of the non-blank variables is new. As a consequence, redeclaration can only appear in a multi-variable short declaration. Redeclaration does not introduce a new variable; it just assigns a new value to the original.
    - handle preexisting symbols in putSymbol

## Type Checking
- make sure in append that the first argument to append is a slice expression to append to, and the second expression
is the element to add (though this probably should happen in the type phase)
- casts will (likely) appear as function calls in your AST. It will be important in a later
phase of the compiler to convert them to the proper cast nodes.
- also pertaining to casts, the weeder only accepts function calls and receive operations as expression statements. However, the parser parser casts as function calls so the weeder will not catch a cast used as a statement. Thus, as soon as we know a function call is actually a cast, we need to check that it is not being used as an expression statement. If it is, we throw an error.

## Sources
http://stackoverflow.com/questions/10826744/semicolon-insertion-ala-google-go-with-flex
