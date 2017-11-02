fn f() {
    #breakpoint()
    i := 0
    while i < 10 {
        j := 0
        println("I")
        println(i)
        while j < 10 {
            println("j")
            println(j)
            j += 1
        }
        i += 1
    }
}
f()
