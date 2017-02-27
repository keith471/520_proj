//invalid32.go - Array size declaration is after the type, it should be before instead

package main

func main() {
	var x string[10] // Error here
	x[0] = "Hello"
	x[1] = "Shruti"
}