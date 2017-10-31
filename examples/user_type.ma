type Vec3 = {
    x := 0.0
    y := 0.0
    z := 0.0
    new (x: double, y: double, z: double) {
        self.x = x;
        self.y = y;
        self.z = z;
    }
    fn print() {
        println(self.x)
        println(self.y)
        println(self.z)
    }
}

v3 := Vec3(1.1, 2.2, 3.3)

v3.print()

v3.x = 999.0
v3.y = 123.0
v3.z = v3.x * v3.y
v3.print()
