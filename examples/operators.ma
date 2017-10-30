
extend double {
    fn << (y: int) -> double {
        return self + y * 2
    }

}

println(42.5 << 3)

extend bool {
    fn ! () -> bool {
        return if self false else true
    }
}

println(true)
println(!true)
println(false)
println(!false)

extend double {
    fn [](idx: int) -> double {
        return self * idx
    }
    fn []=(idx: int, val: double) {
        println(self)
        println(idx)
        println(val)
    }
}

println(42.5[3])

55.123[25] = 3.14
