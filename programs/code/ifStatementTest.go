// tests code generation of if statements

package main

func main() {
    // test a basic if statement
    if true {
        println("this code will always run!")
    }

    // now one with an init statement
    if x := "this will also always run"; true {
        println(x)
    }

    // now if/else
    if true {
        println("this will run")
    } else {
        println("this will not")
    }

    // if/else if
    var y = 0
    if x := "only the else will run here"; y > 0 {
        println("y is less than zero")
        y--
    } else if y < 0 {
        println("y is greater than zero")
        y++
    } else {
        println("y is equal to zero")
        y = y*10
    }
}
