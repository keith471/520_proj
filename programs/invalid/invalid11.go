//invalid11.go - invalid function declaration

package main
func f(a int, b int, c string, d int) {
	return
}
func f() {}

func f(a, b int, c int, ...) string {
	return c
}
