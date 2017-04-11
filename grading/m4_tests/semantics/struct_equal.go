/*OUTPUT:
true
false
false
true
*/

package main

type my struct{
  a int;
}

func main(){
  var m1, m2 my
  m1.a = 1;
  m2.a = 1;
  println(m1==m2);
  println(m1!=m2);
  m2.a = 2;
  println(m1==m2);
  println(m1!=m2);
}
