package main

func isPrime(n int) bool {
    for i := 2; i < n; i++ {
        if n % i == 0 {
            return false
        }
    }
    return true
}

func printPrimes(n int) {
    for i := 2; i <= n; i++ {
        if isPrime(i) {
            println(i)
        }
    }
}

func main() {
	printPrimes(300000)
}
