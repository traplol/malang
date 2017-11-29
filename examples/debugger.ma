type T = {
    x := 1
    new () {
        breakpoint()
        y := self.x
        println(y)
    }
}
T()
