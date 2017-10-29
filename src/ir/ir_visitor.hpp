#ifndef MALANG_IR_IR_VISITOR_HPP
#define MALANG_IR_IR_VISITOR_HPP

struct IR_Visitor
{
    virtual ~IR_Visitor(){}

    virtual void visit(struct IR_Assign_Top&) = 0;
    virtual void visit(struct IR_Noop&) = 0;
    virtual void visit(struct IR_Discard_Result&) = 0;
    virtual void visit(struct IR_Block&) = 0;

    virtual void visit(struct IR_Boolean&) = 0;
    virtual void visit(struct IR_Char&) = 0;
    virtual void visit(struct IR_Fixnum&) = 0;
    virtual void visit(struct IR_Single&) = 0;
    virtual void visit(struct IR_Double&) = 0;
    virtual void visit(struct IR_New_Array&) = 0;
    virtual void visit(struct IR_String&) = 0;
    virtual void visit(struct IR_Symbol&) = 0;
    virtual void visit(struct IR_Callable&) = 0;
    virtual void visit(struct IR_Indexable&) = 0;
    virtual void visit(struct IR_Member_Access&) = 0;

    virtual void visit(struct IR_Call&) = 0;
    virtual void visit(struct IR_Call_Method&) = 0;
    virtual void visit(struct IR_Call_Virtual_Method&) = 0;
    virtual void visit(struct IR_Return&) = 0;
    virtual void visit(struct IR_Label&) = 0;
    virtual void visit(struct IR_Named_Block&) = 0;
    virtual void visit(struct IR_Branch&) = 0;
    virtual void visit(struct IR_Pop_Branch_If_True&) = 0;
    virtual void visit(struct IR_Pop_Branch_If_False&) = 0;
    virtual void visit(struct IR_Branch_If_True_Or_Pop&) = 0;
    virtual void visit(struct IR_Branch_If_False_Or_Pop&) = 0;
    virtual void visit(struct IR_Assignment&) = 0;

    virtual void visit(struct IR_B_Add&) = 0;
    virtual void visit(struct IR_B_Subtract&) = 0;
    virtual void visit(struct IR_B_Multiply&) = 0;
    virtual void visit(struct IR_B_Divide&) = 0;
    virtual void visit(struct IR_B_Modulo&) = 0;
    virtual void visit(struct IR_B_And&) = 0;
    virtual void visit(struct IR_B_Or&) = 0;
    virtual void visit(struct IR_B_Xor&) = 0;
    virtual void visit(struct IR_B_Left_Shift&) = 0;
    virtual void visit(struct IR_B_Right_Shift&) = 0;
    virtual void visit(struct IR_B_Less_Than&) = 0;
    virtual void visit(struct IR_B_Less_Than_Equals&) = 0;
    virtual void visit(struct IR_B_Greater_Than&) = 0;
    virtual void visit(struct IR_B_Greater_Than_Equals&) = 0;
    virtual void visit(struct IR_B_Equals&) = 0;
    virtual void visit(struct IR_B_Not_Equals&) = 0;

    virtual void visit(struct IR_U_Not&) = 0;
    virtual void visit(struct IR_U_Invert&) = 0;
    virtual void visit(struct IR_U_Negate&) = 0;
    virtual void visit(struct IR_U_Positive&) = 0;

    virtual void visit(struct IR_Allocate_Object&) = 0;
    virtual void visit(struct IR_Deallocate_Object&) = 0;
    virtual void visit(struct IR_Allocate_Locals&) = 0;

};

#endif /* MALANG_IR_IR_VISITOR_HPP */
