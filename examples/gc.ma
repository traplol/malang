
alloc_stuff :: fn () {
    n := 51
    x := [n]int
    i := 0
    while i < n {
        x[i] = i * 2
        i = i + 1
    }
}

i := 0
while i < 20000000 {
    println(i)
    alloc_stuff()
    i = i + 1
}
