type A = {
    _CONST := 42
    fn pub() {
        _CONST = 111
    }
}
A().pub()

