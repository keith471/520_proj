package main

func f() {
}

func g(l, a, b, c int) {

}

func funccall_exprs() {
	var a, b, c int
	var d [1]int

	f()
	g(0, a, b+c, d[0])

	(f)()
	(g)(0, a, b+c, d[0])
} 
