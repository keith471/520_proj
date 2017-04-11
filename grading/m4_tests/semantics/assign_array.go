/*OUTPUT :
1
1
1
2
*/
package main

func main(){
  var m1, m2 [3]int
  m1[0] = 1
  m2 = m1;
  println(m1[0]);
  println(m2[0]);
  m2[0] = 2;
  println(m1[0]);
  println(m2[0]);
}
