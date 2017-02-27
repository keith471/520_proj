//invalid8.go - L.H.S. expression in variable declaration not allowed

package main

var x, y = 0, 1
var x + y [10] int = c  //not allowed 

func main() {
	// nothing
}