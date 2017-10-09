#ifndef MALANG_TYPE_INFO_HPP
#define MALANG_TYPE_INFO_HPP


#include <string>
#include <vector>
#include <map>

struct Mal_Object;
struct Execution_Context;
using Mal_Proc_Args = const std::vector<Mal_Object*>&;
using Mal_Procedure = Mal_Object* (*)(Execution_Context &ctx, Mal_Proc_Args args);

struct Type
{
    bool define_type(Type *with_parent_type);
    bool is_defined() const;
    const std::string &name() const;
    Type *parent_type();
    std::vector<Mal_Procedure> &methods();

    bool equals(Type *other) const;

    static Type *get_or_create_type(const std::string &name);
    static Type *get_type(const std::string &name);
    static void initialize();
private:
    bool m_is_defined;
    std::string m_name;
    Type *m_parent_type;
    std::vector<Mal_Procedure> m_methods;

    Type(const std::string &name);
    static std::map<std::string, Type*> defined_types;
    static std::map<std::string, Type*> undefined_types;
};

enum Builtin_Methods
{
    Method_new,
    Method_delete,
    Method_inspect,
    Method_to_string,
    Method_type,
    Method_is_a,

    Method_b_op_add,
    Method_b_op_sub,
    Method_b_op_mul,
    Method_b_op_div,
    Method_b_op_mod,
    Method_b_op_lshift,
    Method_b_op_rshift,
    Method_b_op_and,
    Method_b_op_xor,
    Method_b_op_or,
    Method_b_op_lt,
    Method_b_op_le,
    Method_b_op_eq,
    Method_b_op_ne,
    Method_b_op_ge,
    Method_b_op_gt,
    Method_b_op_cmp,

    Method_u_op_neg,
    Method_u_op_pos,
    Method_u_op_invert,
    Method_u_op_not,

    NUM_BUILTIN_METHODS
};


#endif /* MALANG_TYPE_INFO_HPP */
