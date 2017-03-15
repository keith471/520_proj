package test

func main() {
    var x struct {
        i int
        t float64
    }
    var y struct {
        i int
        t float64
    }
    // structs are identical so assignment is valid
    x = y
}
