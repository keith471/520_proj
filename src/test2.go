package main

func main() {

    var x []int
    var y []int

    x = append(x, 10)

    y = append(y, 3)

    if (x == y) {
        println("same!");
    } else {
        println("different!");
    }
}
