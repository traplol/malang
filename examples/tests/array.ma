n := 10
x := [100]int
i := 0
while i < x.length {
    x[i] = i * x.length
    println(i)
    println(x[i])
    i = i + 1
}
