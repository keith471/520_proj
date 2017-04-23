package main

func merge(array, temp []int, lo, hi int) {
    mid := (lo+hi)/2
    temp_index := lo
    index_left := lo
    index_right := mid+1

    for index_left <= mid && index_right <= hi {
        if (array[index_left] < array[index_right]) {
            temp[temp_index] = array[index_left]
            index_left++
        } else {
            temp[temp_index] = array[index_right]
            index_right++
        }

        temp_index++
    }

    for index_right <= hi {
        temp[temp_index] = array[index_right]
        index_right++
        temp_index++
    }

    for index_left <= mid {
        temp[temp_index] = array[index_left]
        index_left++
        temp_index++
    }

    for i := lo; i <= hi; i++ {
        array[i] = temp[i]
    }

}

func merge_sort_helper(array, temp []int, lo, hi int) {
    if (lo >= hi) {
        return
    }
    
    mid := (lo+hi)/2
    merge_sort_helper(array, temp, lo, mid)
    merge_sort_helper(array, temp, mid+1, hi)

    merge(array, temp, lo, hi)
}

func merge_sort(array []int, length int) {
    var temp []int
    for i := 0; i < length; i++ {
        temp = append(temp, array[i])
    }
    merge_sort_helper(array, temp, 0, length-1)
}

func main() {
    // Create the array to sort
    var array []int
    length := 50000000
    for i := 0; i < length; i++ {
        array = append(array, length-i);
    }

    // Sort the array
    merge_sort(array, length)

    // Print the sorted array
    for i := 0; i < 10; i++ {
        print(array[i], ",")
    }
    println("\nDone!")
}
