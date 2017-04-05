package main

func main() {

    var array [2]int
    array[0] = 1
    array[1] = 2

    var matrix [3][3]int

    var row int = 0
    var col int = 0

    for ; row < 3; row++ {
        for col = 0; col < 3; col++ {
            print(matrix[row][col], " ")
        }
        println()
    }

    var m [3][3]int

    if (matrix == m) {
        println("equal matrices")
    } else {
        println("unequal matrices")
    }

}
