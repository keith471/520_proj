package main

type cool float64

func whatever() struct {x cool;} {
    var x struct {x cool;}
    // no cast of 9.4 to cool
    x.x = 9.4;
    return x
}

func main() {
    whatever()
}
