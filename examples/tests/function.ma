foo :: fn() {
    println("hello world")
}
foo()

fn() {
    println("IIFE!")
}()

# bound_func := 111

fn bound_func(a: int) -> double {
    return a * -1.0
}

fn bound_func(a: double) {
    println(a)
}

fn bound_func(a: double, b: string) {
    println(a)
    println(b)
}

bound_func(bound_func(41), "lol it werkz")
