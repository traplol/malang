type Range = {
    _current := 0
    _next := 0
    _end := 0

    new (start: int, end: int) {
        self._current = start
        self._next = start
        self._end = end
    }

    fn move_next() -> bool {
        _current = _next
        _next = _current + 1
        return _current != _end
    }

    fn current() -> int {
        return _current
    }
}

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
