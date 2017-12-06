import lib::string
import lib::rng

println("123".to_i() == 123)
println("+123".to_i() == 123)
println("0000123".to_i() == 123)
println("-123".to_i() == -123)
println("+".to_i() == 0)
println("-".to_i() == 0)
println("0".to_i() == 0)
println("1".to_i() == 1)
println("2".to_i() == 2)
println("3".to_i() == 3)
println("4".to_i() == 4)
println("5".to_i() == 5)
println("6".to_i() == 6)
println("7".to_i() == 7)
println("8".to_i() == 8)
println("9".to_i() == 9)


fn round_trip(n: int) {
    print(n)
    s := n.to_s()
    print("->" + s + "->")
    print(s + "->")
    si := s.to_i()
    print(si)
    println(if si == n " good" else " bad")
}

round_trip(0)
round_trip(-50)
round_trip(-(1 << 27))
round_trip(1 << 27)


rng := lib::rng::Random(123)
i := 0
while i < 100000000 {
    i += rng.next(1, 10)
    round_trip(i)
    i *= 10
}