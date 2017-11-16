type T = {
    fn xxx() { println("xxx") }
}

t := T()
t.xxx()

xxx := 42
println(xxx)


extend T {
    fn yyy() { println("yyy") }
}


t.yyy()

yyy := 99
println(yyy)

t.yyy()
t.xxx()
