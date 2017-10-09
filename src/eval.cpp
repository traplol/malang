#include <sstream>

#include "ast/nodes.hpp"
#include "object/objects.hpp"
#include "eval.hpp"

#define NOT_IMPL {printf("%s:%d `%s()` not implemented\n", __FILE__, __LINE__, __FUNCTION__); abort(); return nullptr;}

#define DVISIT(ctx, node)                                               \
    (visitors[(node)->type_id()] == nullptr                             \
     ? (printf("visitor not implemented for %s\n", (node)->node_name().c_str()), nullptr) \
     : visitors[(node)->type_id()](ctx, node))

#define VISIT(ctx, node) (visitors[(node)->type_id()](ctx, node))

#define CALL_VISITOR DVISIT

using Visitor = Mal_Object *(*)(Execution_Context &ctx, Ast_Node *node);
static Visitor visitors[256]; // 256 eh ???

Mal_Object *Execution_Context::resolve(const std::string &variable_name)
{
    auto ptr = this;
    while (ptr)
    {
        if (ptr->environment.count(variable_name))
        {
            return ptr->environment[variable_name];
        }
        ptr = ptr->parent;
    }
    return nullptr;
}
Mal_Object *Execution_Context::declare(const std::string &variable_name, Type *type)
{
    if (environment.count(variable_name))
    {
        printf("Runtime error: attempted to re-declare variable: %s\n", variable_name.c_str());
        return nullptr;
    }
    auto method_new = type->methods()[Method_new];
    auto obj = method_new(*this, {});
    environment[variable_name] = obj;
    return obj;
}
Mal_Object *Execution_Context::assign(const std::string &variable_name, Mal_Object *value)
{
    auto ptr = this;
    while (ptr)
    {
        if (ptr->environment.count(variable_name))
        {
            auto obj = ptr->environment[variable_name];
            if (obj->type()->equals(value->type()))
            {
                ptr->environment[variable_name] = value;
                return value;
            }
            else
            {

                printf("Runtime error: attempted to assign to different types: %s\n", variable_name.c_str());
                return nullptr;
            }
        }
        ptr = ptr->parent;
    }
    printf("Runtime error: attempted to assign before declaring variable: %s\n", variable_name.c_str());
    return nullptr;
}
Mal_Object *Execution_Context::decl_assign(const std::string &variable_name, Type *type, Mal_Object *value)
{
    if (!type->equals(value->type()))
    {
        printf("Runtime error: type mismatch: %s\n", variable_name.c_str());
        return nullptr;
    }
    if (environment.count(variable_name))
    {
        printf("Runtime error: attempted to re-declare variable: %s\n", variable_name.c_str());
        return nullptr;
    }
    environment[variable_name] = value;
    return value;
}



static Mal_Object *visit(Execution_Context &ctx, Variable_Node *node)
{
    return ctx.resolve(node->name);
}
static Mal_Object *visit(Execution_Context &ctx, Assign_Node *node)
{
    NOT_IMPL;
    //auto lhs = eval(ctx, node->lhs);
    //auto rhs = eval(ctx, node->rhs);
    //return ctx.assign(
}

static Mal_Object *visit(Execution_Context &ctx, Decl_Node *node)
{
    return ctx.declare(node->variable_name, node->type);
}

static Mal_Object *visit(Execution_Context &ctx, Fn_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, List_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, Integer_Node *node)
{
    return new Mal_Integer(node->value);
}

static Mal_Object *visit(Execution_Context &ctx, Real_Node *node)
{
    return new Mal_Real(node->value);
}

static Mal_Object *visit(Execution_Context &ctx, String_Node *node)
{
    return new Mal_String(node->value);
}

static Mal_Object *visit(Execution_Context &ctx, Boolean_Node *node)
{
    return new Mal_Boolean(node->value);
}

static Mal_Object *visit(Execution_Context &ctx, Reference_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, Logical_Or_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, Logical_And_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, Inclusive_Or_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_or, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Exclusive_Or_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_xor, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, And_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_and, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Equals_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_eq, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Not_Equals_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_ne, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Less_Than_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_lt, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Less_Than_Equals_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_le, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Greater_Than_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_gt, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Greater_Than_Equals_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_ge, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Left_Shift_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_lshift, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Right_Shift_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_rshift, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Add_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_add, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Subtract_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_sub, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Multiply_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_mul, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Divide_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_div, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Modulo_Node *node)
{
    auto lhs = CALL_VISITOR(ctx, node->lhs);
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return lhs->call_method(Method_b_op_mod, ctx, {lhs, rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Call_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, Index_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, Field_Accessor_Node *node)
{
    NOT_IMPL;
}

static Mal_Object *visit(Execution_Context &ctx, Negate_Node *node)
{
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return rhs->call_method(Method_u_op_neg, ctx, {rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Positive_Node *node)
{
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return rhs->call_method(Method_u_op_pos, ctx, {rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Not_Node *node)
{
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return rhs->call_method(Method_u_op_not, ctx, {rhs});
}

static Mal_Object *visit(Execution_Context &ctx, Invert_Node *node)
{
    auto rhs = CALL_VISITOR(ctx, node->rhs);
    return rhs->call_method(Method_u_op_invert, ctx, {rhs});
}


void init_eval()
{
    bool once = true;
    if (once)
    {
        once = false;
#define EMPLACE_VISITOR(class_name) visitors[class_name::_type_id()] = (Visitor)static_cast<Mal_Object*(*)(Execution_Context&,class_name*)>(visit)

        EMPLACE_VISITOR(Variable_Node);
        EMPLACE_VISITOR(Assign_Node);
        EMPLACE_VISITOR(Decl_Node);
        EMPLACE_VISITOR(Fn_Node);
        EMPLACE_VISITOR(List_Node);
        EMPLACE_VISITOR(Integer_Node);
        EMPLACE_VISITOR(Real_Node);
        EMPLACE_VISITOR(String_Node);
        EMPLACE_VISITOR(Boolean_Node);
        EMPLACE_VISITOR(Reference_Node);
        EMPLACE_VISITOR(Logical_Or_Node);
        EMPLACE_VISITOR(Logical_And_Node);
        EMPLACE_VISITOR(Inclusive_Or_Node);
        EMPLACE_VISITOR(Exclusive_Or_Node);
        EMPLACE_VISITOR(And_Node);
        EMPLACE_VISITOR(Equals_Node);
        EMPLACE_VISITOR(Not_Equals_Node);
        EMPLACE_VISITOR(Less_Than_Node);
        EMPLACE_VISITOR(Less_Than_Equals_Node);
        EMPLACE_VISITOR(Greater_Than_Node);
        EMPLACE_VISITOR(Greater_Than_Equals_Node);
        EMPLACE_VISITOR(Left_Shift_Node);
        EMPLACE_VISITOR(Right_Shift_Node);
        EMPLACE_VISITOR(Add_Node);
        EMPLACE_VISITOR(Subtract_Node);
        EMPLACE_VISITOR(Multiply_Node);
        EMPLACE_VISITOR(Divide_Node);
        EMPLACE_VISITOR(Modulo_Node);
        EMPLACE_VISITOR(Call_Node);
        EMPLACE_VISITOR(Index_Node);
        EMPLACE_VISITOR(Field_Accessor_Node);
        EMPLACE_VISITOR(Negate_Node);
        EMPLACE_VISITOR(Positive_Node);
        EMPLACE_VISITOR(Not_Node);
        EMPLACE_VISITOR(Invert_Node);

#undef EMPLACE_VISITOR
    }
}

Mal_Object *eval(Execution_Context &ctx, Ast_Node *node)
{
    return CALL_VISITOR(ctx, node);
}

Mal_Object *eval(Ast_Node *ast_node)
{
    Execution_Context e;
    return eval(e, ast_node);
}
