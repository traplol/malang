n := 10
x := [n]int

i := 0
while i < n {
    x[i] = i * 2
    i = i + 1
}

i = 0
while i < n {
    println(x[i])
    i = i + 1
}
