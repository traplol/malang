#include <stdio.h>
#include <cassert>
#include "type_info.hpp"
#include "eval.hpp"
#include "object/objects.hpp"


std::map<std::string, Type*> Type::defined_types;
std::map<std::string, Type*> Type::undefined_types;

static void insert_default_methods(std::vector<Mal_Procedure> &vector)
{
    vector.clear();
    vector.reserve(NUM_BUILTIN_METHODS);

#define INSERT_DEF_METHOD(idx, str) vector[idx] = \
        ([](Execution_Context&, Mal_Proc_Args) -> Mal_Object*   \
    { \
        puts("Default method '" str "' not implemented"); \
        return nullptr; \
    })

    INSERT_DEF_METHOD(Method_new, "new");
    INSERT_DEF_METHOD(Method_delete, "delete");
    INSERT_DEF_METHOD(Method_inspect, "inspect");
    INSERT_DEF_METHOD(Method_to_string, "to_string");
    INSERT_DEF_METHOD(Method_type, "type");
    INSERT_DEF_METHOD(Method_is_a, "is_a");
    INSERT_DEF_METHOD(Method_b_op_add, "b_op +");
    INSERT_DEF_METHOD(Method_b_op_sub, "b_op -");
    INSERT_DEF_METHOD(Method_b_op_mul, "b_op *");
    INSERT_DEF_METHOD(Method_b_op_div, "b_op /");
    INSERT_DEF_METHOD(Method_b_op_mod, "b_op %");
    INSERT_DEF_METHOD(Method_b_op_lshift, "b_op <<");
    INSERT_DEF_METHOD(Method_b_op_rshift, "b_op >>");
    INSERT_DEF_METHOD(Method_b_op_and, "b_op &");
    INSERT_DEF_METHOD(Method_b_op_xor, "b_op ^");
    INSERT_DEF_METHOD(Method_b_op_or, "b_op |");
    INSERT_DEF_METHOD(Method_b_op_lt, "b_op <");
    INSERT_DEF_METHOD(Method_b_op_le, "b_op <=");
    INSERT_DEF_METHOD(Method_b_op_eq, "b_op ==");
    INSERT_DEF_METHOD(Method_b_op_ne, "b_op !=");
    INSERT_DEF_METHOD(Method_b_op_ge, "b_op >=");
    INSERT_DEF_METHOD(Method_b_op_gt, "b_op >");
    INSERT_DEF_METHOD(Method_b_op_cmp, "b_op <=>");
    INSERT_DEF_METHOD(Method_u_op_neg, "u_op -");
    INSERT_DEF_METHOD(Method_u_op_pos, "u_op +");
    INSERT_DEF_METHOD(Method_u_op_invert, "u_op ~");
    INSERT_DEF_METHOD(Method_u_op_not, "u_op !");
}


Type::Type(const std::string &name)
    : m_is_defined(false)
    , m_name(name)
    , m_parent_type(nullptr)
{
    insert_default_methods(m_methods);
}


bool Type::is_defined() const
{
    return m_is_defined;
}
const std::string &Type::name() const
{
    return m_name;
}
Type *Type::parent_type()
{
    return m_parent_type;
}
std::vector<Mal_Procedure> &Type::methods()
{
    return m_methods;
}
bool Type::equals(Type *other) const
{
    return this == other;
}

void Type::initialize()
{
    static bool initialized = false;
    if (initialized) { return; }
    Mal_Object::register_type();
    Mal_Integer::register_type();
    initialized = true;
}

Type *Type::get_type(const std::string &name)
{
    if (Type::undefined_types.count(name))
    {
        return Type::undefined_types[name];
    }
    if (Type::defined_types.count(name))
    {
        return Type::defined_types[name];
    }
    return nullptr;
}

Type *Type::get_or_create_type(const std::string &name)
{
    auto existing = get_type(name);
    if (existing)
    {
        return existing;
    }
    auto new_type = new Type(name);
    undefined_types[name] = new_type;
    return new_type;
}

bool Type::define_type(Type *with_parent_type)
{
    if (m_is_defined)
    {
        return true;
    }
    if (Type::defined_types.count(m_name) != 0)
    { // Type with this name already in table...
        return false;
    }
    assert(Type::undefined_types.count(m_name));
    Type::undefined_types.erase(m_name);
    m_parent_type = with_parent_type;
    m_is_defined = true;
    Type::defined_types[m_name] = this;
    return true;
}
