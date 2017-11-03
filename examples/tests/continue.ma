i := 0
while i < 10 {
    if i == 5 {
        i += 2
        continue
    }
    println("outer")
    j := 0
    while j < i {
        if j > 3 {
            println("boop")
            j += 3
            continue
        }
        println("inner")
        j += 1
    }
    i += 1
}