package main

func main() {
    var x [10]int
    x[0] = 10
    var y [10]int

    y, z := x, 3

    println(y[0])
}
