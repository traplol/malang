#ifndef NUN_BOXING_H
#define NUN_BOXING_H

#include <stdint.h>
#include <cassert>

#define CHAR_BIT 8

static_assert(sizeof(double) == sizeof(uint64_t), "double and uint64_t not same size");

/*
 * This implementation also works for 32-bit on x86 because sizeof(void*) == sizeof(int32_t)
 * on 32-bit, the downside is that each Value still requires 8 bytes
 */

#if DEBUG_MODE
#define STR(t) XSTR(t)
#define XSTR(t) #t
#define THROW_IF_NOT(pred)                                              \
    if (!(pred)) {                                                      \
        printf("assertion failed: %s %s:%d\n", STR(pred), __FILE__, __LINE__); \
        throw nullptr;                                                  \
    }
#else
#define THROW_IF_NOT(pred)
#endif
    

template<typename ObjectType>
struct Value
{
    static constexpr uint64_t max_double  = UINT64_C(0xfff8000000000000);
    static constexpr uint64_t fixnum_tag  = UINT64_C(0xfff9000000000000);
    static constexpr uint64_t object_tag  = UINT64_C(0xfffa000000000000);
    static constexpr uint64_t pointer_tag = UINT64_C(0xfffc000000000000);

    inline Value()
    {
        v.as_bits = 0;
    }

    static inline Value<ObjectType> with_bits(uint64_t bits)
    {
        Value<ObjectType> v;
        v.v.as_bits = bits;
        return v;
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

    inline Value(void *pointer)
    {
        set<void*, pointer_tag>(pointer);
    }

    template<uint64_t tag>
    inline bool is() const
    {
        decltype(tag) r = bits() & tag;
        return r == tag;
    }

    template<typename T, uint64_t tag>
    inline T as() const
    {
        THROW_IF_NOT(is<tag>());
        return reinterpret_cast<T>(v.as_bits & ~tag);
    }

    inline bool is_double() const
    {
        return v.as_bits < max_double;
    }

    inline bool is_fixnum() const
    {
        //return is<fixnum_tag>();
        return (v.as_bits & fixnum_tag) == fixnum_tag;
    }

    inline bool is_object() const
    {
        //return is<object_tag>();
        return (v.as_bits & object_tag) == object_tag;
    }

    inline bool is_pointer() const
    {
        return (v.as_bits & pointer_tag) == pointer_tag;
        //return is<pointer_tag>();
    }

    inline double as_double() const
    {
        THROW_IF_NOT(is_double());
        return v.as_double;
    }

    inline int32_t as_fixnum() const
    {
        THROW_IF_NOT(is_fixnum());
        return static_cast<int32_t>(as<uint64_t, fixnum_tag>());
    }

    inline ObjectType *as_object() const
    {
        THROW_IF_NOT(is_object());
        return as<ObjectType*, object_tag>();
    }

    inline void *as_pointer() const
    {
        THROW_IF_NOT(is_pointer());
        return as<void*, pointer_tag>();
    }

    template<typename T, uint64_t tag>
    inline void set(T thing)
    {
        uint64_t thing_bytes = 0;
        if (sizeof(T) == sizeof(uint64_t))
        {
            auto thing_ptr = reinterpret_cast<uint64_t*>(&thing);
            thing_bytes = *thing_ptr;
        }
        else if (sizeof(T) < sizeof(uint64_t))
        {
            auto thing_ptr = reinterpret_cast<uint64_t*>(&thing);
            auto tmp = *thing_ptr;
            uint64_t mask = (1ull << sizeof(T) * CHAR_BIT) - 1;
            thing_bytes = tmp & mask;
        }
        else
        {
            assert(0 && "sizeof T must be <= 8");
        }
        // ensure thing fits
        assert((thing_bytes & tag) == 0);
        v.as_bits = thing_bytes | tag;
    }

    inline void set(double number)
    {
        v.as_double = number;
        assert(as_double() == number);
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

    inline void set(void *pointer)
    {
        set<void*, pointer_tag>(pointer);
    }

    inline uint64_t bits() const
    {
        return v.as_bits;
    }

private:
    union
    {
        volatile double as_double;
        volatile uint64_t as_bits;
    } v;

    inline bool is_negative_zero(double number)
    {
        return number == 0 && *reinterpret_cast<int64_t *>(&number) != 0;
    }
};

static_assert(sizeof(Value<int>) == sizeof(uint64_t), "there is some padding in Value struct?");

#endif /* NUN_BOXING_H */
