/*OUTPUT:
true
b
*/
package main

func main(){
  var a,b bool;
  a = true;
  b = true;
  println(a);
  switch a:=false; {
    case a:
      println("a");
    case b:
      println("b");
    default:
      println("default");
  }
}
