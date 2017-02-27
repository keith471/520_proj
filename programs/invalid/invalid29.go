//invalid29.go - multiple return from function

package main

//not allowed
func f(a int, b string) (string,int){
    return a,b
}

func main()
{
    f(1,"hello")
}