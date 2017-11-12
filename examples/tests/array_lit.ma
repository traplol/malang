x := [1, 2, 3, 4]       # no trailing comma
y := [?a, ?b, ?c, ?d,]  # but trailing comma is also fine

i := 0
while i < x.length {
    println(x[i])
    i += 1
}

i = 0
while i < y.length {
    println(y[i])
    i += 1
}
