package main

type test int

var x test

func main() {

    type test float64

    var y test

    // x and y have the same type names, but different underlying types
    // thus the following assignment is invalid
    x = y
}
