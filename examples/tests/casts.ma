println(double(42) == 42.0)
println(int(42.5) == 42)
println(char(97) == ?a)
println(int(?a) == 97)
println(char(int(double(int(char(97))))) == ?a)
