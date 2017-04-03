// arrays and structs should end up as type defs!!

package main

// this wil not end up in the C++ code
type num int

// we'll have a C++ struct for this
type Person struct {
    name string
    age num
}

func main() {

    // we'll have a C++ struct for this
    type SuperHero struct {
        fakeIdentity Person
        name string
        power string
    }

    // we'll have a C++ array of these
    var superheroes [10]SuperHero

    var keith Person
    keith.name = "keith"
    keith.age = num(24)

    var uberMan SuperHero
    uberMan.fakeIdentity = keith
    uberMan.name = "Uber Man"
    uberMan.power = "teleportation"

    println(uberMan.fakeIdentity.name, uberMan.fakeIdentity.age)
    println(uberMan.name, uberMan.power)
};