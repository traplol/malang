type T = {
    i:= 0
    new (i: int) {
        self.i = i
    }
    print :: fn () {
        y := 123.45
        println("I am T of ")
        println(i)
    }
}

t := T(44)
t.print()
