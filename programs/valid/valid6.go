//valid6.go - type Casting in GO

package main

type num int

func main() {
    var x int
    var y, z float64
	y = float64(x)
	x = int(y)
	x = num(z)
}
