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
#    println(x) # not in scope.
}

fn local_test() {
    x := 42
    LOCAL_FN_2 := fn() {
        x := 99
        println(x)
        println("local2")
    }
    fn local_fn_1() {
        x := 123
        println(x)
        println("local1")
    }
    LOCAL_FN_3 := fn() {
        x := -1
        println(x)
        println("local3")
    }
    println(x)
    local_fn_1()
    LOCAL_FN_2()
    LOCAL_FN_3()
    println(x)
}

foo(false)

local_test()
