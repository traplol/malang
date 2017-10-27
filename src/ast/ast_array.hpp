#ifndef MALANG_AST_AST_ARRAY_HPP
#define MALANG_AST_AST_ARRAY_HPP

#include <vector>
#include "ast_value.hpp"
#include "ast_list.hpp"

struct Array_Literal_Node : Ast_RValue
{
    virtual ~Array_Literal_Node();
    Array_Literal_Node(const Source_Location &src_loc, List_Node *values)
        : Ast_RValue(src_loc)
        , values(values)
        {assert(values && !values->contents.empty());}
    AST_NODE_OVERRIDES;
    List_Node *values;
    virtual Type_Info *get_type() override;
};

struct New_Array_Node : Ast_LValue
{
    virtual ~New_Array_Node();
    New_Array_Node(const Source_Location &src_loc, Array_Type_Info *array_type, Type_Node *of_type, Ast_Value *size)
        : Ast_LValue(src_loc)
        , array_type(array_type)
        , of_type(of_type)
        , size(size)
        {}
    AST_NODE_OVERRIDES;
    Array_Type_Info *array_type;
    Type_Node *of_type;
    Ast_Value *size;
    virtual Type_Info *get_type() override;
};

#endif /* MALANG_AST_AST_ARRAY_HPP */
