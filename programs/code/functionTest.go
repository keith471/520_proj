// tests code generation of functions

package main

// print a function with no arguments and no return type
func f() {

}

// a function with arguments
func g(a int, b float64, c, d struct { x int }) {

}

// a function with a return
func h() float64 {
    return 9.4
}

// a function with arguments and a return
func i(a, b, c, d int, e string) string {
    return "it works!"
}

// check that main is generated with a return of type int!
func main() {
    f()

    var c, d struct { x int }

    g(1, 2., c, d)

    var float = h()
    println(float)

    var s = i(1, 2, 3, 4, "test")
    println(s)
}
