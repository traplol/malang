buf := buffer(100)
s := "hello world"
i := 0
while i < s.length {
    buf[i] = s[i]
    i += 1
}

x := string(buf, i)

println(x)
println(x.length)
