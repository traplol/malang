#ifndef MALANG_PROCEDURE_HPP
#define MALANG_PROCEDURE_HPP


#include "ast/ast.hpp"

using Mal_Procedure = Ast_Node*(*)(Execution_Context &ctx, Ast_Node *args);


#endif /* MALANG_PROCEDURE_HPP */
