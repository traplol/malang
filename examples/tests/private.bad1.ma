type A = {
    a := 123
    _b := 42
    C := 55
    _D := 10*10

    fn pp() {
        println(a)
        println(_b)
        println(C)
        println(_D)
        _private_func()
    }

    fn _private_func() {
        println("hello world!")
    }
}

a := A()
a._D = 111
