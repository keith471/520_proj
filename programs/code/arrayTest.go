package main

func main() {

    var array [0]int
    array[0] = 1

    var matrix [0][0]int

    var row int = 0
    var col int = 0

    for ; row < 3; row++ {
        for col = 0; col < 3; col++ {
            print(matrix[row][col], " ")
        }
        println()
    }
}
