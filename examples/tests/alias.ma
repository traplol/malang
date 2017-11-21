# Strong type alias can be used to make psuedo-"subtypes" with no implicit runtime overhead

type alias A = int
type alias B = A
type alias C = B

a : A = 42
b : B = a+1
c : C = b+1
# `unalias' works by "casting" an instance to its top type, in this case `unalias' turns
# the values' types into `int'
println(unalias a)
println(unalias b)
println(unalias c)

type Thing = {
    name := "thingy"
    data := [10]int

    fn do_stuff() {
        println("Thing:do_stuff")
    }
}

type alias ThingUtil = Thing

extend ThingUtil {
    fn print_name() {
        println("ThingUtil:print_name: " + self.name)
    }
}

th := Thing()
#th.print_name()     # compile error, no method `print_name' exists for type `Thing'
th.do_stuff()       # ok
thu := ThingUtil()  # uses Thing:new
thu.print_name()    # ok  ThingUtil:print_name()
thu.do_stuff()      # ok  Thing:do_stuff()

type alias cents   = int
type alias dollars = int
type alias time_ms = int

fn cents(d: dollars) -> cents {
    return unalias(d) * 100
}
fn dollars(c: cents) -> dollars {
    return unalias(c) / 100
}

money    : dollars = 45    # ok: 45
pennies  := cents(money)   # ok: 450
duration : time_ms = 2750  # ok: 2750
println(unalias(money))
println(unalias(pennies))
println(unalias(duration))

#bad1 : cents = duration    # compile error, no conversion exists
#bad2 : dollars = duration  # compile error, no conversion exists
ok1  : dollars = 45         # 45 is an int which is the aliased type
ok2  : cents = 9999         # 9999 is an int which is the aliased type
#bad3 : cents = 42.69       # compile error, 42.69 is not an int

println(unalias(ok1))
println(unalias(ok2))

# the above creates an interesting scenario because something like:
#   `fn print(dollars) -> void'
#   `fn println(dollars) -> void'
# doesn't exist, however `fn print(int) -> void' does exist. This is reasonable because
# printing dollars in the same format as printing an integer doesn't really make sense.
# One implementation of `print' and `println' for dollars might look like:
fn print(dollars: dollars) {
    # $123
    print(?$)
    # notice you can print it using the `int' version by using unalias
    print(unalias(dollars))
}
fn println(dollars: dollars) {
    print(dollars)
    println()
}
println(money) # $45

extend Thing {
    fn baz() {
        println("Thing:baz")
    }
}

extend ThingUtil {
    fn bar() {
        println("ThingWithUtil:bar")
        self.baz()     # Thing:baz
    }
}

type alias ThingWithMoreUtil = ThingUtil
extend ThingWithMoreUtil {
    fn foo() {
        println("ThingWithMoreUtil:foo")
        self.bar()     # ThingUtil:bar
    }
}


println(1)
t := Thing()
t.baz()      # Thing:baz
#t.bar()     # compile error
#t.foo()     # compile error

println(2)
tu : ThingUtil = t
tu.baz()     # Thing:baz
tu.bar()     # ThingUtil:bar
#tu.foo()    # compile error

println(3)
tmu : ThingWithMoreUtil = tu
tmu.baz()    # Thing:baz
tmu.bar()    # ThingUtil:bar
tmu.foo()    # ThingWithMoreUtil:foo