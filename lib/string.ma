
extend string {

    fn + (c: char) -> string {
        b := buffer(1)
        b[0] = c
        return self + string(b, 1)
    }

    fn == (other: string) -> bool {
        if self.length != other.length {
            return false
        }
        i := 0
        while i < self.length {
            if self[i] != other[i] {
                return false
            }
            i += 1
        }
        return true
    }

    fn != (other: string) -> bool {
        return if self == other false else true
    }

    fn * (n: int) -> string {

        # edge case
        if n <= 0 {
            return string(buffer(0))
        }

        total_len := self.length * n
        tmp_buf := buffer(total_len)

        i := 0
        while n > 0 {
            k := 0
            while k < self.length {
                tmp_buf[i] = self[k]
                i += 1
                k += 1
            }
            n -= 1
        }

        return string(tmp_buf, total_len)
    }

    fn substr(s: int, e: int) -> string {
        if e-s < 0 {
            return ""
        }
        tmp := buffer(e-s)
        i := 0
        while s < e {
            tmp[i] = self[s]
            i += 1
            s += 1
        }
        return string(tmp, i)
    }

    fn split(c: char) -> []string {
        tmp := [8]string
        matches := 0
        s := 0
        e := 0
        while e < self.length {
            if self[e] == c {
                if matches >= tmp.length {
                    # Resize
                    tmp2 := [tmp.length*2]string
                    ii := 0
                    while ii < tmp.length {
                        tmp2[ii] = tmp[ii]
                        ii += 1
                    }
                    tmp = tmp2
                }
                tmp[matches] = self.substr(s, e)
                matches += 1
                s = e+1 # skip the delimiter
            }
            e += 1
        }
        res := [matches]string
        i := 0
        while i < matches {
            res[i] = tmp[i]
            i += 1
        }
        return res
    }

    fn reverse() -> string {
        tmp := buffer(self.length)
        i := 0
        while i < self.length {
            tmp[i] = self[self.length-i-1]
            i += 1
        }
        return string(tmp, i)
    }

    fn to_i() -> int {
        if self.length == 0 return 0

        n := 0

        i := 0
        sign := 1
        if self[i] == ?+ or self[i] == ?- {
            sign = if self[i] == ?- {-1} else {1}
            i += 1
        }
        while i < self.length {
            if self[i] < ?0 or self[i] > ?9 {
                break
            }
            n += self[i] - ?0
            n *= 10
            i += 1
        }
        return (n/10) * sign
    }
}

extend int {
    fn to_s() -> string {
        if self == 0 {
            return "0"
        }
        tmp := buffer(32)
        negative := self < 0
        n := if negative {-self} else {self}
        i := 0
        chars := "0123456789"
        while n != 0 {
            tmp[i] = chars[n % 10]
            n /= 10
            i += 1
        }
        if negative {
            tmp[i] = ?-
            i += 1
        }
        return string(tmp, i).reverse()
    }
}

type StringBuilder = {
    _size := 0
    _data := buffer(0)

    new () {
        _data = buffer(8)
    }

    new (n: int) {
        _data = buffer(n)
    }

    fn size() -> int {
        return _size;
    }

    fn _resize() {
        cpy := buffer(_data.length * 2)
        n := 0
        while n < _size {
            cpy[n] = _data[n]
            n += 1
        }
        _data = cpy
    }

    fn [] (idx: int) -> char {
        return _data[idx]
    }

    fn []= (idx: int, val: char) {
        _data[idx] = val
    }

    fn append(val : char) {
        if _size + 1 >= _data.length {
            _resize()
        }
        _data[_size] = val
        _size += 1
    }

    fn append(val: string) {
        n := 0
        while n < val.length {
            append(val[n])
            n += 1
        }
    }

    fn << (val: string) -> StringBuilder {
        append(val)
        return self
    }

    fn << (val: char) -> StringBuilder {
        append(val)
        return self
    }

    fn to_s() -> string {
        return string(_data, _size)
    }
}

type Iterator = {
    _cur := -1
    _s := ""
    new (str: string) {
        _s = str
    }

    fn current() -> char {
        return _s[_cur]
    }

    fn move_next() -> bool {
        _cur += 1
        return _cur < _s.length
    }
}

