/*OUTPUT :
0
+0.000000e+000

0
0
0
*/

package main

type my struct{
  a int;
}

func main(){
  var a int
  var b float64
  var c string
  var d rune
  var x [3]int
  var y my
  println(a);
  println(b);
  println(c);
  println(d);
  println(x[0]);
  println(y.a);
}
