package main

func main() {
    
    var intSlice []int

    intSlice = append(intSlice, 0)
    intSlice = append(intSlice, 1)
    intSlice = append(intSlice, 2)

    for i := 0; i < 3; i++ {
        println(intSlice[i])
    }
}
