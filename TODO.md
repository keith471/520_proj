# TODOS

https://github.com/Sable/comp520-2017-15

Reference compiler:
ssh kstric@teaching.cs.mcgill.ca
~cs520/golitec

## Pretty printer
- fix pretty printing of structs

## Weeding
- test a string of expressions followed by an identifier `(2+3).6.apple`
    - can fix this by checking that each selector exp is either identifierK, selectorK, or indexK

## Symbol table


## Type Checking


## Codegen
- finish cppTypeFUNCTIONDECLARATION
- finish cppTypeTYPEDECLARATION
- make a pass where we collect arrays and structs and make typedefs for them
- any node where we encounter these arrays and structs, we will need to update the node
with the new representative C++ array or struct type that we've made for it
    ==> I think it would be very helpful to give each (TYPE) node a C++ type based on this traversal!
    Rules:
        - if the GoLite type either is or resolves to a base type, then the C++ type is the base type
        - if the GoLite type either is or resolves to a slice type, then the C++ type is a vector<T> where T
        is the type that the slice element types resolve to
        - any time we see a struct or array (either a literal or a named type) we need to compare it with the structs and arrays that we've already encountered and saved as C++ types. If we find a matching one, then the C++ type of the current struct or array is the C++ type that we found.


        - for structs and arrays, we can have either named structs or arrays (declared as type name struct {...} etc.) or unnamed literals (var x struct {...} etc.)
            - we can create a linked list (top is the first struct or array we encounter) of C++ typedefs (if unnamed, then we give it a name, else it has the name given to it in GoLite)
            - if the GoLite type is an idT that resolves to an array, then we need to look in the hashmap for the id and get the associated C++ array
                - if we can't find it then we create a new one and add it to the hashmap
            - if the GoLite type is an array literal, then we ad

- genVARDECLARATION
- genTYPEDECLARATION
- genFUNCTIONDECLARATION
- finish genTYPE ==> idK, structK, arrayK
- finish genDefault
- write test programs and report!

- a c++ vector is just like a slice!!!

## Sources
http://stackoverflow.com/questions/10826744/semicolon-insertion-ala-google-go-with-flex
