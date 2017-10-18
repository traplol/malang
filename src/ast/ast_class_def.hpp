#ifndef MALANG_AST_AST_CLASS_DEF_HPP
#define MALANG_AST_AST_CLASS_DEF_HPP

#include "ast.hpp"
#include "../type_map.hpp"

struct Class_Def_Node : public Ast_Node
{
    Class_Def_Node(const Source_Location &src_loc, Type_Info *type_info, bool has_explicit_supertype = false)
        : Ast_Node(src_loc)
        , type_info(type_info)
        , has_explicit_supertype(has_explicit_supertype)
        {}
    Type_Info *type_info;
    bool has_explicit_supertype;
    std::vector<struct Fn_Node*> constructors;
    std::vector<struct Fn_Node*> methods;
    std::vector<struct Decl_Node*> fields;

    AST_NODE_OVERRIDES;
};

#endif /* MALANG_AST_AST_CLASS_DEF_HPP */
