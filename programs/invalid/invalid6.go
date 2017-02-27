//invalid6.go - keyword used as variable name 
package main

func main() {
	var println string = "Dont use me, I am keyword"
	println(println)
}