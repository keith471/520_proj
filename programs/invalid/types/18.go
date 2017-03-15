package main

func test(a, b, c int) int {
    return a+b+c
}

func main() {
    // too few parameters to function call
    var x int = test(1, 2)
}
