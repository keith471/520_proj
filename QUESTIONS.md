
IMPORTANT TO ASK

- lvalues vs exps vs identifiers --> this should solve reduce-reduce conflicts
- do we need to support slices and array accesses as expressions? probably!
    - as a follow-up, how can we support multi-dimensional arrays and slices?


OTHERS

- can we have a look at my reduce-reduce conflicts?
    - can probably solve by replacing every idList with an lValueList and then using weeding to ensure we only
    have ids where we should have an idList

- how should we treat the tokens "true" and "false"?
    - treat them as identifiers for now. They will be treated later
- do we have to support methods? As in methods for our own types?
    - can structs have functions?
        - e.g. can a function call be like person.getName()?
        - NO!
- do we have to support multi-dimensional arrays?
    - YES!
- print() --> do nothing
- println() --> just print a new line and that's it


- can function calls and receives be parenthesized more than once?
- where in the world can we actually have structs?
- how do we deal with hex and oct ints? This will determine how I define my scanner and parser
