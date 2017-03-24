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


func main() {

}
