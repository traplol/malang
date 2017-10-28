#ifndef MALANG_RUNTIME_PRIMITIVE_TYPES_HPP
#define MALANG_RUNTIME_PRIMITIVE_TYPES_HPP

#include <stdint.h>
#include <string>
#include "../nun_boxing.hpp"

using Malang_Value = Value<struct Malang_Object>;
using Fixnum = int32_t;
using Double = double;
using Char = char;
using Native_Code = void(*)(struct Malang_VM &vm);

struct StringConstant
{
    ~StringConstant()
    {
        m_length = 0;
        delete m_data;
        m_data = nullptr;
    }

    StringConstant()
        : m_length(0)
        , m_data(nullptr)
        {}

    StringConstant(const std::string &string)
    {
        m_length = string.size();
        if (!string.empty())
        {
            m_data = new Char[m_length];
            for (Fixnum i = 0; i < static_cast<Fixnum>(string.size()); ++i)
            {
                m_data[i] = string[i];
            }
        }
        else
        {
            m_data = nullptr;
        }
    }

    StringConstant(const StringConstant &copy)
    {
        m_length = copy.m_length;
        if (m_length)
        {
            m_data = new Char[m_length];
            for (Fixnum i = 0; i < copy.m_length; ++i)
            {
                m_data[i] = copy.m_data[i];
            }
        }
        else
        {
            m_data = nullptr;
        }
    }

    StringConstant(StringConstant &&move) noexcept
        : m_length(move.m_length)
        , m_data(move.m_data)
    {
        move.m_length = 0;
        move.m_data = nullptr;
    }

    StringConstant &operator=(const StringConstant &copy)
    {
        StringConstant tmp(copy);
        *this = std::move(tmp);
        return *this;
    }

    StringConstant &operator=(const std::string &copy)
    {
        StringConstant tmp(copy);
        *this = std::move(tmp);
        return *this;
    }

    StringConstant &operator=(StringConstant &&move)
    {
        delete m_data;
        m_length = move.m_length;
        m_data = move.m_data;
        move.m_length = 0;
        m_data = 0;
        return *this;
    }

    Char operator[](size_t index)
    {
        return m_data[index];
    }

    Fixnum size() const { return m_length; }
    Fixnum length() const { return m_length; }
    const Char *data() const { return m_data; }

private:
    friend std::ostream &operator<<(std::ostream &os, const StringConstant &string)
    {
        os << std::string(string.m_data, string.m_length);
        return os;
    }
    Fixnum m_length;
    Char *m_data;
};

struct Primitive_Function
{
    Primitive_Function(Fixnum index, Native_Code native_code, struct Function_Type_Info *fn_type)
        : index(index)
        , native_code(native_code)
        , fn_type(fn_type)
        {}
    Fixnum index;
    Native_Code native_code;
    struct Function_Type_Info *fn_type;
};


#endif
