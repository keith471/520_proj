package main

type Person struct {
    name string
    age int;
    address struct {
        streetNo int
    }
}

var keith Person

type name struct {
    first, last string
}

func test() {
    var keith Person
    keith.name = "keith"
    keith.age = 23
    keith.address.streetNo = 3570;
    println(keith.address.streetNo)
    println(keith.name);
    type cool struct {
        beans struct {}
    }
}

func test2(name struct {first, last string}) struct {first, last string} {
    print(name.first);
    println(name.last);
    return name
}

func testEverything(arg int) int {
    switch j := "cool"; arg {
    case 3:
        print("arg is 3");
        break;
    case 4,5:
        print("arg is 4 or 5");
    default:
        print("arg is default");
    }

    /* this is a multiline comment
fdjskfdjsfkjdsf
    fdjskfdjskfjdskfjdskjf
    */

    for i := 0; i < 10; i++ {
        println(i);
    }

    for i < 10 {
        println(i);
        i--;
    }

    for {
        print("doing this forever!");
        continue;
    }

    if (true) {

    }

    var splice []int;
    splice = append(splice, 1);
    splice = append(splice, 2);

    var array [3][6]int;
    array[0][0] = 44;
    array[1] = 21;

    return 3;
}

func main() {
    test();
    var keithName name
    keithName.first = "keith"
    keithName.last = "strickling"
    test2(keithName);
    var y int = 2
    if x := 3; x < y {
        print("in first if\n");
    } else if z := 1; z < y {
        print("in second if\n");
    } else {
        print("in else\n");
    }
    if blah {
    } else {
        print(yeah);
    }
    var a, b int
    for {}
}
