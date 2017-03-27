package main

func cast_int() {
	type num int
	var x int = 5
	var y num = num(x)
	x = int(y)
}

func cast_float64() {
	type num float64
	var x float64 = 5.0
	var y num = num(x)
	x = float64(y)
}

func cast_rune() {
	type num rune
	var x rune = 'z'
	var y num = num(x)
	x = rune(y)
}

func cast_bool() {
	type num bool
	var x bool = true
	var y num = num(x)
	x = bool(y)
}

func main() {

}
