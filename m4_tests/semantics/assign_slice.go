/*OUTPUT :
1
1
2
2
*/
package main

func main(){
  var m1, m2 []int
  m1 = append(m1, 1)
  m2 = m1;
  println(m1[0]);
  println(m2[0]);
  m2[0] = 2;
  println(m1[0]);
  println(m2[0]);
}
