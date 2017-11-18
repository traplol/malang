type MethodOrder = {
    fn a() {
        println("a")
        b()
    }
    fn b() {
        println("b")
        c()
    }
    fn c() {
        println("c")
        d()
    }
    fn d() {
        println("d")
        w()
    }

    
    fn z() {
        println("z")
    }
    fn y() {
        println("y")
        z()
    }
    fn x() {
        println("x")
        y()
    }
    fn w() {
        println("w")
        x()
    }

    
}

x := MethodOrder()
x.a()
x.w()
