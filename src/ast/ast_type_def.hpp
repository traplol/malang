#ifndef MALANG_AST_AST_TYPE_DEF_HPP
#define MALANG_AST_AST_TYPE_DEF_HPP

#include "../type_map.hpp"
#include "ast.hpp"
#include "ast_value.hpp"

struct Constructor_Node : public Ast_Node
{
    ~Constructor_Node();
    Constructor_Node(const Source_Location &src_loc, const std::vector<Decl_Node*> &params, const Ast_Nodes &body, Function_Type_Info *fn_type)
        : Ast_Node(src_loc)
        , params(std::move(params))
        , body(std::move(body))
        , fn_type(fn_type)
        , for_type(nullptr)
        {}

    AST_NODE_OVERRIDES;

    std::vector<Decl_Node*> params;
    Ast_Nodes body;
    Function_Type_Info *fn_type;
    Type_Info *for_type;

};

struct Type_Def_Node : public Ast_Node
{
    virtual ~Type_Def_Node();
    Type_Def_Node(const Source_Location &src_loc, Type_Info *type)
        : Ast_Node(src_loc)
        , type(type)
        {}

    AST_NODE_OVERRIDES;

    Type_Info *type;
    std::vector<Constructor_Node*> constructors;
    std::vector<Ast_Node*> fields;
    std::vector<struct Fn_Node*> methods;
};

struct Type_Alias_Node : public Ast_Node
{
    virtual ~Type_Alias_Node();
    Type_Alias_Node(const Source_Location &src_loc, Type_Info *alias)
        : Ast_Node(src_loc)
        , alias(alias)
        {}

    AST_NODE_OVERRIDES;

    Type_Info *alias;
};

struct Unalias_Node : public Ast_RValue
{
    virtual ~Unalias_Node();
    Unalias_Node(const Source_Location &src_loc, Ast_Value *value)
        : Ast_RValue(src_loc)
        , value(value)
        {}

    AST_NODE_OVERRIDES;

    Ast_Value *value;
};


#endif /* MALANG_AST_AST_TYPE_DEF_HPP */
