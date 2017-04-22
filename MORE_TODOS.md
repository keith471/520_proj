
- support passing arrays by value (pass_array)
- fix issue with naming (scope_2)
    - during symbol table generation, generate a new name for every variable

var x = 98
{
    var x int = x
}

print true, false, runes, passing slices
