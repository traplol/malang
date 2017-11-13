#ifndef MALANG_AST_AST_HPP
#define MALANG_AST_AST_HPP

#include <vector>
#include <string>
#include "../metadata.hpp"
#include "../source_code.hpp"
#include "ast_visitor.hpp"

#if 0
#define PRINT_DTOR printf("~%s();\n", node_name().c_str())
#else
#define PRINT_DTOR 
#endif

#define AST_NODE_OVERRIDES                                      \
    METADATA_OVERRIDES;                                         \
    virtual void accept(Ast_Visitor&) override;

#define AST_NODE_OVERRIDES_IMPL(class_name)                             \
    METADATA_OVERRIDES_IMPL(class_name)                                 \
    void class_name::accept(Ast_Visitor &visitor) { visitor.visit(*this); }

struct Ast_Node : public Metadata
{
    virtual ~Ast_Node();

    Ast_Node(const Source_Location &src_loc) : src_loc(src_loc){}
    Source_Location src_loc;

    virtual void accept(Ast_Visitor&) = 0;
    METADATA_OVERRIDES;
};

using Ast_Nodes = std::vector<Ast_Node*>;

struct Ast
{
    ~Ast();
    std::vector<struct Import_Node*> imports;
    std::vector<struct Type_Def_Node*> type_defs;
    std::vector<struct Extend_Node*> extensions;
    std::vector<struct Fn_Node*> bound_funcs;
    std::vector<Ast_Node*> stmts;
};


#endif /* MALANG_AST_AST_HPP */
