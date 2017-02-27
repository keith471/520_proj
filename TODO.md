# TODOS

https://github.com/Sable/comp520-2017-04

## Now

## Later

## Scanner

## Parser

## AST
- do we need to record the signature as a string in FUNCTIONDECLARATION?

## Pretty printing

## Weeding
- make sure assignment of list to list consists of lists of equal length, and that the left list consists of all lvalues
- for switch statements, make sure there is only one default case
- make sure that break and continue are only ever used within for loops
- make sure expStatement is just a function call or receive operation

## Symbol table
- redeclaration: Unlike regular variable declarations, a short variable declaration may redeclare variables provided they were originally declared earlier in the same block (or the parameter lists if the block is the function body) with the same type, and at least one of the non-blank variables is new. As a consequence, redeclaration can only appear in a multi-variable short declaration. Redeclaration does not introduce a new variable; it just assigns a new value to the original.

## Type Checking
- make sure in append that the first argument to append is a slice expression to append to, and the second expression
is the element to add (though this probably should happen in the type phase)
- casts will (likely) appear as function calls in your AST. It will be important in a later
phase of the compiler to convert them to the proper cast nodes.

## Sources
http://stackoverflow.com/questions/10826744/semicolon-insertion-ala-google-go-with-flex
