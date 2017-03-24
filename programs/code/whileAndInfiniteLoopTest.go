// test code generation of while and infinite loops

package main

func main() {
    // a while loop
    var x int = 10
    for x > 0 {
        println(x)
        x--
    }

    // an infinite loop
    println("forever ")
    i := 0
    for {
        if i == 1000 {
            break;
        }
        println("and ever ")
        i++
    }
}
