

## TODO
- typeEXP
    selector
- throw errors in typeVar
- assertEqualTYPEs
- assertValidOpUsage
- write test programs
- write report
- submit

- improve assertion methods to print out alias to (alias to etc)


- when can we know whether something is a cast? probably in this phase - if we check an expression and we find it has kind argumentsK, then we have to check the symbols associated with it and their kinds. The leading symbol will have kind functionDeclSym if the expression is really a function call and typeDeclSym or typeSym if it is actually a cast
- also pertaining to casts, the weeder only accepts function calls and receive operations as expression statements. However, the parser parses casts as function calls so the weeder will not catch a cast used as a statement. Thus, as soon as we know a function call is actually a cast, we need to check that it is not being used as an expression statement. If it is, we throw an error.
- in type checking phase, we'll need to check that the type an array size is casted to is an int
    - e.g. the following is acceptable
        `type cool int
        var x [cool(5)]int`
    - but this isn't
        `var x [float64(5)]`
- the size of an array must evaluate to type int --> nothing else is valid (no aliases to int)
    - this conflicts with the previous point
- make sure that functions with a return type return something (of the correct type) in every code path
