package main;

type Person struct {
    name string
    age int;
    address struct {
        streetNo int
    }
}

type name struct {
    first, last string
}

func test() {
    var keith Person;
    keith.name = "keith"
    keith.age = 23;
    keith.address.streetNo = 3570;
    fmt.Printf("%d\n", keith.address.streetNo)
    fmt.Printf(keith.name);
    type cool struct {
        beans struct {}
    }
}

func test2(name struct {first, last string}) struct {first, last string} {
    fmt.Printf(name.first);
    fmt.Printf(name.last);
    return name
}

func main() {
    test();
    var keithName name
    keithName.first = "keith"
    keithName.last = "strickling"
    test2(keithName);
    var y int = 2
    if x := 3; x < y {
        fmt.Printf("in first if\n");
    } else if z := 1; z < y {
        fmt.Printf("in second if\n");
    } else {
        fmt.Printf("in else\n");
    }
    var a, b int
    a+b
}
