package main

type cool int

var x cool

func main() {

	type cool int

	var y cool

    // x and y have the same type names and underlying types, yet
    // their types were defined in different typespecs and thus are different,
    // making this assignment invalid
	x = y

}
