// Test code generation of variable declarations

package main

// try a variable declaration out here
var x int

// try two out here
var y, z float64

func main() {
    // try one in here
    var x bool

    // and two more
    var y, z rune

    var (
        m rune
        n string = "will this work?"
    )

    // try one with no declared type
    var t = 12

    // and one with type and exp
    var s int = 33

    // try a short variable declaration
    a := "hello"

    // try a short variable declaration with a redeclaration
    a, b := "hi", 9.6

}
