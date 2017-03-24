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

    // three-part for loops

	for ;; {
        print("this will print forever!")
        break;
	}

	// only an init statement
	for a := 0; ; {
	}

	// this is like a while loop
    var a int = 0
	for ; a < 10; {
        a++
	}

	// only a post statement
	for ; ; a++ {
        break;
	}

	// just init and expression
	for a := 0; a < 10; {
        a++
	}

	// just init and post
	for a := 10; ; a++ {
        break;
	}

	// just an exp and post
	for ; a < 10; a++ {

	}

	// everything!
    println("countdown")
	for a := 10; a > 0; a-- {
        println(a)
	}
    println("blast off!")
}
