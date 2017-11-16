import lib$string

println("hello world")

# string has a readonly .length property
println("hello".length)
x := "world"
y := "hello"
println(x.length)

# and it also support readonly indexing, []= is not support for strings because 
# of their immutability.
println(x[4])

# string concatenation (+) is a native builtin method
z := y + " " + x
println("z is: \"" + z + "\"")

n := 0
while n < z.length {
    println(z[n])
    n = n + 1
}

println("hello " * 15) # hello hello hello hello hello hello hello hello hello hello 

split := ("hello " * 15).split(? )

println("split.length: " + split.length.to_s())
i := 0
while i < split.length {
    println("split[" + i.to_s() + "]=" + split[i] + " len=" + split[i].length.to_s()) 
    i += 1
}
