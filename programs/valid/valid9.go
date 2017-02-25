//valid9.go - Case Statements

package main

var answer string
type token struct{
	name string
	
} 

func main(){
 	
	var t1 token
	t1.name="a"
	var t2 token
	t2.name="b"
	var t3 token
	t3.name="c"
	var t4 token
	t4.name="d"
	
	answer=switchCase(t1)
	print(answer)
	
}
	
		
	


func switchCase(t1 token) string{
	switch t1.name {	
	default : return "Nothing received"
	case "a" : return "Concatenate"
	case "b" : return "Break"
	case "c" : return "Continue"
	case "d" : return "Delete"
	}
	
}