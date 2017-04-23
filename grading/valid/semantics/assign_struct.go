/*OUTPUT :
1
1
1
2
*/
package main

type my struct{
  a int;
}

func main(){
  var m1, m2 my
  m1.a = 1;
  m2 = m1;
  println(m1.a);
  println(m2.a);
  m2.a = 2;
  println(m1.a);
  println(m2.a);
}
