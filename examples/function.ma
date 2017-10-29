foo :: fn() {
    println("hello world")
}
foo()

fn() {
    println("IIFE!")
}()

fn bound_func(a: int) -> double {
    return a * -1.0
}

fn bound_func(a: double) {
    println(a)
}

fn bound_func(a: double, b: buffer) {
    println(a)
    println(b)
}

bound_func(bound_func(42), "lol it werkz")
