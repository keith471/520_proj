/*OUTPUT:
1
*/
package main;

func f(a struct { golite_5 int; }){
  println(a.golite_5);
}

func main(){
  var a struct { golite_5 int; }
  a.golite_5 = 1
  f(a)
}
