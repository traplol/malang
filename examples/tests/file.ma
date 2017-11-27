import std$file
str := string(std$file$File("examples/abc.txt").read_all())
println(str)
