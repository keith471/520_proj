//invalid5.go - invalid variable declaration 

package main
func f(){
	var x = 10;
	type cute struct{
		p, q string
		a, b int
	}
	var z []cute;
	var w [20]cute;
	"10001" &^= 12 // invalid here

	return "not working!!"
}
