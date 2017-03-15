package main

func main() {

    var x struct {
        _ y int
        z int
    }

    var y struct {
        z int
    }

    // invalid because the types are not the same (due to blank identifier)
    x = y

}
