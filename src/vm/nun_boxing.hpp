#ifndef NUN_BOXING_H
#define NUN_BOXING_H

#include <stdint.h>
#include <cassert>

inline bool is_negative_zero(double number)
{
    return number == 0 && *reinterpret_cast<int64_t *>(&number) != 0;
}

static_assert(sizeof(double) == sizeof(uint64_t), "double and uint64_t not same size");

/*
 * This implementation also works for 32-bit on x86 because sizeof(void*) == sizeof(int32_t)
 * on 32-bit, the downside is that each Value still requires 8 bytes
 */
template<typename ObjectType = void>
struct Value
{
    static constexpr uint64_t max_double  = 0xfff8000000000000;
    static constexpr uint64_t fixnum_tag   = 0xfff9000000000000;
    static constexpr uint64_t object_tag = 0xfffa000000000000;
    static constexpr uint64_t pointer_tag = 0xfffb000000000000;

    inline Value()
    {
        v.as_bits = 0;
    }

    inline Value(double number)
    {
        set(number);
    }

    inline Value(int32_t fixnum)
    {
        set(fixnum);
    }

    inline Value(ObjectType *object)
    {
        set<ObjectType*, object_tag>(object);
    }

    template<typename T>
    inline Value(T *pointer)
    {
        set<T>(pointer);
    }

    template<uint64_t tag>
    inline bool is() const
    {
        return (v.as_bits & tag) == tag;
    }

    template<typename T, uint64_t tag>
    inline T as() const
    {
        assert(is<tag>());
        return reinterpret_cast<T>(v.as_bits & ~tag);
    }

    inline bool is_double() const
    {
        return v.as_bits < max_double;
    }

    inline bool is_fixnum() const
    {
        return is<fixnum_tag>();
    }

    inline bool is_pointer() const
    {
        return is<pointer_tag>();
    }

    inline bool is_object() const
    {
        return is<object_tag>();
    }

    inline double as_double() const
    {
        assert(is_double());
        return v.as_double;
    }

    inline int32_t as_fixnum() const
    {
        assert(is_fixnum());
        return static_cast<int32_t>(as<uint64_t, fixnum_tag>());
    }

    template<typename T>
    inline T *as_pointer() const
    {
        assert(is_pointer());
        return as<T*, pointer_tag>();
    }

    inline ObjectType *as_object() const
    {
        assert(is_object());
        return as<ObjectType*, object_tag>();
    }

    template<typename T, uint64_t tag>
    inline void set(T thing)
    {
        static_assert(sizeof(T) == sizeof(uint64_t), "sizeof T must be 8 bytes!");
        auto thing_ptr = reinterpret_cast<uint64_t*>(&thing);
        auto thing_bytes = *thing_ptr;
        // ensure that the pointer really is only 48 bit
        assert((thing_bytes & tag) == 0);

        v.as_bits = thing_bytes | tag;
    }

    inline void set(double number)
    {
        int32_t fixnum = static_cast<int32_t>(number);

        // if the double can be losslessly stored as an int32 do so
        // (int32 doesn't have -0, so check for that too)
        if (number == fixnum && !is_negative_zero(number))
        {
            set(fixnum);
            assert(as_fixnum() == fixnum);
        }
        else
        {
            v.as_double = number;
            assert(as_double() == number);
        }
    }

    inline void set(int32_t number)
    {
        // cast to unsigned so the sign isn't automatically extended
        set<uint64_t, fixnum_tag>(static_cast<uint32_t>(number));
        assert(as_fixnum() == number);
    }

    inline void set(ObjectType *object)
    {
        set<ObjectType*, object_tag>(object);
    }

    inline uint64_t bits() const
    {
        return v.as_bits;
    }

private:
    union
    {
        double as_double;
        uint64_t as_bits;
    } v;
};

static_assert(sizeof(Value<>) == sizeof(uint64_t), "there is some padding in Value struct?");

#endif /* NUN_BOXING_H */
