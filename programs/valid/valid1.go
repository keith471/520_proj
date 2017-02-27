// valid1.go - variable declaration - go lite

package main

func main() {

    var a string = "initial"
    println(a)

    // You can declare multiple variables at once.
    var b, c int = 1, 2
    println(b, c)

    // Go will infer the type of initialized variables.
    var d = true    
    println(d)

    // Variables declared without a corresponding initialization are zero valued.
    var e int
    println(e)

    f, g, h := "short", "medium", "tall"
    println(f)
}
