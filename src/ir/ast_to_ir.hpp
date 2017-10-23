#ifndef MALANG_CODEGEN_AST_TO_IR_HPP
#define MALANG_CODEGEN_AST_TO_IR_HPP

#include <vector>
#include "../ast/ast_visitor.hpp"
#include "../ast/ast.hpp"
#include "ir.hpp"

struct Scope
{
    Scope();
    ~Scope();
    Label_Map *labels;
    Symbol_Map *symbols;
};

struct Ast_To_IR : Ast_Visitor
{
    Ast_To_IR(struct Primitive_Function_Map *primitives, Type_Map *types);

    virtual void visit(struct Variable_Node&n) override;
    virtual void visit(struct Assign_Node&n) override;
    virtual void visit(struct Decl_Node&n) override;
    virtual void visit(struct Fn_Node&n) override;
    virtual void visit(struct List_Node&n) override;
    virtual void visit(struct Integer_Node&n) override;
    virtual void visit(struct Real_Node&n) override;
    virtual void visit(struct String_Node&n) override;
    virtual void visit(struct Boolean_Node&n) override;
    virtual void visit(struct Logical_Or_Node&n) override;
    virtual void visit(struct Logical_And_Node&n) override;
    virtual void visit(struct Inclusive_Or_Node&n) override;
    virtual void visit(struct Exclusive_Or_Node&n) override;
    virtual void visit(struct And_Node&n) override;
    virtual void visit(struct Equals_Node&n) override;
    virtual void visit(struct Not_Equals_Node&n) override;
    virtual void visit(struct Less_Than_Node&n) override;
    virtual void visit(struct Less_Than_Equals_Node&n) override;
    virtual void visit(struct Greater_Than_Node&n) override;
    virtual void visit(struct Greater_Than_Equals_Node&n) override;
    virtual void visit(struct Left_Shift_Node&n) override;
    virtual void visit(struct Right_Shift_Node&n) override;
    virtual void visit(struct Add_Node&n) override;
    virtual void visit(struct Subtract_Node&n) override;
    virtual void visit(struct Multiply_Node&n) override;
    virtual void visit(struct Divide_Node&n) override;
    virtual void visit(struct Modulo_Node&n) override;
    virtual void visit(struct Call_Node&n) override;
    virtual void visit(struct Index_Node&n) override;
    virtual void visit(struct Field_Accessor_Node&n) override;
    virtual void visit(struct Negate_Node&n) override;
    virtual void visit(struct Positive_Node&n) override;
    virtual void visit(struct Not_Node&n) override;
    virtual void visit(struct Invert_Node&n) override;
    virtual void visit(struct Class_Def_Node&n) override;
    virtual void visit(struct Type_Node&n) override;
    virtual void visit(struct Decl_Assign_Node&n) override;
    virtual void visit(struct Decl_Constant_Node&n) override;
    virtual void visit(struct Return_Node&n) override;

    Malang_IR *convert_one(Ast_Node &n);
    Malang_IR *convert(Ast &ast);

private:
    Primitive_Function_Map *primitives;
    Type_Map *types;
    Scope *cur_scope;
    struct Fn_Node *cur_fn;
    Malang_IR *ir;
    IR_Node *tree;
    Symbol_Scope cur_symbol_scope;
    uint16_t cur_locals_count;
    std::vector<Scope*> scopes;

    void push_scope();
    void pop_scope();
    IR_Symbol *find_symbol(const std::string &name);

    template<typename T = IR_Node>
    T *get(Ast_Node &n)
    {
        tree = nullptr;
        n.accept(*this);
        return dynamic_cast<T*>(tree);
    }
};

#endif /* MALANG_CODEGEN_AST_TO_IR_HPP */
