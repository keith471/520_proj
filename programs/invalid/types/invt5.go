package main

type Person struct {
    name string
    age int
}

func main() {
    var x Person

    // no weight field
    x.weight = 180
}
