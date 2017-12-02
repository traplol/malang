#ifndef MALANG_CODEGEN_IR_TO_CODE_HPP
#define MALANG_CODEGEN_IR_TO_CODE_HPP

#include <vector>
#include "codegen.hpp"
#include "../ir/ir_visitor.hpp"
#include "../ir/ir.hpp"

struct IR_To_Code : IR_Visitor
{
    virtual void visit(struct IR_Noop &n) override;
    virtual void visit(struct IR_Discard_Result&) override;
    virtual void visit(struct IR_Duplicate_Result&) override;
    virtual void visit(struct IR_Assign_Top &n) override;
    virtual void visit(struct IR_Block &n) override;

    virtual void visit(struct IR_Boolean &n) override;
    virtual void visit(struct IR_Fixnum &n) override;
    virtual void visit(struct IR_Single &n) override;
    virtual void visit(struct IR_Double &n) override;
    virtual void visit(struct IR_New_Array &n) override;
    virtual void visit(struct IR_String &n) override;
    virtual void visit(struct IR_Symbol &n) override;
    virtual void visit(struct IR_Callable &n) override;
    virtual void visit(struct IR_Method &n) override;
    virtual void visit(struct IR_Indexable &n) override;
    virtual void visit(struct IR_Member_Access &n) override;

    virtual void visit(struct IR_Call &n) override;
    virtual void visit(struct IR_Call_Method &n) override;
    virtual void visit(struct IR_Call_Virtual_Method &n) override;
    virtual void visit(struct IR_Return &n) override;
    virtual void visit(struct IR_Label &n) override;
    virtual void visit(struct IR_Named_Block &n) override;
    virtual void visit(struct IR_Branch &n) override;
    virtual void visit(struct IR_Pop_Branch_If_True &n) override;
    virtual void visit(struct IR_Pop_Branch_If_False &n) override;
    virtual void visit(struct IR_Branch_If_True_Or_Pop &n) override;
    virtual void visit(struct IR_Branch_If_False_Or_Pop &n) override;
    virtual void visit(struct IR_Assignment &n) override;

    virtual void visit(struct IR_B_Add &n) override;
    virtual void visit(struct IR_B_Subtract &n) override;
    virtual void visit(struct IR_B_Multiply &n) override;
    virtual void visit(struct IR_B_Divide &n) override;
    virtual void visit(struct IR_B_Modulo &n) override;
    virtual void visit(struct IR_B_And &n) override;
    virtual void visit(struct IR_B_Or &n) override;
    virtual void visit(struct IR_B_Xor &n) override;
    virtual void visit(struct IR_B_Left_Shift &n) override;
    virtual void visit(struct IR_B_Right_Shift &n) override;
    virtual void visit(struct IR_B_Less_Than &n) override;
    virtual void visit(struct IR_B_Less_Than_Equals &n) override;
    virtual void visit(struct IR_B_Greater_Than &n) override;
    virtual void visit(struct IR_B_Greater_Than_Equals &n) override;
    virtual void visit(struct IR_B_Equals &n) override;
    virtual void visit(struct IR_B_Not_Equals &n) override;

    virtual void visit(struct IR_U_Not &n) override;
    virtual void visit(struct IR_U_Invert &n) override;
    virtual void visit(struct IR_U_Negate &n) override;
    virtual void visit(struct IR_U_Positive &n) override;

    virtual void visit(struct IR_Allocate_Object &n) override;
    virtual void visit(struct IR_Deallocate_Object &n) override;
    virtual void visit(struct IR_Allocate_Locals &n) override;

    Codegen *convert(Malang_IR &ir);

private:
    Codegen *cg;
    Malang_IR *ir;
    void convert_one(IR_Node &n);
    void convert_many(const std::vector<IR_Node*> &n);
    void binary_op_helper(struct IR_Binary_Operation &bop);
    void unary_op_helper(struct IR_Unary_Operation &bop);
};

#endif /* MALANG_CODEGEN_IR_TO_CODE_HPP */
