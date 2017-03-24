package main

type hello struct {
    r int
}

type cool struct {
    x, y int
    t struct {
        nice string
        v struct {
            x hello
        }
    }
}

func main() {
    type beans int
    var x int
    var y beans

    y = beans(x)
}
