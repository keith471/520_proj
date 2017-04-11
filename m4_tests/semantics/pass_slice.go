/*OUTPUT:
1
2
*/

package main;

func f(a []int){
  a[0] = 2;
}

func main(){
  var a []int;
  a = append(a,1);
  
  println(a[0]);
  f(a);
  println(a[0]);
}
