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

# however string multiplication doesn't yet exist for strings, so let us implement it
extend string {
    # naive version that makes a bunch of copies
    #fn * (n: int) -> string {
    #    sum := ""
    #    while n > 0 {
    #        sum = sum + self
    #        n = n - 1
    #    }
    #    return sum
    #}

    fn * (n: int) -> string {

        # edge case
        if n <= 0 {
            return string(buffer(0))
        }

        total_len := self.length * n
        tmp_buf := buffer(total_len)

        i := 0
        while n > 0 {
            k := 0
            while k < self.length {
                tmp_buf[i] = self[k]
                i += 1
                k += 1
            }
            n -= 1
        }

        return string(tmp_buf, total_len)
    }
}

println("hello " * 15) # hello hello hello hello hello hello hello hello hello hello 
