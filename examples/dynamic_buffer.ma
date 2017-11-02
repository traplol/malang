type StringBuilder = {
    _size := 0
    _data := buffer(8)

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

    fn [] (idx: int) -> int {
        return _data[idx]
    }

    fn []= (idx: int, val: int) {
        _data[idx] = val
    }

    fn append(val : int) {
        if _size + 1 >= _data.length {
            _resize()
        }
        _data[_size] = val
        _size += 1
    }


    fn << (val: string) -> StringBuilder {
        n := 0
        while n < val.length {
            append(val[n])
            n += 1
        }
        return self
    }

    fn to_s() -> string {
        return string(_data, _size)
    }
}

s := StringBuilder()
s << "does " << "it " << "work now?"
println(s.to_s())

