fn foo(a: bool) {
    println(a)
    b := 123
    if a {
        x := 42
        println(x)
        println(b)
    }
    else {
        x := 99
        println(x)
        println(b)
    }
    println(b)
#    println(x)
}

foo(false)
