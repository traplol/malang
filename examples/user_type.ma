type Vec3 = {
  x := 0.0
  y := 0.0
  z := 0.0
  new () {}  # default, only necessary because we define another constructor
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

fn println(v3: Vec3) {
  println(v3.x)
  println(v3.y)
  println(v3.z)
}

breakpoint()
v3 := Vec3(1.2, 3.0, 4.5)
println("v3")
println(v3)
println("v3*2.5")
println(v3 * 2.5)
println("v3+v3")
println(v3 + v3)
println("v3-v3")
println(v3 - v3)


type D = {
    e := 0
    new (e: int) {
        self.e = e
    }
}
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

