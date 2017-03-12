# TODOS

https://github.com/Sable/comp520-2017-15

Reference compiler:
ssh kstric@teaching.cs.mcgill.ca
~cs520/golitec

## Milestone 2
- get everything working
- write test programs

## Later
- check that the array size evaluates to a positive integer
    i.e. `var y [-(-43)]int` and `var y [^-43]int` are acceptable

## Pretty printer
- fix pretty printing of structs

## Weeding
- make sure that array type declarations use int literals or expressions of all int literals (no identifiers) for sizes
- test a string of expressions followed by an identifier `(2+3).6.apple`

## Symbol table


## Type Checking
- when can we know whether something is a cast? probably in this phase - if we check an expression and we find it has kind argumentsK, then we have to check the symbols associated with it and their kinds. The leading symbol will have kind functionDeclSym if the expression is really a function call and typeDeclSym or typeSym if it is actually a cast
- decide where symbols need to be stored in the ast!
- make sure in append that the first argument to append is a slice expression to append to, and the second expression
is the element to add (though this probably should happen in the type phase)
- casts will (likely) appear as function calls in your AST. It will be important in a later
phase of the compiler to convert them to the proper cast nodes.
- also pertaining to casts, the weeder only accepts function calls and receive operations as expression statements. However, the parser parser casts as function calls so the weeder will not catch a cast used as a statement. Thus, as soon as we know a function call is actually a cast, we need to check that it is not being used as an expression statement. If it is, we throw an error.
- a short var declaration may redeclare variables but only if they are of the same type as what they were originally declared as!
    - the symbol table marks any such variables as redeclared, and the type they were declared with? (what if they weren't declared with a type!) and then the type checker ensures that they are assigned the same type in the short var decl as that they were originally declared with
- in type checking phase, we'll need to check that the type an array size is casted to is an int
    - e.g. the following is acceptable
        `type cool int
        var x [cool(5)]int`
    - but this isn't
        `var x [float64(5)]`
- the size of an array must evaluate to type int --> nothing else is valid (no aliases to int)
    - this conflicts with the previous point
- make sure that functions with a return type return something (of the correct type) in every code path


## Sources
http://stackoverflow.com/questions/10826744/semicolon-insertion-ala-google-go-with-flex
