
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

  fn print() {
    println(x)
    println(y)
    println(z)
  }
}

fn println(v3: Vec3) {
  println(v3.x)
  println(v3.y)
  println(v3.z)
}

a := Vec3(1.2, 3.4, 5.5)
println(a)
(a * 2.0).print()
