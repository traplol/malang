#ifndef MALANG_PARSER_HPP
#define MALANG_PARSER_HPP

#include <string>
#include "source_code.hpp"
#include "lexer.hpp"
#include "ast/ast.hpp"

struct Parser
{
    Ast parse(const std::string &filename);
    Ast parse(const std::string &filename, const std::string &code);
    Token_Id peek_id(size_t n=0) const;
    const Token *peek(size_t n=0) const;
    bool accept(Token &out, const std::vector<Token_Id> &ids);
    bool accept(const std::vector<Token_Id> &ids);
    Token *expect(Token_Id id);
    void report_error(const Token &token, const char *fmt, ...);
    void report_debug(const Token &token, const char *fmt, ...) const;

    size_t errors;
    Source_Code code;
    Lexer lexer;
    size_t lex_idx;
};

#endif /* MALANG_PARSER_HPP */
