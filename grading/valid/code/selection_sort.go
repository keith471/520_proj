package main

// Sorts the array in increasing order
func selection_sort(array []int, length int) {
    for i := 0; i < length; i++ {
        min := array[i]
        min_index := i
        for j := i+1; j < length; j++ {
            if array[j] < min {
                min = array[j]
                min_index = j
            }
        }

        if min_index != i {
            // Swap i and min_index
            array[min_index] = array[i]
            array[i] = min
        }
    }
}

func main() {
    // Create the array to sort
    var array []int
    length := 100000
    for i := 0; i < length; i++ {
        array = append(array, length-i);
    }

    // Sort the array
    selection_sort(array, length)

    // Print the sorted array
    for i := 0; i < 10; i++ {
        print(array[i], ",")
    }

    println("\nDone!")
}
