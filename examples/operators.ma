
extend double {
    fn << (y: int) -> double {
        return self + y * 2
    }

    fn -@ () -> double {
        return self * (0.0 - 1.0)
    }
}

println(-42.5)
