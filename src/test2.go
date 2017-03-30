package main

var x struct {
    _ int
    r int
}

var y struct {
    _ int
    r int
}

func main() {

    x.r = 3

    if (x == y) {
     println("equal!");
    } else {
     println("not equal!");
    }

}
