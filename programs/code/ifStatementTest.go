// tests code generation of if statements

package main

func main() {
    // test a basic if statement
    if true {
        print("this code will always run!")
    }

    // now one with an init statement
    if x := "this will also always run"; true {
        print(x)
    }

    // now if/else
    if true {
        print("this will run")
    } else {
        print("this will not")
    }

    // if/else if
    var y = 0
    if x := "only the else will run here"; y > 0 {
        print("y is less than zero")
        y--
    } else if y < 0 {
        print("y is greater than zero")
        y++
    } else {
        print("y is equal to zero")
        y = y*10
    }
}
