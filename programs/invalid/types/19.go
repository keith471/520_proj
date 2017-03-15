package main

func test(a, b, c int) int {
    return a+b+c
}

func main() {
    // cannot assign int return to float64
    var x float64 = test(1, 2, 3)
}
