i := 0
while i < 10 {
    if i == 5
        break
    println("outer")
    j := 0
    while j < i {
        if j > 3
            break
        println("inner")
        j += 1
    }
    i += 1
}