package main

func main() {

    var x struct {
        y struct {
            z struct {
                a int
            }
        }
    }

    var y struct {
        a int
    }

    y.a = 10

    // valid
    x.y.z = y

    // no field b
    x.y.z.b = 11
}
