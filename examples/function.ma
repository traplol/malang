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

bound_func(bound_func(42))
