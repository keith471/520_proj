//invalid36.go - nested type block error
package main;
type (
    x int;
    type decimal float64;  //type in type
);