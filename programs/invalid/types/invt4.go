package main

func main() {
    type Person struct {
        name string
    }
    type SuperHero struct {
        name string
    }
    // the underling struct types are the same, but the types are still different
    var x Person
    var y SuperHero = x
}
