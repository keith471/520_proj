package main

func f() struct { a int; } {
    var a struct { a int; }
    return a
}

func main() {
    var x [10]int
    x[f().a] = 1
}
