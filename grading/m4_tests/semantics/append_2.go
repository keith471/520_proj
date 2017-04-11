//OUTPUT : panic: runtime error: index out of range

package main;

func main(){
  var a, b []int;
  b = append(a,1);

  println(b[1]);
}
