package main

var struct_1 struct {
    _ int
    r int
}

var struct_2 struct {
    _ int
    r int
}

func main() {

    struct_1.r = 3

    var array_1 [10]int

    if (struct_1 == struct_2) {
     println("equal!");
    } else {
     println("not equal!");
    }

}
