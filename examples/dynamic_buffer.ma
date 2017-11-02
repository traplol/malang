type StringBuilder = {
    _size := 0
    _data := buffer(8)

    fn size() -> int {
        return _size;
    }

    fn _resize() {
        cpy := buffer(self._data.length * 2)
        n := 0
        while n < _size {
            cpy[n] = self._data[n]
            n += 1
        }
        _data = cpy
    }

    fn [] (idx: int) -> int {
        return self._data[idx]
    }

    fn []= (idx: int, val: int) {
        self._data[idx] = val
    }

    fn append(val : int) {
        if self._size + 1 >= self._data.length {
            self._resize()
        }
        self._data[self._size] = val
        self._size += 1
    }


    fn << (val: string) -> StringBuilder {
        n := 0
        while n < val.length {
            self.append(val[n])
            n += 1
        }
        return self
    }

    fn to_str() -> string {
        return string(self._data, self._size)
    }
}

s := StringBuilder()
x := "hello world"

s << "it can " # << "work like this" << " too!"
println(s.to_str())
