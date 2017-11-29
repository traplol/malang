fn fib(n: int) -> int {
    return if n < 3 1 else recurse(n-1) + recurse(n-2)
}
println(fib(30))
