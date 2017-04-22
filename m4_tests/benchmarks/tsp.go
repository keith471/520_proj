/* Author: L Emery-Fertitta */

/*
 * This program is a brute force TSP solver.
 * At n=12, official Go takes ~17 seconds on my machine.
 */

package main

// s is the solution, i.e. the order to visit each vertex
var s []int

// cost of the current solution
var cost_s int

//  w[i][j] is the weight of edge ij
var w [][]int

// crappy random int
var r int

func rand() int {
	r = (8121*r + 28411) % 134456
	return r
}

func compute_cost(n int, A []int) int {
	var sum = 0
	for i := 0; i < n-1; i++ {
		sum += w[A[i]][A[i+1]]
	}
	return sum + w[n-1][0]
}

func permute(n int, A []int) {

	var index []int
	for i := 0; i < n; i++ {
		index = append(index, 0)
	}

	var temp, swap, cost_i int
	for i := 1; i < n; {
		if index[i] < i {
			swap = i % 2 * index[i]
			temp = A[swap]
			A[swap] = A[i]
			A[i] = temp
			cost_i = compute_cost(n, A)
			if cost_i < cost_s {
				cost_s = cost_i
				for j := 0; j < n-1; j++ {
					s[j] = A[j]
				}
			}
			index[i]++
			i = 1
		} else {
			index[i] = 0
			i++
		}

	}
}

func main() {
	var n = 11
	for i := 0; i < n; i++ {
		s = append(s, i)
		var w_i []int
		for j := 0; j < n; j++ {
			w_i = append(w_i, rand())
		}
		w = append(w, w_i)
	}
	cost_s = compute_cost(n, s)
	println("Ordered path cost: ", cost_s)
	permute(n, s)
	println("Optimal path cost: ", cost_s)
}
