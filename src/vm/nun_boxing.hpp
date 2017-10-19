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
template<typename PointerType = void>
struct Value
{
    static constexpr uint64_t max_double  = 0xfff8000000000000;
    static constexpr uint64_t fixnum_tag   = 0xfff9000000000000;
    static constexpr uint64_t pointer_tag = 0xfffa000000000000;

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

    inline Value(PointerType *pointer)
    {
        set<PointerType>(pointer);
    }

    template<typename T>
    inline Value(T *pointer)
    {
        set<T>(pointer);
    }

    inline bool is_double() const
    {
        return v.as_bits < max_double;
    }

    inline bool is_fixnum() const
    {
        return (v.as_bits & fixnum_tag) == fixnum_tag;
    }

    inline bool is_pointer() const
    {
        return (v.as_bits & pointer_tag) == pointer_tag;
    }

    inline double as_double() const
    {
        assert(is_double());
        return v.as_double;
    }

    inline int32_t as_fixnum() const
    {
        assert(is_fixnum());
        return static_cast<int32_t>(v.as_bits & ~fixnum_tag);
    }

    template<typename T>
    inline T *as_pointer() const
    {
        assert(is_pointer());
        return reinterpret_cast<T*>(v.as_bits & ~pointer_tag);
    }

    inline PointerType *as_pointer() const
    {
        assert(is_pointer());
        return reinterpret_cast<PointerType*>(v.as_bits & ~pointer_tag);
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
        v.as_bits = static_cast<uint32_t>(number) | fixnum_tag;
        assert(as_fixnum() == number);
    }

    template<typename T>
    inline void set(T *pointer)
    {
        // ensure that the pointer really is only 48 bit
        assert((reinterpret_cast<uint64_t>(pointer) & pointer_tag) == 0);

        v.as_bits = reinterpret_cast<uint64_t>(pointer) | pointer_tag;
        assert(as_pointer() == pointer);
    }

    inline void set(PointerType *pointer)
    {
        set<PointerType>(pointer);
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
