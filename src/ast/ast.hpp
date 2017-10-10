#ifndef MALANG_AST_AST_HPP
#define MALANG_AST_AST_HPP

#include "../metadata.hpp"
#include <vector>
#include <string>
#include "ast_visitor.hpp"

#if 0
#define PRINT_DTOR printf("~%s();\n", node_name().c_str())
#else
#define PRINT_DTOR 
#endif

#define AST_NODE_OVERRIDES                                      \
    METADATA_OVERRIDES;                                         \
    virtual std::string to_string() const override;             \
    virtual void accept(Ast_Visitor&) override;

#define AST_NODE_OVERRIDES_IMPL(class_name)                             \
    METADATA_OVERRIDES_IMPL(class_name)                                 \
    void class_name::accept(Ast_Visitor &visitor) { return visitor.visit(*this); }

struct Ast_Node : public Metadata
{
    virtual ~Ast_Node();
    
    virtual void accept(Ast_Visitor&) = 0;
    METADATA_OVERRIDES;
};

struct Ast
{
    std::vector<Ast_Node*> roots;
    std::string to_string() const;
};


#endif /* MALANG_AST_AST_HPP */
