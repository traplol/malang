
alloc_stuff :: fn () {
    n := 100
    x := [n]int
    i := 0
    while i < n {
        x[i] = i * i
        i = i + 1
    }
}

i := 0
while i < 20000 {
    alloc_stuff()
    i = i + 1
}
