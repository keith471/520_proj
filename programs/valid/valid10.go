//valid10.go- Square root function

/* ------------------- */
package main

func sqrt(x int) float {
	var q float
	z := 1.0

	for iter := 10; iter > 0; iter-- {
		q = x / z
		z = 0.5 * (z + q)
	}
	return z
}
