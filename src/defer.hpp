#ifndef MALANG_DEFER_HPP
#define MALANG_DEFER_HPP

#include <functional>

struct _defer_impl
{
    using defer_func = std::function<void()>;
    _defer_impl(defer_func fn)
        : m_on_destruct(fn)
        {}
    ~_defer_impl()
    {
        m_on_destruct();
    }
private:
    defer_func m_on_destruct;
};

#define CAT(x, y) x##y
#define XCAT(x, y) CAT(x, y)
#define UNIQ(x)    XCAT(x, __COUNTER__)
#define defer(code) _defer_impl UNIQ(_defer_) ([&]() code )
#define defer1(code) _defer_impl UNIQ(_defer_) ([&]() {code;} )

#endif /* MALANG_DEFER_HPP */

