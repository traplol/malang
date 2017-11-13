type Random = {
    # A simple implementation of the xorshift128 PRNG
    # https://en.wikipedia.org/wiki/Xorshift
    state := [4]int
    new () {
        reset((1 << 30) - 1)
    }

    new (seed: int) {
        reset(seed)
    }

    fn reset(seed: int) {
        state = [seed,
                (1 << 24) - 1,
                (1 << 16) - 1,
                (1 << 8) - 1]
    }

    fn next() -> int {
        s: int
        t := state[3]
        t ^= t << 11
        t ^= t >>  8
        state[3] = state[2]
        state[2] = state[1]
        s = state[0]
        state[1] = s
        t ^= s
        t ^= s >> 19
        state[0] = t
        return if t < 0 {-t} else {t}
    }

    fn next(max: int) -> int {
        return next() % max
    }

    fn next(min: int, max: int) -> int {
        d := max - min
        return (next() % d) + min
    }
}


strs := ["╱", "╲",]

r := Random()
i := 1
while i <= 1000 {
    print(strs[r.next(strs.length)])
    if i % 50 == 0
        println()
    i += 1
}

