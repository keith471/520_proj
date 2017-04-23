//OUTPUT : panic: runtime error: index out of range

package main;

func f(a []int){
  a = append(a, 2);
}

func main(){
  var a []int;
  a = append(a,1);

  f(a);
  println(a[1]);
}
