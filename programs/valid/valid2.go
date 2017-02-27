//valid2.go - Bubble Sort
package main


func bubble_sort(x []int) []int {
	l := len(x)
	for i := 0; i < l; i++ {
		for j := i; j < l; j++ {
			if x[i] > x[j] {
				x[i], x[j] = x[j], x[i]
			}
		}
	}
	return x
}
