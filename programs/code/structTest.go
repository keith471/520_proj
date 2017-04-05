package main

type Cat struct {
    name string
    color string
    owner struct {
        name string
        address string
    }
}

func printName(c Cat) {
    println("The name of this cat is: " + c.name)
}

func main() {
    var yoda Cat
    yoda.name = "yoda"
    yoda.color = "white"
    yoda.owner.name = "I have no owner! I am the king!"
    yoda.owner.address = "1840 Cattle Drive"

    var other Cat
    other.name = "felix"
    other.color = "black"
    other.owner.name = "john"
    other.owner.address = "3521 Streety Street"

    printName(yoda)
    printName(other)

    if (yoda == other) {
        println("Really?!")
    } else {
        println("Of course yoda is much cooler")
    }
}
