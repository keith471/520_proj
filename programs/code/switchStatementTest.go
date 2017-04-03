package main

func main() {

    switch x := 2; x {
    case 1:
        println("x is 1")
    case 2:
        println("x is 2")
    default:
        println("x is something else")
    }

    var y string = "konichiwa"
    switch y {
    case "hello":
        println("hello")
    case "hola":
        println("hola")
    default:
        println(y)
    }

    var x int = 10
    switch {
    case x < 10:
        x = x*10
    case x > 10:
        x = x/10
    case x == 10:
        x = x^10
    }
    println(x)

    var z = 4
    switch z {
    case 1, 2, 3, 4:
        println("z is 1, 2, 3, or 4")
    }

    switch {

    }
}
