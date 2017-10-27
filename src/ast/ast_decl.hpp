#ifndef MALANG_AST_AST_DECL_HPP
#define MALANG_AST_AST_DECL_HPP

#include <string>

#include "ast_value.hpp"
#include "ast_type.hpp"

struct Decl_Node : public Ast_LValue
{
    virtual ~Decl_Node();
    Decl_Node(const Source_Location &src_loc, const std::string &variable, Type_Node *type)
        : Ast_LValue(src_loc)
        , variable_name(variable)
        , type(type)
    {}

    std::string variable_name;
    Type_Node *type;

    virtual Type_Info *get_type() final;
    AST_NODE_OVERRIDES;
};

struct Decl_Assign_Node : Ast_Node
{
    virtual ~Decl_Assign_Node();
    Decl_Assign_Node(const Source_Location &src_loc, Decl_Node *decl, Ast_Value *value)
        : Ast_Node(src_loc)
        , decl(decl)
        , value(value)
        {}

    Decl_Node *decl;
    Ast_Value *value;

    AST_NODE_OVERRIDES;
};

struct Decl_Constant_Node : Ast_Node
{
    virtual ~Decl_Constant_Node();
    Decl_Constant_Node(const Source_Location &src_loc, Decl_Node *decl, Ast_Value *value)
        : Ast_Node(src_loc)
        , decl(decl)
        , value(value)
        {}

    Decl_Node *decl;
    Ast_Value *value;

    AST_NODE_OVERRIDES;
};


#endif /* MALANG_AST_AST_DECL_HPP */
