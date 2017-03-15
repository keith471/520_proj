package main

func test() int {
    // cannot return string from int function
    return "hello"
}

func main() {
    var x int = test()
}
