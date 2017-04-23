package main

var size = 50000
var arr [50000]int

func insertionSort() {
	for j := 1; j < size; j++ {
		key := arr[j]
		i := j - 1
		for i >= 0 && arr[i] > key {
			arr[i+1] = arr[i]
			i = i - 1
		}

		arr[i+1] = key
	}
}

func main() {
	for j := size - 1; j >= 1; j-- {
		arr[size - j - 1] = j
	}
	insertionSort()
	println("Sorted. Printing the first 10 sorted numbers")
	for i := 0; i < 10; i++ {
		println(arr[i])
	}

}
