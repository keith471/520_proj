/*OUTPUT:
1
1
*/

package main

type my struct{
  a int;
}

func f(m my){
  m.a = 2;
}

func main(){
  var m1 my
  m1.a = 1;
  println(m1.a);
  f(m1)
  println(m1.a);
}
