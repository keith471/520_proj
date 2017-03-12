package main

// type alias
type t1 int

// distributive type aliases
type (
	t2 rune
	t3 bool
)

// empty type aliases
type ( )

// struct type
type t4 struct {
	x float64
	y, z float64
}

// nested struct
type t5 struct {
	k float64
	n struct {
		a int
	}
}

// empty struct
type t6 struct {
}

// slices
type t7 []int			// base type
type t8 []point			// type id
type t9 []struct {		// composite type
	a int
}
type t10 [][]float64		// matrix

// arrays
type t11 [4]int			// base type
type t12 [4]point		// type id
type t13 [8]struct {		// composite type
	a int
}
type t13 [3][3]float64	// matrix
