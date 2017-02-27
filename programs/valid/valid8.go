//valid8.go -  Check if an array is a heap

package main

func isHeap(arr []int) bool {
	length := len(arr)
	for i := 0; i < length; i++ {
		if 2*i+1 < length && arr[2*i+1] < arr[i] {
			return false
		}
		if 2*i+2 < length && arr[2*i+2] < arr[i] {
			return false
		}
	}
	return true
}
