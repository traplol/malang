
extend int {
    fn print() {
        println(self)
    }
}

# currently the parser sees the following as one expression instead of two:
#    println(1 + 2)(3 * 2).print()
# as if println is returning a function that could be called
println(1 + 2)

(3 * 2).print()

# If you say line endings terminate expressions then you encounter this other
# problem:

x :=   1
     + 2
     + 3
     
# is that x := 1 + 2 + 3 or is it
# x := 1 <END>
# positive 2 <END>
# positive 3 <END>