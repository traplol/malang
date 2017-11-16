import lib$range

# `for' actually evaluates the expression and stores it into a temporary variable
# which allows this to only instantiate only one Range
for Range(0, 10) {
    println(it) # the `it'erator is implied
}

# `it' can be nested, but currently there is no way to access higher `it'erators
# from lower scope
for Range(0, 5) {
    println(it)
    for Range(90, 95) {
        println(it)
    }
}
