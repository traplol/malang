type T = {
    x := 1
    new () {
        y := self.x
        println(y)
    }
}
