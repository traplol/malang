import string
import range

fn get_to_eof() -> string {
    sb := string::StringBuilder()
    while true {
        c := get_char();
        if int(c) == -1
            break
        sb.append(c)
    }
    return sb.to_s()
}

fn indent(i: int) {
    for range::Range(0, i*4) {
        print(? )
    }
}


src := get_to_eof()
ind := 0

# Named functions are compiled first and aren't able to see any global variables however,
# we can allow a function to see them by lazily compiling this function only after the
# global variable has at least been declared.
out := fn(s: string) {
    indent(ind)
    println(s)
}

out("#include <stdio.h>")
out("char array[30000];")
out("int main() {")

ind += 1
out("char *ptr = array;")

for string::Iterator(src) {
    if it == ?>
        out("++ptr;")
    else if it == ?<
        out("--ptr;")
    else if it == ?+
        out("++*ptr;")
    else if it == ?-
        out("--*ptr;")
    else if it == ?.
        out("putchar(*ptr);")
    else if it == ?,
        out("*ptr=getchar();")
    else if it == ?[ {
        out("while (*ptr) {")
        ind += 1
    }
    else if it == ?] {
        ind -= 1
        out("}")
    }
}

out("return 0;")
ind -= 1
out("}")
