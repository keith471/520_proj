/*OUTPUT :
true
false
false
true
*/

package main

func main(){
  var a [3]int;
  var b [3]int;
  a[0] = 1;
  a[1] = 1;
  a[2] = 1;
  b[0] = 1;
  b[1] = 1;
  b[2] = 1;
  println(a==b);
  println(a!=b);
  b[2] = 2;
  println(a==b);
  println(a!=b);

}
