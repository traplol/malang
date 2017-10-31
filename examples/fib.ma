fib :: fn(n: int) -> int {
    return if n < 3 1 else fib(n-1) + fib(n-2)
}
println(fib(10))
