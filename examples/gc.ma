
alloc_stuff :: fn () {
    n := 1000
    x := [n]int
    #i := 0
    #while i < n {
    #    x[i] = i * i
    #    i += 1
    #}
}

i := 0
while i < 2000 {
    alloc_stuff()
    i += 1
}
