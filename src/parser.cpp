#include <memory>
#include <stdarg.h>

#include "parser.hpp"
#include "ast/nodes.hpp"

const Token *Parser::peek(int n) const
{
    if (lex_idx + n >= (int)lexer.tokens.size())
    {
        return nullptr;
    }
    if (lex_idx + n < 0)
    {
        return 0;
    }
    return &lexer.tokens[lex_idx];
}

Token_Id Parser::peek_id(int n) const
{
    auto tk = peek(n);
    return tk ? tk->id() : Token_Id::Invalid;
}

void Parser::skip(int n)
{
    lex_idx += n;
}

bool Parser::accept(Token &out, const std::vector<Token_Id> &ids)
{
    if (lex_idx >= (int)lexer.tokens.size())
    {
        return false;
    }
    for (auto &&id : ids)
    {
        if (id == peek_id())//lexer.tokens[lex_idx].id())
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
    if (lex_idx >= (int)lexer.tokens.size())
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
    token.src_loc().vreport("error", fmt, vargs);
    va_end(vargs);
}

void Parser::report_debug(const Token &token, const char *fmt, ...) const
{
    va_list vargs;
    va_start(vargs, fmt);
    token.src_loc().vreport("debug", fmt, vargs);
    va_end(vargs);
}

// forward decls
/*
 * This may look odd because this is the only place in this codebase that uses unique_ptr
 * but that is because an AST node may be partially contructed by the time an error is seen
 * or the parser has decided the current parse doesn't match. This is abused by wrapping
 * most of the parsing assertions in macros that automatically report on error or fail fast.
 * This is where the unique_ptr becomes useful as it will automatically delete any partially
 * constructed nodes.
 */
template<typename... T>
using uptr = std::unique_ptr<T...>;
static uptr<Assign_Node> parse_assignment(Parser &parser);
static uptr<Return_Node> parse_return(Parser &parser);
static uptr<Break_Node> parse_break(Parser &parser);
static uptr<Continue_Node> parse_continue(Parser &parser);
static uptr<While_Node> parse_while(Parser &parser);
static uptr<Decl_Node> parse_declaration(Parser &parser, bool type_required = true);
static uptr<Decl_Assign_Node> parse_decl_assign(Parser &parser);
static uptr<Decl_Constant_Node> parse_decl_constant(Parser &parser);
static uptr<Array_Literal_Node> parse_array_literal(Parser &parser);
static uptr<New_Array_Node> parse_new_array(Parser &parser);
static uptr<Ast_Value> parse_logical_or_exp(Parser &parser);
static uptr<Ast_Value> parse_logical_and_exp(Parser &parser);
static uptr<Ast_Value> parse_inclusive_or_exp(Parser &parser);
static uptr<Ast_Value> parse_exclusive_or_exp(Parser &parser);
static uptr<Ast_Value> parse_and_exp(Parser &parser);
static uptr<Ast_Value> parse_equality_exp(Parser &parser);
static uptr<Ast_Value> parse_relational_exp(Parser &parser);
static uptr<Ast_Value> parse_shift_exp(Parser &parser);
static uptr<Ast_Value> parse_additive_exp(Parser &parser);
static uptr<Ast_Value> parse_multiplicative_exp(Parser &parser);
static uptr<Ast_Value> parse_unary_exp(Parser &parser);
static uptr<Ast_Value> parse_postfix_exp(Parser &parser);
static uptr<Ast_Value> parse_primary(Parser &parser);
static uptr<List_Node> parse_expression_list(Parser &parser, const Source_Location &src_loc);
static uptr<Ast_Value> parse_expression(Parser &parser);
static uptr<Type_Node> parse_type(Parser &parser);
static uptr<Ast_Node> parse_statement(Parser &parser);
static bool parse_body(Parser &parser, std::vector<Ast_Node*> &body);
static uptr<Fn_Node> parse_fn(Parser &parser);
static uptr<Constructor_Node> parse_ctor(Parser &parser);
static uptr<Extend_Node> parse_extend(Parser &parser);
static uptr<Type_Def_Node> parse_type_definition(Parser &parser);

#define SAVE                                        \
    auto _save_idx = parser.lex_idx;                \
    auto _save_is_extending = parser.is_extending
    
#define RESTORE                                                 \
    do {parser.lex_idx = _save_idx;                             \
        parser.is_extending = _save_is_extending;} while (0)
    
#define PARSE_FAIL {RESTORE; return nullptr;}
#define ACCEPT_OR_FAIL(...) if (!parser.accept(__VA_ARGS__)) { PARSE_FAIL; }
#define CHECK_OR_FAIL(check) if (!(check)) PARSE_FAIL;
#define CHECK_OR_ERROR(check, ...) if (!(check)) { parser.report_error(__VA_ARGS__); PARSE_FAIL; }
//#define DPRINT(tk, ...) parser.report_debug(tk, __VA_ARGS__)
#define DPRINT(tk, ...)

static uptr<Assign_Node> parse_assignment(Parser &parser)
{   // assignment :=
    //     lvalue = expression
    SAVE;
    auto lhs = parse_expression(parser);
    CHECK_OR_FAIL(lhs);
    Token ass_tk;
    ACCEPT_OR_FAIL(ass_tk, {
            Token_Id::Equals,                 // =
                Token_Id::Plus_Equals,        // +=
                Token_Id::Minus_Equals,       // -=
                Token_Id::Star_Equals,        // *=
                Token_Id::Slash_Equals,       // /=
                Token_Id::Mod_Equals,         // %=
                Token_Id::L_Shift_Equals,     // <<=
                Token_Id::R_Shift_Equals,     // >>=
                Token_Id::Bit_And_Equals,     // &=
                Token_Id::Bit_Xor_Equals,     // ^=
                Token_Id::Bit_Or_Equals,      // |=
                });
    CHECK_OR_ERROR(lhs->can_lvalue(), ass_tk, "LHS of assignment is not an lvalue.");
    if (ass_tk.id() == Token_Id::Equals)
    {   // trivial case
        auto rhs = parse_expression(parser);
        CHECK_OR_FAIL(rhs);
        return uptr<Assign_Node>(new Assign_Node(ass_tk.src_loc(),
                                                static_cast<Ast_LValue*>(lhs.release()),
                                                rhs.release()));
    }
    else
    {
        // This looks very odd, but it must be done until Ast_Node mandates some sort of
        // .deep_copy() method.
        // The mistaken approach to this is to create the operation expression and use the
        // LHS of the assignment also as the LHS of the binary operation. This is a bug
        // because every AST node owns any nodes it can directly reach which upon freeing
        // the AST would cause the LHS of this expression to be freed twice.
        RESTORE;
        auto lhs_copy = parse_expression(parser);
        parser.skip(); // skip the assignment operator we already parsed
        auto rhs = parse_expression(parser);
        CHECK_OR_FAIL(rhs);

        Binary_Holder *rhs_of_assignment = nullptr;
        switch (ass_tk.id())
        {
            default:
                parser.report_error(ass_tk, "impossible assignment operator %s (%s)",
                             to_string(ass_tk.id()).c_str(), ass_tk.to_string().c_str());
                abort();
                case Token_Id::Plus_Equals:
                    rhs_of_assignment = new Add_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::Minus_Equals:
                    rhs_of_assignment = new Subtract_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::Star_Equals:
                    rhs_of_assignment = new Multiply_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::Slash_Equals:
                    rhs_of_assignment = new Divide_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::Mod_Equals:
                    rhs_of_assignment = new Modulo_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::L_Shift_Equals:
                    rhs_of_assignment = new Left_Shift_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::R_Shift_Equals:
                    rhs_of_assignment = new Right_Shift_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::Bit_And_Equals:
                    rhs_of_assignment = new And_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::Bit_Xor_Equals:
                    rhs_of_assignment = new Exclusive_Or_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
                case Token_Id::Bit_Or_Equals:
                    rhs_of_assignment = new Inclusive_Or_Node{
                        ass_tk.src_loc(), lhs_copy.release(), rhs.release()};
                    break;
        }
        CHECK_OR_FAIL(rhs_of_assignment);
        return uptr<Assign_Node>(new Assign_Node(ass_tk.src_loc(),
                                                static_cast<Ast_LValue*>(lhs.release()),
                                                rhs_of_assignment));
    }
}
static uptr<Return_Node> parse_return(Parser &parser)
{   // return :=
    //     return
    //     return expression_list
    SAVE;
    Token retn_tk;
    ACCEPT_OR_FAIL(retn_tk, { Token_Id::K_return });
    auto values = parse_expression_list(parser, retn_tk.src_loc());
    return uptr<Return_Node>(new Return_Node{retn_tk.src_loc(), values.release()});
}
static uptr<Break_Node> parse_break(Parser &parser)
{   // break :=
    //     break
    //     break expression_list
    SAVE;
    Token break_tk;
    ACCEPT_OR_FAIL(break_tk, { Token_Id::K_break });
    auto values = parse_expression_list(parser, break_tk.src_loc());
    return uptr<Break_Node>(new Break_Node{break_tk.src_loc(), values.release()});
}
static uptr<Continue_Node> parse_continue(Parser &parser)
{   // continue :=
    //     continue
    SAVE;
    Token continue_tk;
    ACCEPT_OR_FAIL(continue_tk, { Token_Id::K_continue });
    return uptr<Continue_Node>(new Continue_Node{continue_tk.src_loc()});
}
static uptr<If_Else_Node> parse_if_else(Parser &parser)
{
    SAVE;
    Token if_tk;
    ACCEPT_OR_FAIL(if_tk, { Token_Id::K_if });
    auto condition = parse_expression(parser);
    CHECK_OR_ERROR(condition, if_tk, "Couldn't parse if conditional");
    std::vector<Ast_Node*> consequence, alternative;
    if (parser.peek_id() == Token_Id::Open_Curly)
    {
        CHECK_OR_FAIL(parse_body(parser, consequence));
    }
    else
    {
        auto single = parse_statement(parser);
        if (single)
        {
            consequence.push_back(single.release());
        }
        else
        {
            // empty if leg
        }
    }

    if (parser.accept({Token_Id::K_else}))
    {
        if (parser.peek_id() == Token_Id::K_if)
        {
            auto single = parse_if_else(parser);
            CHECK_OR_FAIL(single);
            alternative.push_back(single.release());
        }
        else if (parser.peek_id() == Token_Id::Open_Curly)
        {
            CHECK_OR_FAIL(parse_body(parser, alternative));
        }
        else
        {
            auto single = parse_statement(parser);
            CHECK_OR_FAIL(single);
            alternative.push_back(single.release());
        }
    }
    return uptr<If_Else_Node>(new If_Else_Node{if_tk.src_loc(), condition.release(), consequence, alternative, parser.types->get_void()});
}
static uptr<While_Node> parse_while(Parser &parser)
{   // while :=
    //     while expression { body }
    SAVE;
    Token while_tk;
    ACCEPT_OR_FAIL(while_tk, { Token_Id::K_while});
    auto condition = parse_expression(parser);
    std::vector<Ast_Node*> body;
    if (parser.peek_id() == Token_Id::Open_Curly)
    {
        CHECK_OR_FAIL(parse_body(parser, body));
    }
    else
    {
        auto single = parse_statement(parser);
        CHECK_OR_FAIL(single);
        body.push_back(single.release());
    }
    return uptr<While_Node>(new While_Node{while_tk.src_loc(), condition.release(), body});
}
static uptr<For_Node> parse_for(Parser &parser)
{   // for :=
    //     for expression { body }
    SAVE;
    Token for_tk;
    ACCEPT_OR_FAIL(for_tk, { Token_Id::K_for});
    auto iterable = parse_expression(parser);
    std::vector<Ast_Node*> body;
    if (parser.peek_id() == Token_Id::Open_Curly)
    {
        CHECK_OR_FAIL(parse_body(parser, body));
    }
    else
    {
        auto single = parse_statement(parser);
        CHECK_OR_FAIL(single);
        body.push_back(single.release());
    }
    return uptr<For_Node>(new For_Node{for_tk.src_loc(), iterable.release(), body});
}
static uptr<Decl_Node> parse_declaration(Parser &parser, bool type_required)
{   // decl :=
    //     identifier : type
    //     identifier : 
    SAVE;
    Token ident, type_name;
    ACCEPT_OR_FAIL(ident, { Token_Id::Identifier });
    Token colon;
    ACCEPT_OR_FAIL(colon, { Token_Id::Colon });
    auto type = parse_type(parser);
    // Normally I would opt to default this to true, however function parameters are parsed
    // with this and don't want to be readonly.
    constexpr bool is_readonly = false;
    if (type)
    {
        return uptr<Decl_Node>(
            new Decl_Node(ident.src_loc(), ident.to_string(), type.release(), is_readonly));
    }
    if (type_required)
    {
        parser.report_error(colon, "Type specifier required here");
        return nullptr;
    }
    return uptr<Decl_Node>(
        new Decl_Node(ident.src_loc(), ident.to_string(), nullptr, is_readonly));
}
static uptr<Decl_Assign_Node> parse_decl_assign(Parser &parser)
{   // decl_assign :=
    //     decl = value
    SAVE;
    auto decl = parse_declaration(parser, false);
    CHECK_OR_FAIL(decl);
    decl->is_readonly = false;
    Token tk_equals;
    ACCEPT_OR_FAIL(tk_equals, {Token_Id::Equals});
    auto value = parse_expression(parser);
    if (!value)
    {
        parser.report_error(tk_equals, "Expected expression on right hand side of declaration assignment");
        return nullptr;
    }
    if (!decl->type)
    {
        auto val_ty = value->get_type();
        if (val_ty)
        {
            decl->type = new Type_Node(decl->src_loc, val_ty);
        }
    }
    return uptr<Decl_Assign_Node>(
        new Decl_Assign_Node(decl->src_loc, decl.release(), value.release()));
}
static uptr<Decl_Constant_Node> parse_decl_constant(Parser &parser)
{   // decl_constant :=
    //     decl : value
    SAVE;
    auto decl = parse_declaration(parser, false);
    CHECK_OR_FAIL(decl);
    decl->is_readonly = true;
    Token tk_colon;
    ACCEPT_OR_FAIL(tk_colon, {Token_Id::Colon});
    auto value = parse_expression(parser);
    if (!value)
    {
        parser.report_error(tk_colon, "Expected expression on right hand side of constant assignment");
        return nullptr;
    }
    if (!decl->type)
    {
        auto val_ty = value->get_type();
        if (val_ty)
        {
            decl->type = new Type_Node(decl->src_loc, val_ty);
        }
    }
    return uptr<Decl_Constant_Node>(new Decl_Constant_Node(decl->src_loc,
                                                           decl.release(),
                                                           value.release()));
}
static uptr<Array_Literal_Node> parse_array_literal(Parser &parser)
{   // array_literal :=
    //     [ expression_list ]
    SAVE;
    Token open_bracket_tk;
    ACCEPT_OR_FAIL(open_bracket_tk, {Token_Id::Open_Square});
    auto values = parse_expression_list(parser, open_bracket_tk.src_loc());
    CHECK_OR_FAIL(values);
    Token close_bracket_tk;
    parser.expect(close_bracket_tk, Token_Id::Close_Square);
    CHECK_OR_ERROR(!values->contents.empty(), open_bracket_tk, "Array literal cannot be empty");
    return uptr<Array_Literal_Node>(new Array_Literal_Node{open_bracket_tk.src_loc(), values.release()});
}
static uptr<New_Array_Node> parse_new_array(Parser &parser)
{   // new_array :=
    //     [ expression ] type

    SAVE;
    Token open_bracket_tk;
    ACCEPT_OR_FAIL(open_bracket_tk, {Token_Id::Open_Square});
    auto size = parse_expression(parser);
    CHECK_OR_FAIL(size);
    Token close_bracket_tk;
    ACCEPT_OR_FAIL({Token_Id::Close_Square});
    auto of_type = parse_type(parser);
    CHECK_OR_FAIL(of_type);
    auto arr_ty = parser.types->get_array_type(of_type->type);
    return uptr<New_Array_Node>(
        new New_Array_Node{open_bracket_tk.src_loc(),
                arr_ty,
                of_type.release(),
                size.release()});
}
static uptr<Ast_Value> parse_logical_or_exp(Parser &parser)
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
        lhs = uptr<Ast_Value>(new Logical_Or_Node(tok.src_loc(), lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_logical_and_exp(Parser &parser)
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
        lhs = uptr<Ast_Value>(new Logical_And_Node(tok.src_loc(), lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_inclusive_or_exp(Parser &parser)
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
        lhs = uptr<Ast_Value>(new Inclusive_Or_Node(tok.src_loc(), lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_exclusive_or_exp(Parser &parser)
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
        lhs = uptr<Ast_Value>(new Exclusive_Or_Node(tok.src_loc(), lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_and_exp(Parser &parser)
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
        lhs = uptr<Ast_Value>(new And_Node(tok.src_loc(), lhs.release(), rhs.release()));
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_equality_exp(Parser &parser)
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
            lhs = uptr<Ast_Value>(new Equals_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Not_Equals)
        {
            lhs = uptr<Ast_Value>(new Not_Equals_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_relational_exp(Parser &parser)
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
            lhs = uptr<Ast_Value>(new Less_Than_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Less_Equals)
        {
            lhs = uptr<Ast_Value>(new Less_Than_Equals_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Greater)
        {
            lhs = uptr<Ast_Value>(new Greater_Than_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Greater_Equals)
        {
            lhs = uptr<Ast_Value>(new Greater_Than_Equals_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_shift_exp(Parser &parser)
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
            lhs = uptr<Ast_Value>(new Left_Shift_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::R_Shift)
        {
            lhs = uptr<Ast_Value>(new Right_Shift_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_additive_exp(Parser &parser)
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
            DPRINT(tok, "Add");
            lhs = uptr<Ast_Value>(new Add_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Minus)
        {
            DPRINT(tok, "Subtract");
            lhs = uptr<Ast_Value>(new Subtract_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_multiplicative_exp(Parser &parser)
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
            DPRINT(tok, "Multiply");
            lhs = uptr<Ast_Value>(new Multiply_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Slash)
        {
            DPRINT(tok, "Divide");
            lhs = uptr<Ast_Value>(new Divide_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
        else if (tok.id() == Token_Id::Mod)
        {
            DPRINT(tok, "Modulo");
            lhs = uptr<Ast_Value>(new Modulo_Node(tok.src_loc(), lhs.release(), rhs.release()));
        }
    }
    if (lhs)
    {
        return lhs;
    }
    PARSE_FAIL;
}

static uptr<Ast_Value> parse_unary_exp(Parser &parser)
{   // unary :=
    //     postfix
    //     - unary
    //     + unary
   //     ! unary
    //     ~ unary
    SAVE;
    if (auto postfix = parse_postfix_exp(parser))
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
            DPRINT(tok, "Negate");
            return uptr<Ast_Value>(new Negate_Node(tok.src_loc(), unary.release()));
        }
        if (tok.id() == Token_Id::Plus)
        {
            DPRINT(tok, "Positive");
            return uptr<Ast_Value>(new Positive_Node(tok.src_loc(), unary.release()));
        }
        if (tok.id() == Token_Id::Not)
        {
            DPRINT(tok, "Not");
            return uptr<Ast_Value>(new Not_Node(tok.src_loc(), unary.release()));
        }
        if (tok.id() == Token_Id::Invert)
        {
            DPRINT(tok, "Invert");
            return uptr<Ast_Value>(new Invert_Node(tok.src_loc(), unary.release()));
        }
    }
    PARSE_FAIL;
}
static uptr<Ast_Value> parse_postfix_exp(Parser &parser)
{   // postfix :=
    //     primary
    //     postfix ( )
    //     postfix ( expression_list )
    //     postfix [ expression_list ]
    //     postfix . identifier

    SAVE;
    auto expr = parse_primary(parser);
    CHECK_OR_FAIL(expr);
    Token tok;
    while (parser.accept(tok, {Token_Id::Open_Paren, Token_Id::Open_Square, Token_Id::Dot}))
    {
        switch (tok.id())
        {
            default:
                printf("impossible %s:%d.\n", __FILE__, __LINE__);
                abort();
                break;

            case Token_Id::Open_Paren:
            {
                auto args = parse_expression_list(parser, tok.src_loc());
                CHECK_OR_FAIL(parser.expect(tok, Token_Id::Close_Paren));
                expr = uptr<Ast_Value>(new Call_Node(tok.src_loc(), expr.release(), args.release()));
            } break;
            case Token_Id::Open_Square:
            {
                auto index = parse_expression_list(parser, tok.src_loc());
                CHECK_OR_FAIL(parser.expect(tok, Token_Id::Close_Square));
                expr = uptr<Ast_Value>(new Index_Node(tok.src_loc(), expr.release(), index.release()));
            } break;
            case Token_Id::Dot:
            {
                Token ident;
                CHECK_OR_FAIL(parser.expect(ident, Token_Id::Identifier));
                auto var = new Variable_Node(ident.src_loc(), ident.to_string(), {});
                expr = uptr<Ast_Value>(new Member_Accessor_Node(tok.src_loc(), expr.release(), var));
            } break;
        }
    }
    if (expr)
    {
        return expr;
    }
    PARSE_FAIL;
}

static uptr<Variable_Node> parse_qualified_name(Parser &parser)
{   // qualified_name :=
    //     ident
    //     ident $ qualified_name
    SAVE;
    Token name, first;
    ACCEPT_OR_FAIL(name, {Token_Id::Identifier});
    first = name;
    Variable_Node::Name_Qualifiers qualifiers;
    while (parser.accept({Token_Id::Dollar}))
    {
        qualifiers.push_back(name.to_string());
        CHECK_OR_FAIL(parser.expect(name, Token_Id::Identifier));
    }
    return std::make_unique<Variable_Node>(first.src_loc(), name.to_string(), qualifiers);
}

static uptr<Ast_Value> parse_primary(Parser &parser)
{
    SAVE;
    Token token;
    if (parser.accept(token, { Token_Id::StmtTerminator }))
    {
        PARSE_FAIL;
    }
    if (parser.accept(token, { Token_Id::Open_Paren }))
    {
        auto expr = parse_expression(parser);
        if (!parser.expect(token, Token_Id::Close_Paren))
        {
            PARSE_FAIL;
        }
        if (expr)
        {
            DPRINT(token, "%s", expr->type_name().c_str());
            return expr;
        }
        PARSE_FAIL;
    }
    if (parser.peek_id() == Token_Id::K_fn)
    {
        return parse_fn(parser);
    }
    if (parser.accept(token, { Token_Id::Integer }))
    {
        return uptr<Ast_Value>(
            new Integer_Node(token.src_loc(), token.to_int(), parser.types->get_int()));
    }
    if (parser.accept(token, { Token_Id::String }))
    {
        return uptr<Ast_Value>(
            new String_Node(token.src_loc(), token.to_string(), parser.types->get_string()));
    }
    if (parser.accept(token, { Token_Id::Character }))
    {
        return uptr<Ast_Value>(
            new Character_Node(token.src_loc(), token.to_char(), parser.types->get_char()));
    }
    if (parser.accept(token, { Token_Id::Real }))
    {
        return uptr<Ast_Value>(
            new Real_Node(token.src_loc(), token.to_real(), parser.types->get_double()));
    }
    if (parser.accept(token, { Token_Id::K_true, Token_Id::K_false }))
    {
        auto value = token.id() == Token_Id::K_true;
        return uptr<Ast_Value>(
            new Boolean_Node(token.src_loc(), value, parser.types->get_bool()));
    }
    if (auto qualified_name = parse_qualified_name(parser))
    {
        return qualified_name;
    }
    //if (parser.accept(token, { Token_Id::Identifier }))
    //{
    //    return uptr<Ast_Value>(
    //        new Variable_Node(token.src_loc(), token.to_string()));
    //}
    PARSE_FAIL;
}
static uptr<List_Node> parse_expression_list(Parser &parser, const Source_Location &src_loc)
{   // expression_list :=
    //     expression
    //     expression_list , expression
    //     expression_list ,
    std::vector<Ast_Value*> contents;
    while (true)
    {
        auto x = parse_expression(parser);
        if (!x)
        {
            if (!contents.empty())
            {
                auto &prev_tk = parser.lexer.tokens[parser.lex_idx-1];
                if (prev_tk.id() != Token_Id::Comma)
                {
                    parser.report_error(prev_tk, "Unexpected token %s in expression list",
                                        prev_tk.debug().c_str());
                }
            }
            break; // an empty list is a valid list
        }
        contents.push_back(x.release());
        if (!parser.accept({Token_Id::Comma}))
        {
            break; // implicitly breaks on close braces: ) ] > }
        }
    }
    return uptr<List_Node>(new List_Node(src_loc, contents));
}
static uptr<Ast_Value> parse_expression(Parser &parser)
{   // expression :=
    //     function
    //     if_else
    //     new_array
    //     array_literal
    //     l_or
    auto tk = parser.peek();
    if (!tk)
    {
        return nullptr;
    }
    if (tk->id() == Token_Id::K_if)
    {
        return parse_if_else(parser);
    }
    if (tk->id() == Token_Id::Open_Square)
    {
        if (auto new_arr = parse_new_array(parser))
        {
            return new_arr;
        }
        return parse_array_literal(parser);
    }
    return parse_logical_or_exp(parser);
}
static uptr<Type_Node> parse_type(Parser &parser)
{   // type :=
    //     qualified_ident
    //     [ ] type
    //     [] type
    //     fn ( ) -> type
    //     fn ( type ) -> type
    //     fn ( type, type+ ) -> type
    //     @TODO: ( type, type+ ) 
    SAVE;
    Token first_tk;
    if (parser.accept(first_tk, {Token_Id::K_fn}))
    {
        std::vector<uptr<Type_Node>> params_ty_nodes;
        CHECK_OR_FAIL(parser.expect(Token_Id::Open_Paren));
        while (true)
        {
            auto p_ty_node = parse_type(parser);
            if (!p_ty_node)
            {
                break;
            }
            params_ty_nodes.push_back(std::move(p_ty_node));
            if (!parser.accept({Token_Id::Comma}))
            {
                break;
            }
        }
        CHECK_OR_FAIL(parser.expect(Token_Id::Close_Paren));
        CHECK_OR_FAIL(parser.expect(Token_Id::Right_Arrow));
        auto ret_ty_node = parse_type(parser);
        CHECK_OR_FAIL(ret_ty_node);
        std::vector<Type_Info*> params_types;
        for (auto &&n : params_ty_nodes)
        {
            params_types.push_back(n->type);
        }
        const auto is_native = false;
        auto fn_type = parser.types->declare_function(params_types, ret_ty_node->type, is_native);
        return uptr<Type_Node>(new Type_Node(first_tk.src_loc(), fn_type));
    }
    else if (parser.accept(first_tk, {Token_Id::Open_Paren}))
    {
        parser.report_error(first_tk, "Tuple type not implemented yet.");
        PARSE_FAIL;
    }
    else if (parser.accept(first_tk, {Token_Id::Op_Index_Get})) // []
    {
        auto of_ty_node = parse_type(parser);
        auto array_type = parser.types->get_array_type(of_ty_node->type);
        return uptr<Type_Node>(new Type_Node(first_tk.src_loc(), array_type));
    }
    else if (parser.accept(first_tk, {Token_Id::Open_Square})) // [
    {
        parser.expect(Token_Id::Close_Square);                 // ]
        auto of_ty_node = parse_type(parser);
        auto array_type = parser.types->get_array_type(of_ty_node->type);
        return uptr<Type_Node>(new Type_Node(first_tk.src_loc(), array_type));
    }
    else if (auto qual_name = parse_qualified_name(parser))
    {
        auto type = parser.types->get_or_declare_type(qual_name->name());
        return uptr<Type_Node>(new Type_Node(qual_name->src_loc, type));
    }
    PARSE_FAIL;
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
        if (auto stmt = parse_statement(parser))
        {
            body.push_back(stmt.release());
            continue;
        }
        break;
    }
    return parser.expect(Token_Id::Close_Curly);
}
static bool parse_decl_list(Parser &parser, std::vector<Decl_Node*> &decls)
{   // decl_list :=
    //     decl
    //     decl , decl_list
    SAVE;
    while (true)
    {
        auto last_tk = parser.peek();
        auto decl = parse_declaration(parser);
        if (!decl)
        {
            break;
        }
        if (!decl->type || !decl->type->type)
        {
            parser.report_error(*last_tk, "Declaration list expected to have types");
            RESTORE;
            return false;
        }
        decls.push_back(decl.release());
        if (!parser.accept({Token_Id::Comma}))
        {
            break;
        }
    }
    return true;
}
// This returns void* to get the compiler to stop complaining about implicit conversion of
// nullptr to bool
static void *parse_fn_shared(Parser &parser, std::vector<Decl_Node*> &params, Type_Node **ret_ty, std::vector<Ast_Node*> &body, Function_Type_Info **fn_ty)
{
    SAVE;
    CHECK_OR_FAIL(parser.expect(Token_Id::Open_Paren));
    CHECK_OR_FAIL(parse_decl_list(parser, params));
    Token close_paren_tk;
    CHECK_OR_FAIL(parser.expect(close_paren_tk, Token_Id::Close_Paren));
    *ret_ty = nullptr;
    Token arrow_tk;
    if (parser.accept(arrow_tk, {Token_Id::Right_Arrow}))
    {
        *ret_ty = parse_type(parser).release();
        CHECK_OR_ERROR(ret_ty, arrow_tk, "Expected type signature for function's return type");
    }
    else
    {
        // @Leak: if the body fails to parse this will leak.
        *ret_ty = new Type_Node(close_paren_tk.src_loc(), parser.types->get_void());
    }
    CHECK_OR_FAIL(ret_ty);
    CHECK_OR_FAIL(parse_body(parser, body));
    std::vector<Type_Info*> p_types;
    for (auto &&decl : params)
    {
        p_types.push_back(decl->type->type);
    }
    const auto is_native = false;
    *fn_ty = parser.types->declare_function(p_types, (*ret_ty)->type, is_native);
    return reinterpret_cast<void*>(1);
}
static uptr<Fn_Node> parse_bound_fn(Parser &parser)
{   // bound_function :=
    //     fn ident ( ) -> type { body }
    //     fn ident ( decl_list ) -> type { body }

    SAVE;
    Token tk_fn;
    ACCEPT_OR_FAIL(tk_fn, {Token_Id::K_fn});
    Token tk_ident;
    if (parser.is_extending) {
        ACCEPT_OR_FAIL(tk_ident, {
                Token_Id::Identifier,
                    Token_Id::Plus,
                    Token_Id::Minus,
                    Token_Id::Star,
                    Token_Id::Slash,
                    Token_Id::Equals_Equals,
                    Token_Id::Not_Equals,
                    Token_Id::Less,
                    Token_Id::Less_Equals,
                    Token_Id::Greater,
                    Token_Id::Greater_Equals,
                    Token_Id::Mod,
                    Token_Id::L_Shift,
                    Token_Id::R_Shift,
                    Token_Id::Bit_And,
                    Token_Id::Bit_Xor,
                    Token_Id::Bit_Or,
                    Token_Id::Invert,
                    Token_Id::Not,
                    Token_Id::Plus_At,
                    Token_Id::Minus_At,
                    Token_Id::Op_Index_Get,
                    Token_Id::Op_Index_Set,
                    });
    }
    else
    {
        ACCEPT_OR_FAIL(tk_ident, {Token_Id::Identifier});
    }
    std::vector<Decl_Node*> params;
    Type_Node *ret_ty;
    std::vector<Ast_Node*> body;
    Function_Type_Info *fn_ty;
    CHECK_OR_FAIL(parse_fn_shared(parser, params, &ret_ty, body, &fn_ty));
    return uptr<Fn_Node>(new Fn_Node(tk_fn.src_loc(), tk_ident.to_string(), params, ret_ty, body, fn_ty));
}
static uptr<Fn_Node> parse_fn(Parser &parser)
{   // function :=
    //     fn ( ) -> type { body }
    //     fn ( decl_list ) -> type { body }
    SAVE;
    Token tk_fn;
    ACCEPT_OR_FAIL(tk_fn, {Token_Id::K_fn});
    std::vector<Decl_Node*> params;
    Type_Node *ret_ty;
    std::vector<Ast_Node*> body;
    Function_Type_Info *fn_ty;
    CHECK_OR_FAIL(parse_fn_shared(parser, params, &ret_ty, body, &fn_ty));
    return uptr<Fn_Node>(new Fn_Node(tk_fn.src_loc(), params, ret_ty, body, fn_ty));
}
static uptr<Constructor_Node> parse_ctor(Parser &parser)
{   // ctor :=
    //     new ( ) { body }
    //     new ( decl_list ) { body }
    SAVE;
    Token tk_new;
    ACCEPT_OR_FAIL(tk_new, {Token_Id::Identifier});
    CHECK_OR_FAIL(tk_new.to_string() == "new"); // contextual keyword.
    std::vector<Decl_Node*> params;
    std::vector<Ast_Node*> body;
    Type_Node *ret_ty;
    Function_Type_Info *fn_ty;
    CHECK_OR_FAIL(parse_fn_shared(parser, params, &ret_ty, body, &fn_ty));
    if (ret_ty->type != parser.types->get_void())
    {
        parser.report_error(tk_new, "Constructors cannot have a return type.");
        delete ret_ty;
        PARSE_FAIL;
    }
    delete ret_ty; // Return type is unused but the shared code still parses for it
    return uptr<Constructor_Node>(
        new Constructor_Node(tk_new.src_loc(), params, body, fn_ty));
}
static uptr<Type_Def_Node> parse_type_definition(Parser &parser)
{
    SAVE;
    Token type_tk;
    ACCEPT_OR_FAIL(type_tk, {Token_Id::K_type});
    Token type_name_tk;
    CHECK_OR_FAIL(parser.expect(type_name_tk, Token_Id::Identifier));
    auto type = parser.types->get_or_declare_type(type_name_tk.to_string());
    auto old_extending = parser.is_extending;
    parser.is_extending = type;
    CHECK_OR_FAIL(parser.expect(Token_Id::Equals));
    CHECK_OR_FAIL(parser.expect(Token_Id::Open_Curly));
    auto type_def = uptr<Type_Def_Node>(new Type_Def_Node(type_tk.src_loc(), type));
    while (true)
    {
        parser.accept({Token_Id::StmtTerminator});
        if (auto ctor = parse_ctor(parser))
        {
            type_def->constructors.push_back(ctor.release());
            continue;
        }
        // @FixMe: ensure field names are unique
        if (auto field = parse_decl_assign(parser))
        {
            type_def->fields.push_back(field.release());
            continue;
        }
        if (auto field = parse_decl_constant(parser))
        {
            type_def->fields.push_back(field.release());
            continue;
        }
        if (auto field = parse_declaration(parser))
        {
            type_def->fields.push_back(field.release());
            continue;
        }
        if (auto method = parse_bound_fn(parser))
        {
            type_def->methods.push_back(method.release());
            continue;
        }

        // didn't parse anything, so we're done
        break;
    }
    CHECK_OR_FAIL(parser.expect(Token_Id::Close_Curly));
    parser.is_extending = old_extending;
    return type_def;
}
static uptr<Extend_Node> parse_extend(Parser &parser)
{
    SAVE;
    Token extend_tk;
    ACCEPT_OR_FAIL(extend_tk, {Token_Id::K_extend});
    auto for_type = parse_type(parser);
    CHECK_OR_ERROR(for_type, extend_tk, "Expected type to follow extend.");
    auto old_extending = parser.is_extending;
    parser.is_extending = for_type->type;
    CHECK_OR_FAIL(parser.expect(Token_Id::Open_Curly));
    Ast_Bound_Functions body;
    while (true)
    {
        parser.accept({Token_Id::StmtTerminator});
        if (auto bound_fn = parse_bound_fn(parser))
        {
            body.push_back(bound_fn.release());
            continue;
        }
        // didn't parse anything, so we're done
        break;
    }
    CHECK_OR_FAIL(parser.expect(Token_Id::Close_Curly));
    parser.is_extending = old_extending;
    return uptr<Extend_Node>(new Extend_Node(extend_tk.src_loc(), for_type.release(), body));
}
static uptr<Ast_Node> parse_statement(Parser &parser)
{   // statement :=
    //     declaration statement
    //     assignment statement
    //     expression statement
    //     definition statement

    SAVE;
    while (parser.accept({Token_Id::StmtTerminator}))
    {
        // eat stray semicolons
    }
    auto stmt_parser = [](Parser &parser) -> uptr<Ast_Node>
    {
        if (auto bound = parse_bound_fn(parser))
        {
            return bound;
        }
        if (auto _for = parse_for(parser))
        {
            return _for;
        }
        if (auto decl_ass = parse_decl_assign(parser))
        {
            return decl_ass;
        }
        if (auto decl_con = parse_decl_constant(parser))
        {
            return decl_con;
        }
        if (auto decl = parse_declaration(parser))
        {
            return decl;
        }
        if (auto assign = parse_assignment(parser))
        {
            return assign;
        }
        if (auto retn = parse_return(parser))
        {
            return retn;
        }
        if (auto brek = parse_break(parser))
        {
            return brek;
        }
        if (auto continu = parse_continue(parser))
        {
            return continu;
        }
        if (auto whil = parse_while(parser))
        {
            return whil;
        }
        if (auto expr = parse_expression(parser))
        {
            return expr;
        }
        return nullptr;
    };
    if (auto stmt = stmt_parser(parser))
    {
        parser.accept({Token_Id::StmtTerminator});
        return stmt;
    }
    PARSE_FAIL;
}


static
uptr<Import_Node> parse_import(Parser &parser)
{   // import :=
    //     import qualified_name

    SAVE;
    Token import;
    ACCEPT_OR_FAIL(import, {Token_Id::K_import});
    auto qual_name = parse_qualified_name(parser);
    CHECK_OR_FAIL(qual_name);
    auto fully_qualified_name = qual_name->qualifiers();
    fully_qualified_name.push_back(qual_name->local_name());
    CHECK_OR_FAIL(parser.expect(Token_Id::StmtTerminator));

    auto mod = parser.modules->get(fully_qualified_name);
    return std::make_unique<Import_Node>(import.src_loc(), mod);
}

void build_ast(Parser &parser, Ast &ast)
{
    // top-level :=
    //     <nothing>
    //     statement top-level
    bool is_allowed_to_import = true;
    for (;;)
    {
        while (parser.accept({Token_Id::StmtTerminator}))
        {
            // eat stray semicolons
        }
        auto tk = parser.peek();
        if (!tk)
            return;
        if (auto import = parse_import(parser))
        {
            if (!is_allowed_to_import)
            {
                parser.report_error(*tk,
                                    "Imports are only allowed before any non-import statement.");
            }
            ast.imports.push_back(import.release());
            continue;
        }
        is_allowed_to_import = false;
        if (auto type_def = parse_type_definition(parser))
        {
            ast.type_defs.push_back(type_def.release());
            continue;
        }
        if (auto extend = parse_extend(parser))
        {
            ast.extensions.push_back(extend.release());
            continue;
        }
        if (auto bound_fn = parse_bound_fn(parser))
        {
            ast.bound_funcs.push_back(bound_fn.release());
            continue;
        }
        if (auto stmt = parse_statement(parser))
        {
            ast.stmts.push_back(stmt.release());
            continue;
        }
        break;
    }
}


Ast Parser::parse(Source_Code *src_code)
{
    errors = 0;
    code = src_code;
    lexer.lex(code);
    //lexer.dump();
    is_extending = nullptr;
    lex_idx = 0;
    Ast ast;
    build_ast(*this, ast);
    return ast;
}
