/*OUTPUT:
2
2
*/
package main

func main(){
  var a int;
  a = 2
  println(a);
  switch a {
    case 1, 2:
      println(a);
    default:
      println("default");
  }
}
