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


## @TODO: Plenty more to document.




