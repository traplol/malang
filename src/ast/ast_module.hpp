#ifndef MALANG_AST_AST_MODULE_HPP
#define MALANG_AST_AST_MODULE_HPP

#include "ast.hpp"

struct Module;
struct Import_Node : Ast_Node
{
    virtual ~Import_Node();
    Import_Node(const Source_Location &src_loc, Module *mod_info)
        : Ast_Node(src_loc)
        , mod_info(mod_info)
        {}

    AST_NODE_OVERRIDES;
    
    Module *mod_info;
};

#endif /* MALANG_AST_AST_MODULE_HPP */
