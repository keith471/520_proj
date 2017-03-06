package main

func unary_exprs() {
	var x int
	var y int
	// single
	y = -x
	y = +x
	y = !x
	y = ^x

	// multiple
	y = - -x
	y = + +x
	y = !!x
	y = ^^x
} 
