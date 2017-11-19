import string
import range

extend string {
    fn index_of(c: char) -> int {
        for Range(0, self.length) {
            if self[it] == c {
                return it
            }
        }
        return -1
    }
    fn contains(c: char) -> bool {
        return if index_of(c) != -1 true else false
    }

    fn to_b() -> buffer {
        b := buffer(self.length+1)
        for Range(0, self.length) {
            b[it] = self[it]
        }
        return b
    }
}

type Vector = {
    _length := 0
    _capacity := 8
    _array := [_capacity]int

    fn push_back(item: int) {
        if _length + 1 >= _capacity {
            resize(_capacity * 2)
        }
        _array[_length] = item
        _length += 1
    }
    fn pop_back() -> int {
        n := peek()
        _length -= 1
        return n
    }
    fn [](idx: int) -> int {
        return _array[idx]
    }
    fn []=(idx: int, val: int) {
        _array[idx] = val
    }
    fn resize(new_size: int) {
        tmp := [new_size]int
        n := if _length < new_size _length else new_size
        for Range(0, n)
            tmp[it] = _array[it]
        _array = tmp
        _capacity = new_size
    }
    fn peek() -> int {
        return _array[_length-1]
    }
    fn length() -> int {
        return _length
    }
}

type BfMachine = {
    pc := 0
    dp := 0
    dp_max := 0
    data := buffer(30000)
    jmp_table := Vector()
    code := buffer(0)
    good := true

    new () {
        for Range(0, data.length) {
            data[it] = char(0)
        }
    }

    fn read(source: string) {
        jmp_stack := Vector()
        jmps := Vector()
        code := string$StringBuilder()
        i := 0
        for string$Iterator(source) {
            if "<>+-.,[]".contains(it) {
                code.append(it)
                jmps.push_back(0)
                if it == ?[ {
                    jmp_stack.push_back(i)
                }
                else if it == ?] {
                    if jmps.length() < 1 {
                        println("mismatched ']'")
                        good = false
                    }
                    tmp := jmp_stack.pop_back()
                    jmps[i] = tmp
                    jmps[tmp] = i
                }
                i += 1
            }
        }
        code << char(0)
        if jmp_stack.length() != 0 {
            println("mismatched '['...")
            good = false
        }
        else {
            self.code = code.to_s().to_b()
            self.jmp_table = jmps
        }
        println("ok.")
    }

    # >
    fn inc_dp() {
        dp += 1
        if dp > dp_max
            dp_max = dp
        pc += 1
    }

    # <
    fn dec_dp() {
        dp -= 1
        pc += 1
    }

    # +
    fn inc_at_dp() {
        data[dp] += char(1)
        pc += 1
    }

    # -
    fn dec_at_dp() {
        data[dp] -= char(1)
        pc += 1
    }

    # .
    fn output() {
        print(data[dp])
        pc += 1
    }

    # ,
    fn input() {
        data[dp] = get_char()
        pc += 1
    }

    # [
    fn jmp_f() {
        pc = if data[dp] == char(0)
            jmp_table[pc]
        else
            pc + 1
    }

    # ]
    fn jmp_b() {
        pc = if data[dp] != char(0)
            jmp_table[pc]
        else
            pc + 1
    }

    fn exec() {
        while true {
            ins := code[pc]
            if ins == 0 {
                println("done.")
                return
            }
            if ins == ?>
                inc_dp()
            else if ins == ?<
                dec_dp()
            else if ins == ?+
                inc_at_dp()
            else if ins == ?-
                dec_at_dp()
            else if ins == ?[
                jmp_f()
            else if ins == ?]
                jmp_b()
            else if ins == ?.
                output()
            else if ins == ?,
                input()
            else 
                pc += 1
        }
    }
}

# https://en.wikipedia.org/wiki/Brainfuck
rot13 := " 
-,+[                         Read first character and start outer character reading loop
    -[                       Skip forward if character is 0
        >>++++[>++++++++<-]  Set up divisor (32) for division loop
                               (MEMORY LAYOUT: dividend copy remainder divisor quotient zero zero)
        <+<-[                Set up dividend (x minus 1) and enter division loop
            >+>+>-[>>>]      Increase copy and remainder / reduce divisor / Normal case: skip forward
            <[[>+<-]>>+>]    Special case: move remainder back to divisor and increase quotient
            <<<<<-           Decrement dividend
        ]                    End division loop
    ]>>>[-]+                 End skip loop; zero former divisor and reuse space for a flag
    >--[-[<->+++[-]]]<[         Zero that flag unless quotient was 2 or 3; zero quotient; check flag
        ++++++++++++<[       If flag then set up divisor (13) for second division loop
                               (MEMORY LAYOUT: zero copy dividend divisor remainder quotient zero zero)
            >-[>+>>]         Reduce divisor; Normal case: increase remainder
            >[+[<+>-]>+>>]   Special case: increase remainder / move it back to divisor / increase quotient
            <<<<<-           Decrease dividend
        ]                    End division loop
        >>[<+>-]             Add remainder back to divisor to get a useful 13
        >[                   Skip forward if quotient was 0
            -[               Decrement quotient and skip forward if quotient was 1
                -<<[-]>>     Zero quotient and divisor if quotient was 2
            ]<<[<<->>-]>>    Zero divisor and subtract 13 from copy if quotient was 1
        ]<<[<<+>>-]          Zero divisor and add 13 to copy if quotient was 0
    ]                        End outer skip loop (jump to here if ((character minus 1)/32) was not 2 or 3)
    <[-]                     Clear remainder from first division if second division was skipped
    <.[-]                    Output ROT13ed character from copy and clear it
    <-,+                     Read next character
]                            End character reading loop"

hello_world := "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++."

bf := BfMachine()
bf.read(hello_world)
if bf.good
    bf.exec()
