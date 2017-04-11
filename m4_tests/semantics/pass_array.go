/*OUTPUT:
1
1
*/

package main;

func f(a [2]int){
  a[1] = 2;
}

func main(){
  var a [2]int;
  a[0] = 1; 
  a[1] = 1; 
  println(a[1]);
  f(a);
  println(a[1]);
}
