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

    fn substr(s: int, e: int) -> string {
        tmp := buffer(e-s)
        i := 0
        while s < e {
            tmp[i] = self[s]
            i += 1
            s += 1
        }
        return string(tmp, i)
    }

    fn split(c :int) -> []string {
        tmp := [8]string
        matches := 0
        s := 0
        e := 0
        while e < self.length {
            if self[e] == c {
                if matches >= tmp.length {
                    # Resize
                    tmp2 := [tmp.length*2]string
                    ii := 0
                    while ii < tmp.length {
                        tmp2[ii] = tmp[ii]
                        ii += 1
                    }
                    tmp = tmp2
                }
                tmp[matches] = self.substr(s, e)
                matches += 1
                s = e+1 # skip the delimiter
            }
            e += 1
        }
        res := [matches]string
        i := 0
        while i < matches {
            res[i] = tmp[i]
            i += 1
        }
        return res
    }

    fn reverse() -> string {
        tmp := buffer(self.length)
        i := 0
        while i < self.length {
            tmp[i] = self[self.length-i-1]
            i += 1
        }
        return string(tmp, i)
    }
}

extend int {
    fn to_s() -> string {
        if self == 0 {
            return "0"
        }
        tmp := buffer(32)
        negative := if self < 0 true else false
        n := if negative {-self} else {self}
        i := 0
        chars := "0123456789"
        while n != 0 {
            tmp[i] = chars[n % 10]
            n /= 10
            i += 1
        }
        if negative {
           tmp[i] = "-"[0] # no character literals at the moment so this is a hack
           i += 1
        }
        return string(tmp, i).reverse()
    }
}

println("hello " * 15) # hello hello hello hello hello hello hello hello hello hello 

spl := ("hello " * 15).split(32)

println("spl.length: " + spl.length.to_s())
i := 0
while i < spl.length {
    println("spl[" + i.to_s() + "]=" + spl[i] + " len=" + spl[i].length.to_s()) 
    i += 1
}
