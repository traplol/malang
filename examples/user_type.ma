type Vec3 = {
    x := 1.0
    y := 2.0
    z := 3.0
    arr := [100]Vec3
    new (x: double, y: double, z: double) {
        self.x = x;
        self.y = y;
        self.z = z;
    }

    new () {
        i := 0
        while i < arr.length {
            arr[i] = Vec3(i * 1.0, i * 2.0, i * 3.0)
            i += 1
        }
    }

    fn print() {
        i := 0
        while i < arr.length {
            println("=======")
            println(arr[i].x)
            println(arr[i].y)
            println(arr[i].z)
            i += 1
        }
    }
}

v3 := Vec3()
#v3.print()

i := 0
while i < v3.arr.length {
    println("=======")
    println(v3.arr[i].x)
    println(v3.arr[i].y)
    println(v3.arr[i].z)
    i += 1
}

i = 0
while i < 123 {
    v3 = Vec3(1.0, 2.0, 3.0)
    i += 1
}


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

