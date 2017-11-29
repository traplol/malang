import lib::rng

strs := ["╱", "╲",]

r := lib::rng::Random()
i := 1
while i <= 1000 {
    print(strs[r.next(strs.length)])
    if i % 50 == 0
        println()
    i += 1
}

