#ifndef MALANG_CODEGEN_AST_TO_IR_HPP
#define MALANG_CODEGEN_AST_TO_IR_HPP

#include <vector>
#include "../ast/ast_visitor.hpp"
#include "../ast/ast.hpp"
#include "../vm/runtime/primitive_types.hpp"
#include "../module_map.hpp"
#include "ir.hpp"
#include "symbol_map.hpp"
#include "bound_function_map.hpp"
#include "scope_lookup.hpp"

struct Ast_To_IR : Ast_Visitor
{
    Ast_To_IR() : noisy(false) {}
    virtual void visit(struct Import_Node&n) override;
    virtual void visit(struct Variable_Node&n) override;
    virtual void visit(struct Assign_Node&n) override;
    virtual void visit(struct Decl_Node&n) override;
    virtual void visit(struct Fn_Node&n) override;
    virtual void visit(struct List_Node&n) override;
    virtual void visit(struct Integer_Node&n) override;
    virtual void visit(struct Real_Node&n) override;
    virtual void visit(struct String_Node&n) override;
    virtual void visit(struct Boolean_Node&n) override;
    virtual void visit(struct Character_Node&n) override;
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
    virtual void visit(struct Member_Accessor_Node&n) override;
    virtual void visit(struct Negate_Node&n) override;
    virtual void visit(struct Positive_Node&n) override;
    virtual void visit(struct Not_Node&n) override;
    virtual void visit(struct Invert_Node&n) override;
    virtual void visit(struct Type_Def_Node&n) override;
    virtual void visit(struct Constructor_Node&n) override;
    virtual void visit(struct Extend_Node&n) override;
    virtual void visit(struct Type_Node&n) override;
    virtual void visit(struct Decl_Assign_Node&n) override;
    virtual void visit(struct Decl_Constant_Node&n) override;
    virtual void visit(struct Return_Node&n) override;
    virtual void visit(struct Continue_Node&n) override;
    virtual void visit(struct Break_Node&n) override;
    virtual void visit(struct While_Node&n) override;
    virtual void visit(struct For_Node&n) override;
    virtual void visit(struct If_Else_Node&n) override;
    virtual void visit(struct Array_Literal_Node&n) override;
    virtual void visit(struct New_Array_Node&n) override;

    void convert(Ast &ast, Malang_IR *ir, Module_Map *mod_map, Scope_Lookup *global, std::vector<String_Constant> *strings);

    void is_noisy(bool value) { noisy = value; }
    bool is_noisy() const { return noisy; }
private:
    bool noisy;
    uint16_t cur_locals_count;
    Module *cur_module;
    Module_Map *mod_map;
    Type_Info *is_extending;
    struct Fn_Node *cur_fn;
    IR_Label *cur_fn_ep;
    IR_Label *cur_true_label, *cur_false_label;
    IR_Label *cur_continue_label, *cur_break_label;
    Function_Parameters *cur_call_arg_types;
    Malang_IR *ir;
    std::vector<String_Constant> *strings;
    std::vector<IR_Return*> *all_returns_this_fn;
    Scope_Lookup *locality;
    Symbol_Scope cur_symbol_scope;

    void convert_intern(Ast &ast);
    IR_Symbol *find_symbol(const std::string &name);
    void convert_body(const std::vector<Ast_Node*> &src, std::vector<IR_Node*> &dst, struct IR_Value **last_node_as_value = nullptr);
    bool symbol_already_declared_here(const std::string &name);
    void gen_for_iterator_buffer(For_Node &n, IR_Value *buffer);
    void gen_for_iterator_array(For_Node &n, IR_Value *array);
    void gen_for_iterator(For_Node &n, IR_Value *itr, Method_Info *move_next, Method_Info *current);


    IR_Node *__tree;
    template<typename T = IR_Node*>
    T get(Ast_Node &n)
    {
        __tree = nullptr;
        n.accept(*this);
        return dynamic_cast<T>(__tree);
    }
};

#endif /* MALANG_CODEGEN_AST_TO_IR_HPP */
