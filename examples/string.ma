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

# however string multiplication doesn't yet exist for strings, so let us implement
# that in a naive manner (it makes a bunch of temporary copies, later we will
# support converting buffers into proper strings)
extend string {
    fn *(n: int) -> string {
        sum := ""
        while n > 0 {
            sum = sum + self
            n = n - 1
        }
        return sum
    }
}

println("hello " * 10) # hello hello hello hello hello hello hello hello hello hello 
