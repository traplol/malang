#ifndef MALANG_EVAL_HPP
#define MALANG_EVAL_HPP

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include "ast/ast.hpp"
#include "object/object.hpp"

struct Execution_Context
{
    Execution_Context *parent;
    std::map<std::string, Mal_Object*> environment;

    Mal_Object *resolve(const std::string &variable_name);
    Mal_Object *declare(const std::string &variable_name, Type *type);
    Mal_Object *assign(const std::string &variable_name, Mal_Object *value);
    Mal_Object *decl_assign(const std::string &variable_name, Type *type, Mal_Object *value);
};

void init_eval();
Mal_Object *eval(Ast_Node *ast_node);
Mal_Object *eval(Execution_Context &ctx, Ast_Node *ast_node);

#endif /* MALANG_EVAL_HPP */
