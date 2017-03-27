package main

func assign_stmts() {
	var a, b int
	// basic assignment
	a = 0
	// multiple assignment
	a, b = 0, 1

	// parenthesized assignment
	(a) = 0
	(a), (b) = 0, 1

	// op assign
	a += 1
	a -= 1
	a *= 1
	a /= 1
	a %= 1
	a &= 1
	a |= 1
	a ^= 1
	a &^= 1
	a <<= 1
	a >>= 1
} 
