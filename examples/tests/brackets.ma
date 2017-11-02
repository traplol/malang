type T0 = {
    new () {
        println(42)
    }
}
type T1 = {
    new () {
        println(123)
    }
}
t1 := T1()
t0 := T0()
