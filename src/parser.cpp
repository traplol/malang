#include <memory>
#include <stdarg.h>

#include "parser.hpp"
#include "ast/nodes.hpp"

template<typename... T>
using uptr = std::unique_ptr<T...>;

Ast Parser::parse(const std::string &filename)
{
    throw "not implemented";
}


static Ast_Node *parse_top_level(Parser &parser);
Ast Parser::parse(const std::string &filename, const std::string &code)
{
    this->errors = 0;
    this->code = Source_Code(filename, code);
    this->lexer.lex(this->code);
    //this->lexer.dump();
    this->lex_idx = 0;
    Ast ast;
    while (auto root = parse_top_level(*this))
    {
        ast.roots.push_back(root);
    }
    return ast;
}

const Token *Parser::peek(size_t n) const
{
    if (lex_idx + n >= lexer.tokens.size())
    {
        return nullptr;
    }
    return &lexer.tokens[lex_idx];
}

Token_Id Parser::peek_id(size_t n) const
{
    auto tk = peek(n);
    return tk ? tk->id() : Token_Id::Invalid;
}

bool Parser::accept(Token &out, const std::vector<Token_Id> &ids)
{
    if (lex_idx >= lexer.tokens.size())
    {
        return false;
    }
    for (auto &&id : ids)
    {
        if (id == lexer.tokens[lex_idx].id())
        {
            out = lexer.tokens[lex_idx];
            ++lex_idx;
            return true;
        }
    }
    return false;
}
bool Parser::accept(const std::vector<Token_Id> &ids)
{
    Token dummy;
    return accept(dummy, ids);
}
bool Parser::expect(Token &out, Token_Id id)
{
    if (lex_idx >= lexer.tokens.size())
    {
        auto &last_tk = lexer.tokens[lexer.tokens.size()-1];
        report_error(last_tk, "Expected token %s but there are no more tokens.\n", to_string(id).c_str());
        return false;
    }
    auto &cur_tk = lexer.tokens[lex_idx];
    if (id != cur_tk.id())
    {
        report_error(cur_tk, "Expected token %s but got %s.\n", to_string(id).c_str(), cur_tk.debug().c_str());
        return false;
    }
    ++lex_idx;
    out = cur_tk;
    return true;
}
bool Parser::expect(Token_Id id)
{
    Token dummy;
    return expect(dummy, id);
}

void Parser::report_error(const Token &token, const char *fmt, ...)
{
    errors++;
    va_list vargs;
    va_start(vargs, fmt);
    code.vreport_at_src_loc("error", token.src_loc(), fmt, vargs);
    va_end(vargs);
}

void Parser::report_debug(const Token &token, const char *fmt, ...) const
{
    va_list vargs;
    va_start(vargs, fmt);
    code.vreport_at_src_loc("debug", token.src_loc(), fmt, vargs);
    va_end(vargs);
}

// forward decls
static uptr<Ast_Node> parse_assignment(Parser &parser);
static uptr<Ast_Node> parse_declaration(Parser &parser);
static uptr<Ast_Node> parse_logical_or_exp(Parser &parser);
static uptr<Ast_Node> parse_logical_and_exp(Parser &parser);
static uptr<Ast_Node> parse_inclusive_or_exp(Parser &parser);
static uptr<Ast_Node> parse_exclusive_or_exp(Parser &parser);
static uptr<Ast_Node> parse_and_exp(Parser &parser);
static uptr<Ast_Node> parse_equality_exp(Parser &parser);
static uptr<Ast_Node> parse_relational_exp(Parser &parser);
static uptr<Ast_Node> parse_shift_exp(Parser &parser);
static uptr<Ast_Node> parse_additive_exp(Parser &parser);
static uptr<Ast_Node> parse_multiplicative_exp(Parser &parser);
static uptr<Ast_Node> parse_unary_exp(Parser &parser);
static uptr<Ast_Node> parse_postfix_exp(Parser &parser);
static uptr<Ast_Node> parse_primary(Parser &parser);
static uptr<List_Node> parse_expression_list(Parser &parser);
static uptr<Ast_Node> parse_expression(Parser &parser);
static uptr<Ast_Node> parse_statement(Parser &parser);
static bool parse_body(Parser &parser, std::vector<Ast_Node*> &body);
static uptr<Ast_Node> parse_fn(Parser &parser);

#define SAVE auto _save_idx = parser.lex_idx
#define RESTORE parser.lex_idx = _save_idx
#define PARSE_FAIL {RESTORE; return nullptr;}
#define ACCEPT_OR_FAIL(...) if (!parser.accept(__VA_ARGS__)) { PARSE_FAIL; }
#define CHECK_OR_FAIL(check) if (!(check)) PARSE_FAIL;
//#define DEBUG(tk, ...) parser.report_debug(tk, __VA_ARGS__)
#define DEBUG(tk, ...)

static uptr<Ast_Node> parse_assignment(Parser &parser)
{
    SAVE;
    auto lhs = parse_declaration(parser);
    if (!lhs) 
    {
        Token ident;
        ACCEPT_OR_FAIL(ident, { Token_Id::Identifier });
    }
    ACCEPT_OR_FAIL({ Token_Id::Equals });
    auto rhs = parse_expression(parser);
    CHECK_OR_FAIL(rhs);
    return uptr<Assign_Node>(new Assign_Node(lhs.release(), rhs.release()));
}
static uptr<Ast_Node> parse_declaration(Parser &parser)
{
    SAVE;
    Token ident, type_name;
    ACCEPT_OR_FAIL(ident, { Token_Id::Identifier });
    ACCEPT_OR_FAIL({ Token_Id::Colon });
    if (parser.accept(type_name, { Token_Id::Identifier }))
    {
        auto type = Type::get_or_create_type(type_name.to_string());
        return uptr<Decl_Node>(new Decl_Node(ident.to_string(), type));
    }
    if (parser.peek_id() == Token_Id::Equals)
    {   // x : = 
        return uptr<Decl_Node>(new Decl_Node(ident.to_string(), nullptr));
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_logical_or_exp(Parser &parser)
{   // l_or :=
    //     l_and
    //     l_or || l_and
    SAVE;
    auto lhs = parse_logical_and_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Log_Or}))
    {
        auto rhs = parse_logical_and_exp(parser);
        CHECK_OR_FAIL(rhs);
        lhs = uptr<Ast_Node>(new Logical_Or_Node(lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_logical_and_exp(Parser &parser)
{   // l_and :=
    //     inclusive_or
    //     l_and && inclusive_or
    SAVE;
    auto lhs = parse_inclusive_or_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Log_And}))
    {
        auto rhs = parse_inclusive_or_exp(parser);
        CHECK_OR_FAIL(rhs);
        lhs = uptr<Ast_Node>(new Logical_And_Node(lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_inclusive_or_exp(Parser &parser)
{   // inclusive_or :=
    //     exclusive_or
    //     inclusive_or | exclusive_or
    SAVE;
    auto lhs = parse_exclusive_or_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Bit_Or}))
    {
        auto rhs = parse_exclusive_or_exp(parser);
        CHECK_OR_FAIL(rhs);
        lhs = uptr<Ast_Node>(new Inclusive_Or_Node(lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_exclusive_or_exp(Parser &parser)
{   // exclusive_or :=
    //     and
    //     exclusive_or ^ and
    SAVE;
    auto lhs = parse_and_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Bit_Xor}))
    {
        auto rhs = parse_and_exp(parser);
        CHECK_OR_FAIL(rhs);
        lhs = uptr<Ast_Node>(new Exclusive_Or_Node(lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_and_exp(Parser &parser)
{   // and :=
    //     equality
    //     and & equality
    SAVE;
    auto lhs = parse_equality_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Bit_And}))
    {
        auto rhs = parse_equality_exp(parser);
        CHECK_OR_FAIL(rhs);
        lhs = uptr<Ast_Node>(new And_Node(lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_equality_exp(Parser &parser)
{   // equality :=
    //     relational
    //     equality == relational
    //     equality != relational
    SAVE;
    auto lhs = parse_relational_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Equals_Equals, Token_Id::Not_Equals}))
    {
        auto rhs = parse_relational_exp(parser);
        CHECK_OR_FAIL(rhs);
        if (tok.id() == Token_Id::Equals_Equals)
        {
            lhs = uptr<Ast_Node>(new Equals_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Not_Equals)
        {
            lhs = uptr<Ast_Node>(new Not_Equals_Node(lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_relational_exp(Parser &parser)
{   // relational :=
    //     shift
    //     relational <  shift
    //     relational <= shift
    //     relational >  shift
    //     relational >= shift
    SAVE;
    auto lhs = parse_shift_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Less,    Token_Id::Less_Equals,
                               Token_Id::Greater, Token_Id::Greater_Equals}))
    {
        auto rhs = parse_shift_exp(parser);
        CHECK_OR_FAIL(rhs);
        if (tok.id() == Token_Id::Less)
        {
            lhs = uptr<Ast_Node>(new Less_Than_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Less_Equals)
        {
            lhs = uptr<Ast_Node>(new Less_Than_Equals_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Greater)
        {
            lhs = uptr<Ast_Node>(new Greater_Than_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Greater_Equals)
        {
            lhs = uptr<Ast_Node>(new Greater_Than_Equals_Node(lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_shift_exp(Parser &parser)
{   // shift :=
    //     additive
    //     shift << additive
    //     shift >> additive
    SAVE;
    auto lhs = parse_additive_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::L_Shift, Token_Id::R_Shift}))
    {
        auto rhs = parse_additive_exp(parser);
        CHECK_OR_FAIL(rhs);
        if (tok.id() == Token_Id::L_Shift)
        {
            lhs = uptr<Ast_Node>(new Left_Shift_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::R_Shift)
        {
            lhs = uptr<Ast_Node>(new Right_Shift_Node(lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_additive_exp(Parser &parser)
{   // additive :=
    //     multiplicative
    //     additive + multiplicative
    //     additive - multiplicative
    SAVE;
    auto lhs = parse_multiplicative_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Plus, Token_Id::Minus}))
    {
        auto rhs = parse_multiplicative_exp(parser);
        CHECK_OR_FAIL(rhs);
        if (tok.id() == Token_Id::Plus)
        {
            DEBUG(tok, "Add");
            lhs = uptr<Ast_Node>(new Add_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Minus)
        {
            DEBUG(tok, "Subtract");
            lhs = uptr<Ast_Node>(new Subtract_Node(lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_multiplicative_exp(Parser &parser)
{   // multiplicative :=
    //     unary
    //     multiplicative * unary
    //     multiplicative / unary
    //     multiplicative % unary
    SAVE;
    auto lhs = parse_unary_exp(parser);
    CHECK_OR_FAIL(lhs);
    Token tok;
    while (parser.accept(tok, {Token_Id::Star, Token_Id::Slash, Token_Id::Mod}))
    {
        auto rhs = parse_unary_exp(parser);
        CHECK_OR_FAIL(rhs);
        if (tok.id() == Token_Id::Star)
        {
            DEBUG(tok, "Multiply");
            lhs = uptr<Ast_Node>(new Multiply_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Slash)
        {
            DEBUG(tok, "Divide");
            lhs = uptr<Ast_Node>(new Divide_Node(lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Mod)
        {
            DEBUG(tok, "Modulo");
            lhs = uptr<Ast_Node>(new Modulo_Node(lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}

static uptr<Ast_Node> parse_unary_exp(Parser &parser)
{   // unary :=
    //     postfix
    //     - unary
    //     + unary
   //     ! unary
    //     ~ unary
    SAVE;
    uptr<Ast_Node> postfix = parse_postfix_exp(parser);
    if (postfix)
    {
        return postfix;
    }
    RESTORE;
    Token tok;
    if (parser.accept(tok, {Token_Id::Minus, Token_Id::Plus,
                    Token_Id::Invert, Token_Id::Not}))
    {
        auto unary = parse_unary_exp(parser);
        if (tok.id() == Token_Id::Minus)
        {
            DEBUG(tok, "Negate");
            return uptr<Ast_Node>(new Negate_Node(unary.release()));
        }
        if (tok.id() == Token_Id::Plus)
        {
            DEBUG(tok, "Positive");
            return uptr<Ast_Node>(new Positive_Node(unary.release()));
        }
        if (tok.id() == Token_Id::Not)
        {
            DEBUG(tok, "Not");
            return uptr<Ast_Node>(new Not_Node(unary.release()));
        }
        if (tok.id() == Token_Id::Invert)
        {
            DEBUG(tok, "Invert");
            return uptr<Ast_Node>(new Invert_Node(unary.release()));
        }
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_postfix_exp(Parser &parser)
{   // postfix :=
    //     primary
    //     postfix ( )
    //     postfix ( expression_list )
    //     postfix [ expression_list ]
    //     postfix . identifier
    SAVE;
    auto prim = parse_primary(parser);
    Token tok;
    if (parser.accept(tok, {Token_Id::Open_Paren}))
    {
        auto args = parse_expression_list(parser);
        if (!parser.expect(tok, Token_Id::Close_Paren))
        {
            PARSE_FAIL;
        }
        return uptr<Ast_Node>(new Call_Node(prim.release(), args.release()));
    }
    if (parser.accept(tok, {Token_Id::Open_Square}))
    {
        auto args = parse_expression_list(parser);
        if (!parser.expect(tok, Token_Id::Close_Square))
        {
            PARSE_FAIL;
        }
        return uptr<Ast_Node>(new Index_Node(prim.release(), args.release()));
    }
    //if (parser.accept(tok, {Token_Id::Dot}))
    //{
    //    
    //}
    if (prim)
    {
        return prim;
    }
    PARSE_FAIL;
}
static uptr<Ast_Node> parse_primary(Parser &parser)
{
    SAVE;
    Token token;
    if (parser.accept(token, { Token_Id::Open_Paren }))
    {
        auto expr = parse_expression(parser);
        if (!parser.expect(token, Token_Id::Close_Paren))
        {
            PARSE_FAIL;
        }
        if (expr)
        {
            DEBUG(token, "%s", expr->node_name().c_str());
            return expr;
        }
        PARSE_FAIL;
    }
    if (parser.accept(token, { Token_Id::Integer }))
    {
        return uptr<Ast_Node>(new Integer_Node(token.to_int()));
    }
    if (parser.accept(token, { Token_Id::Real }))
    {
        return uptr<Ast_Node>(new Real_Node(token.to_real()));
    }
    if (parser.accept(token, { Token_Id::Identifier }))
    {
        return uptr<Ast_Node>(new Variable_Node(token.to_string()));
    }
    //if (auto tk = parser.peek())
    //{
    //    parser.report_error(*tk, "Unexpected %s\n", tk->debug().c_str());
    //}
    PARSE_FAIL;
}
static uptr<List_Node> parse_expression_list(Parser &parser)
{   // expression_list :=
    //     expression
    //     expression_list , expression
    std::vector<Ast_Node*> contents;
    while (true)
    {
        auto x = parse_expression(parser);
        if (!x)
        {
            if (contents.size() > 0)
            {
                auto &prev_tk = parser.lexer.tokens[parser.lex_idx-1];
                parser.report_error(prev_tk, "Unexpected token %s in expression list", prev_tk.debug().c_str());
            }
            break; // an empty list is a valid list
        }
        contents.push_back(x.release());
        if (!parser.accept({Token_Id::Comma}))
        {
            break; // implicitly breaks on close braces: ) ] > }
        }
    }
    return uptr<List_Node>(new List_Node(contents));
}
static uptr<Ast_Node> parse_expression(Parser &parser)
{   // expression :=
    //     function
    //     l_or
    auto tk = parser.peek();
    if (!tk)
    {
        return nullptr;
    }
    if (tk->id() == Token_Id::K_fn)
    {
        return parse_fn(parser);
    }
    return parse_logical_or_exp(parser);
}
static bool parse_body(Parser &parser, std::vector<Ast_Node*> &body)
{
    SAVE;
    if (!parser.expect(Token_Id::Open_Curly))
    {
        RESTORE;
        return false;
    }
    
    while (true)
    {
        auto stmt = parse_statement(parser);
        if (stmt)
        {
            body.push_back(stmt.release());
        }
        if (parser.accept({Token_Id::Close_Curly}))
        {
            return true;
        }
    }
}
static uptr<Ast_Node> parse_fn(Parser &parser)
{   // function :=
    //     fn ( ) -> identifier { body }
    //     fn ( decl_list ) -> identifier { body }
    SAVE;
    ACCEPT_OR_FAIL({Token_Id::K_fn});
    CHECK_OR_FAIL(parser.expect(Token_Id::Open_Paren));
    CHECK_OR_FAIL(parser.expect(Token_Id::Close_Paren));
    CHECK_OR_FAIL(parser.expect(Token_Id::Right_Arrow));
    Token return_type;
    CHECK_OR_FAIL(parser.expect(return_type, Token_Id::Identifier));
    std::vector<Ast_Node*> body;
    CHECK_OR_FAIL(parse_body(parser, body));
    return uptr<Ast_Node>(new Fn_Node({}, new Variable_Node(return_type.to_string()), body));
}
static uptr<Ast_Node> parse_statement(Parser &parser)
{   // statement :=
    //     declaration statement
    //     assignment statement
    //     expression statement
    //     definition statement

    //while (parser.accept({Token_Id::Semicolon}))
    //{
    //    // eat stray semicolons
    //}
    SAVE;
    if (auto stmt = parse_assignment(parser))
    {
        //CHECK_OR_FAIL(parser.expect(Token_Id::Semicolon));
        parser.accept({Token_Id::Semicolon});
        return stmt;
    }
    if (auto stmt = parse_declaration(parser))
    {
        //CHECK_OR_FAIL(parser.expect(Token_Id::Semicolon));
        parser.accept({Token_Id::Semicolon});
        return stmt;
    }
    if (auto stmt = parse_expression(parser))
    {
        //CHECK_OR_FAIL(parser.expect(Token_Id::Semicolon));
        parser.accept({Token_Id::Semicolon});
        return stmt;
    }
    PARSE_FAIL;
}
static Ast_Node *parse_top_level(Parser &parser)
{
    // top-level :=
    //     <nothing>
    //     statement top-level
    SAVE;
    auto top = parse_statement(parser);
    if (top.get())
    {
        return top.release();
    }
    PARSE_FAIL;
}
