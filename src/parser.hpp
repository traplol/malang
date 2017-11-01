#ifndef MALANG_PARSER_HPP
#define MALANG_PARSER_HPP

#include <string>
#include "source_code.hpp"
#include "lexer.hpp"
#include "type_map.hpp"
#include "ast/ast.hpp"

struct Parser
{
    Parser(Type_Map *types) : types(types) {}
    Ast parse(const std::string &filename);
    Ast parse(Source_Code *src_code);
    Token_Id peek_id(int n=0) const;
    const Token *peek(int n=0) const;
    void skip(int n=1);
    bool accept(Token &out, const std::vector<Token_Id> &ids);
    bool accept(const std::vector<Token_Id> &ids);
    bool expect(Token &out, Token_Id id);
    bool expect(Token_Id id);
    void handle_newlines();
    void consume_newlines();
    void report_error(const Token &token, const char *fmt, ...);
    void report_debug(const Token &token, const char *fmt, ...) const;

    Type_Map *types;
    int errors;
    Source_Code *code;
    Lexer lexer;
    int lex_idx;
    Type_Info *is_extending;
};

#endif /* MALANG_PARSER_HPP */
