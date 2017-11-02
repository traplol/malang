println(fn (n: int) -> int {
    println(n)
    return if n < 3 1 else recurse(n-1) + recurse(n-2)
}(10))
