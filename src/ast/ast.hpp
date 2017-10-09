#ifndef MALANG_AST_AST_HPP
#define MALANG_AST_AST_HPP

#include <typeindex>
#include <vector>
#include <string>

#define NAME_FUNC(class_name) std::string class_name::node_name() const { return #class_name; }

#define TYPE_ID_FUNC(class_name)                                        \
    Node_Type_Id class_name::type_id() {                                \
        static Node_Type_Id id = Ast_Node::num_node_types++;            \
        return id;}                                                     \
    Node_Type_Id class_name::node_type_id() const { return class_name::type_id(); }

#define AST_NODE_OVERRIDES_IMPL(class_name)                             \
    NAME_FUNC(class_name)                                               \
    TYPE_ID_FUNC(class_name)                                            \

#define AST_NODE_OVERRIDES                                      \
    virtual std::string node_name() const override;             \
    virtual std::string to_string() const override;             \
    virtual Node_Type_Id node_type_id() const override;         \
    static Node_Type_Id type_id()

#if 0
#define PRINT_DTOR printf("~%s();\n", node_name().c_str())
#else
#define PRINT_DTOR 
#endif

using Node_Type_Id = size_t;

struct Ast_Node
{
    virtual ~Ast_Node();
    virtual std::string to_string() const;
    virtual std::string node_name() const;
    virtual Node_Type_Id node_type_id() const;

    static Node_Type_Id type_id();
    static Node_Type_Id num_node_types;
};

struct Ast
{
    std::vector<Ast_Node*> roots;
    std::string to_string() const;
};


#endif /* MALANG_AST_AST_HPP */
