one :: fn() -> bool {
    println(1)
    return true
}
two :: fn() -> bool {
    println(2)
    return true
}
three :: fn() -> bool {
    println(3)
    return true
}
four :: fn() -> bool {
    println(4)
    return true
}

if one() && two() || three() && four()
    println(9999)
