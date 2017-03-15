package cool

func main() {
    // this is all fine
    var x int
    if x := 1; x < 2 {
       var x = 5
    } else if x > 2{
        x = 3
    }

    // redeclaration of x
    var x int
}
