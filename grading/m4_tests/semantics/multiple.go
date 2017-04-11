/*OUTPUT
1
2
4
5
6
7
8
*/

package main;

func main(){
  var a,b int = 1, 2;
  println(a);
  println(b);
  a, a, b = 3, 4, 5;
  println(a);
  println(b);
  a, b, c := 6, 7, 8;
  println(a);
  println(b);
  println(c);
}
