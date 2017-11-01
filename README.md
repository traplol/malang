# malang
malang is a general purpose, statically and strongly typed, structured programming language. It is early 
in development but already it features:

+ a tracing garbage collector
+ type inference
+ primitive types such as ```string```, ```int```, ```bool```, ```double```, ```buffer```s, and arrays.
+ first class function expressions
+ block expressions
+ bound/named functions that support parameter overloading
+ extend any type with operators and methods

Planned features:

+ strong type aliasing
+ user-defined structures
+ multiple return values
+ a module system
+ string interpolation
+ closures
+ zero overhead generics
+ traits
+ optimized ahead of time compiliation that can be saved to disk and run at a later time.


malang is compiled into a fairly low-level stack-based bytecode which is then interpreted by the malangVM. 

## Building
There are plans to support and move towards CMake, but [tup](http://gittup.org/tup/) is the current build tool.

### Building on *nix

1. Make sure you have installed the dependencies:
  * `g++` 4.7 or later or `clang++` 3.x or later
  * `tup`
  * `git`

2. Build and run
  ```sh
  $ git clone https://github.com/traplol/malang
  $ cd malang
  $ tup
  $ ./mal examples/hello-world.ma
  ```

## Crash course

### Variables
Variables can be declared with their type, or the type may be entirely omitted if it can be immediately deduced.
```
x : int
x = 42
y : double = 123.45

PI :: 3.14159
PI = 3.0             # compile error because :: signifies it is a constant

z := "hello world"

a : int = 1.5        # compile error without explicit cast

b := returns_int()

c := if true "yep" else "nope"
```

### Functions
Functions can be defined and bound to a name, assigned to a variable, or used as a value in an expression. The 
main difference between binding to a name and to a variable is a function bound to a name can share that name
with other bound functions if their parameter types differ.

```
# Binds to 'hello' with with no parameters
fn hello() -> void {
  println("hello world")
}

# Binds to 'hello' with 1 parameter (int)
# notice the void return type has been ommitted/implied
fn hello(a: int) {
  hello()
  println(a)
}

min := fn(a: double, b: double) -> double {
  if a < b
    return a
  else
    return b
}

fib := fn(n: int) -> int {
  if n < 3 {
    return 1
  }
  else {
    # notice the use of 'recurse' because otherwise 'fib' could be redefined to another 
    # function with the same function signature and that one would get called instead.
    return recurse(n-1) + recurse(n-2)
  }
}

```


### User defined types

```
type D = {
    e := 0
    new (e: int) {
        self.e = e
    }
}
# A, B, and C don't have a constructor that takes parameters so the default constrcutor
# is implied. In the future, types will be order independent.
type C = {
    d := D(42)
}
type B = {
    c := C()
}
type A = {
    b := B()
}
a := A()
println(a.b.c.d.e)



type Vec3 = {
  x := 0.0
  y := 0.0
  z := 0.0

  # default, only necessary because we define another constructor
  new () {}  

  new (x: double, y: double, z: double) {
    self.x = x
    self.y = y
    self.z = z
  }

  fn * (scalar: double) -> Vec3 {
    # note the implicit resolution of x,y,z 
    return Vec3(x*scalar, y*scalar, z*scalar)
  }

  fn + (other: Vec3) -> Vec3 {
    # and here we can shadow fields of the same name but still access them with
    # the implicit "self" reference
    x := self.x + other.x
    y := self.y + other.y
    z := self.z + other.z
    return Vec3(x, y, z)
  }

  fn - (other: Vec3) -> Vec3 {
    x := self.x - other.x
    y := self.y - other.y
    z := self.z - other.z
    return Vec3(x, y, z)
  }
}

a := Vec3() # creates a Vec3 with the default constructor
a.x = 999.0
b := Vec3(1.5, 2.3, 3.14)
c := a + b
println(c.x) # 1000.5
println(c.y) # 2.3
println(c.z) # 3.14

```