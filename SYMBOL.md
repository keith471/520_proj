# Symbol table notes

## The Symbol Table
- symbol table structure

    identifier      kind      type      ref

- identifier = the id
- kind
    - type
    - parameter
    - function
    - package
    - variable
    - field
- depending on the kind, the symbol might also have a type
    - the following symbols should have types
        - type
        - parameter
        - variable
        - field
    - type should be one of
        - primitive types
            - int
            - float64
            - rune
            - bool
            - string
        - complex types
            - array
            - slice
            - struct ("FIELD")
        - if not any of these (e.g. something has type "cool"), then we'd better check for a type declaration that defined cool as another type
            - i.e. we need to search the symbol table for an entry for the (example) form
                cool    typeKind    int     ref_to_type_declaration_where_cool_was_declared
- ref is a reference to the node in the tree corresponding to where the symbol first came up
- right away, before starting any tree traversal, we need to add defaults to the topmost symbol table
    - int
        int     typeKind    reference to TYPE struct of kind intK ==> it's time to update TYPE in tree.h!   NULL
    - float64
        float64     typeKind    reference to TYPE of kind floatK    NULL
    - rune
        rune    typeKind    reference to TYPE of kind runeK     NULL
    - bool
        bool    typeKind    reference to TYPE of kind boolK     NULL
    - string
        string  typeKind    reference to TYPE of kind stringK   NULL
    - true
        true    varKind     reference to TYPE of kind boolK     NULL
    - false
        false   varKind     reference to TYPE of kind boolK     NULL   

## Things we need to use the symbol table to ensure
- ALL identifiers must be declared before use (this is a single pass compiler)
    - error message
    ```
    undefined: name_of_identifier
    ```
- identifiers may not be redeclared
    - error message
    ```
    x redeclared in this block
    	previous declaration at lineno
    ```
    - only exception: a short variable declaration may redeclare variables provided
        - they were originally declared earlier in the same block (or the parameter lists if the block is the function body) with the same type,
        - and at least one of the **non-blank** variables is new.
        - if either of these conditions is not adhered to, then the error is
        ```
        no new variables on left side of :=
        ```
    - As a consequence, redeclaration can only appear in a multi-variable short declaration. Redeclaration does not introduce a new variable; it just assigns a new value to the original. (this last bit can be handled in a later phase - for the purposes of the symbol table, it just means we add no new symbol)
- to validate field access!
- to validate function existence
    - something nice: go does not allow two functions to have the same name, even if their args are different!
        - so if you notice a new function with the same name as an existing one, simply throw an error :)
        ```
        func_name redeclared in this block
        ```

## TODO
- try to get this compiling!
    - after that is done, uncomment the symbol stuff in main and add the files to the Makefile
    - try running and fix errors!
- in lastSelector in EXP, the last selector should not have to be an id! It could be an EXP! Make this modification
to the ast and everything it affects
- need to figure out where to save the symbols in the tree...
    - I don't see that this needs doing yet. Let's hold off on it

## Questions
- where do we check if there are too many or too few args passed in a function call?
- do we need to support mutual recursion?
    - no, this is single pass

## Notes from reading through docs
- a blank identifier does not introduce a binding and is thus not declared (don't add it to symbol table)
- Each type T has an underlying type: If T is one of the predeclared boolean, numeric, or string types, or a type literal, the corresponding underlying type is T itself.
- the size of an array must evaluate to type int --> nothing else is valid (no aliases to int)
