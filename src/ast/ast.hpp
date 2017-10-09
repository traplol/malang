#ifndef MALANG_AST_AST_HPP
#define MALANG_AST_AST_HPP

#include "../metadata.hpp"
#include <vector>
#include <string>

#if 0
#define PRINT_DTOR printf("~%s();\n", node_name().c_str())
#else
#define PRINT_DTOR 
#endif

struct Ast_Node
{
    virtual ~Ast_Node();
    virtual std::string to_string() const;
    virtual std::string node_name() const;
    virtual Type_Id type_id() const;
    static Type_Id _type_id();
};

struct Ast
{
    std::vector<Ast_Node*> roots;
    std::string to_string() const;
};


#endif /* MALANG_AST_AST_HPP */
