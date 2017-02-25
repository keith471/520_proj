//valid5.go - GCD

package main

func main(){
	var x,y = 50,18
	print("the gcd of 50 and 18 is:")
	print(gcd(x,y))
}

//function to calculate gcd of two numbers
func gcd(a, b int) int {
	c:=1;
	for a!= 0 {
		c = a
		a = b%a
		b = c
	}
	return b
}