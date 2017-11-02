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

range := Range(0, 10)
for range {
    println(it)
}
