type T = {
    i:= 0
    new (i: int) {
        self.i = i
    }
    fn print() {
        y := 123.45
        println("I am T of ")
        println(i)
    }
}

#t := T(10)
#t.print()
#T(20).print()
arr := [100]T

i := 0
while i < arr.length {
    arr[i] = T(i)
    i += 1
}

i = 0
while i < arr.length {
    arr[i].print()
    i += 1
}
