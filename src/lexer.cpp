#include <sstream>
#include <stdarg.h>
#include "lexer.hpp"

double Token::to_real() const
{
    return ::atof(m_string.c_str());
}

int64_t Token::to_int() const
{
    return ::atoi(m_string.c_str());
}

std::string Token::to_string() const
{
    return m_string;
}

Token_Id Token::id() const
{
    return m_id;
}

std::string Token::debug() const
{
    std::stringstream ss;
    ss << ::to_string(m_id) << " (" << m_string << ")";
    return ss.str();
}

Source_Location Token::src_loc() const
{
    return m_src_loc;
}

static const char *tk_type_map[static_cast<size_t>(Token_Id::NUM_TOKEN_TYPES)];
static void init_tk_type_map()
{
    static bool once = true;
    if (once)
    {
        once = false;
#define SET_TK_TYPE_MAP(a) tk_type_map[static_cast<size_t>(Token_Id::a)] = #a
        SET_TK_TYPE_MAP(Invalid);
        SET_TK_TYPE_MAP(Integer);
        SET_TK_TYPE_MAP(Real);
        SET_TK_TYPE_MAP(String);
        SET_TK_TYPE_MAP(Equals);
        SET_TK_TYPE_MAP(Colon);
        SET_TK_TYPE_MAP(Semicolon);
        SET_TK_TYPE_MAP(Open_Paren);
        SET_TK_TYPE_MAP(Close_Paren);
        SET_TK_TYPE_MAP(Open_Square);
        SET_TK_TYPE_MAP(Close_Square);
        SET_TK_TYPE_MAP(Open_Curly);
        SET_TK_TYPE_MAP(Close_Curly);
        SET_TK_TYPE_MAP(Comma);
        SET_TK_TYPE_MAP(Plus);
        SET_TK_TYPE_MAP(Minus);
        SET_TK_TYPE_MAP(Star);
        SET_TK_TYPE_MAP(Slash);
        SET_TK_TYPE_MAP(Equals_Equals);
        SET_TK_TYPE_MAP(Not_Equals);
        SET_TK_TYPE_MAP(Less);
        SET_TK_TYPE_MAP(Less_Equals);
        SET_TK_TYPE_MAP(Greater);
        SET_TK_TYPE_MAP(Greater_Equals);
        SET_TK_TYPE_MAP(Identifier);
        SET_TK_TYPE_MAP(Dot);
        SET_TK_TYPE_MAP(Mod);
        SET_TK_TYPE_MAP(Right_Arrow);
        SET_TK_TYPE_MAP(L_Shift);
        SET_TK_TYPE_MAP(R_Shift);
        SET_TK_TYPE_MAP(Compare);
        SET_TK_TYPE_MAP(Bit_And);
        SET_TK_TYPE_MAP(Bit_Xor);
        SET_TK_TYPE_MAP(Bit_Or);
        SET_TK_TYPE_MAP(Log_And);
        SET_TK_TYPE_MAP(Log_Or);
        SET_TK_TYPE_MAP(Invert);
        SET_TK_TYPE_MAP(Not);
        SET_TK_TYPE_MAP(K_b_op);
        SET_TK_TYPE_MAP(K_u_op);
        SET_TK_TYPE_MAP(K_proc);
        SET_TK_TYPE_MAP(K_for);
        SET_TK_TYPE_MAP(K_while);
        SET_TK_TYPE_MAP(K_return);
        SET_TK_TYPE_MAP(K_if);
        SET_TK_TYPE_MAP(K_else);
        SET_TK_TYPE_MAP(K_fn);
#undef SET_TK_TYPE_MAP
        for (size_t i = 0; i < static_cast<size_t>(Token_Id::NUM_TOKEN_TYPES); ++i)
        {
            if (tk_type_map[i] == nullptr)
            {
                printf("forgot to set tk type map index = %li!!\n", i);
            }
        }
    }
}

std::string to_string(Token_Id tk_type)
{
    init_tk_type_map();
    if (tk_type >= Token_Id::Invalid && tk_type < Token_Id::NUM_TOKEN_TYPES)
    {
        if (tk_type_map[static_cast<size_t>(tk_type)])
        {
            return tk_type_map[static_cast<size_t>(tk_type)];
        }
        return "NOT_SET";
    }
    return tk_type_map[static_cast<size_t>(Token_Id::Invalid)];
}

void Lexer::dump()
{
    printf("=======lex dump=======\n");
    for (auto &&tk : tokens)
    {
        printf("%s\n", tk.debug().c_str());
    }
    printf("======================\n");
}

bool Lexer::lex(Source_Code &src)
{
    init_tk_type_map();
    tokens.clear();
    while (src.peek() != src.end_of_file)
    {
        if (is_wspace(src.peek()))
        {
            src.next();
            continue;
        }
        if (is_digit(src.peek()))
        {
            std::stringstream num;
            auto line = src.line();
            auto line_char = src.line_char();
            bool is_real = false;
            while (src.peek() != src.end_of_file)
            {
                if (is_digit(src.peek()))
                {
                    num << (char)src.next();
                    continue;
                }
                if (src.peek() == '.')
                {
                    // valid: 123.stuff
                    if (!is_digit(src.peek(1))) { break; }
                    // valid: 123.4.stuff
                    if (is_real) { break; }
                    is_real = true;
                    num << (char)src.next();
                    continue;
                }
                if (is_ident_char(src.peek()))
                {
                    // invalid: 123a
                    src.report_at_src_loc("error", src.curr_src_loc(), "Unexpected character '%c' while parsing number\n", src.peek());
                    return false;
                }
                // breaks on non-digits and non-ident chars
                break;
            }
            if (is_real)
            {
                tokens.push_back(Token(Token_Id::Real, num.str(), {src.filename(), line, line_char}));
            }
            else
            {
                tokens.push_back(Token(Token_Id::Integer, num.str(), {src.filename(), line, line_char}));
            }
            continue;
        }
#define PUSH_KEY_IDENT(str, id) \
        if (match_ident(src, (str))) \
        { \
            tokens.push_back(Token(Token_Id::id, (str), src.curr_src_loc())); \
            src.advance(sizeof(str) - 1); \
            continue; \
        }
        PUSH_KEY_IDENT("b_op", K_b_op);
        PUSH_KEY_IDENT("u_op", K_u_op);
        PUSH_KEY_IDENT("proc", K_proc);
        PUSH_KEY_IDENT("for", K_for);
        PUSH_KEY_IDENT("while", K_while);
        PUSH_KEY_IDENT("return", K_return);
        PUSH_KEY_IDENT("fn", K_fn);
        PUSH_KEY_IDENT("if", K_if);
        PUSH_KEY_IDENT("else", K_else);
        if (is_ident_start_char(src.peek()))
        {
            std::stringstream ident;
            auto line = src.line();
            auto line_char = src.line_char();
            while (is_ident_char(src.peek()))
            {
                ident << (char)src.next();
            }
            tokens.push_back(Token(Token_Id::Identifier, ident.str(), {src.filename(), line, line_char}));
            continue;
        }
#define PUSH_TOKEN_BODY(str, id) \
{ \
tokens.push_back(Token(Token_Id::id, (str), src.curr_src_loc())); \
for (size_t i = 0; i < sizeof(str)-1; ++i) { src.next(); } \
continue; \
}
        // @FixMe: this needs lookahead after the token because >=< will parse into [Greater_Equals, Less]
#define PUSH_TOKEN_1C(str, id) if (src.peek() == (str)[0]) PUSH_TOKEN_BODY(str, id)
#define PUSH_TOKEN_2C(str, id) if (src.peek() == (str)[0] && src.peek(1) == (str)[1]) PUSH_TOKEN_BODY(str, id)
#define PUSH_TOKEN_3C(str, id) if (src.peek() == (str)[0] && src.peek(1) == (str)[1] && src.peek(2) == (str[2])) PUSH_TOKEN_BODY(str, id)

        PUSH_TOKEN_3C("<=>", Compare);

        PUSH_TOKEN_2C("->", Right_Arrow);
        PUSH_TOKEN_2C("<<", L_Shift);
        PUSH_TOKEN_2C(">>", R_Shift);
        PUSH_TOKEN_2C("==", Equals_Equals);
        PUSH_TOKEN_2C("!=", Not_Equals);
        PUSH_TOKEN_2C("<=", Less_Equals);
        PUSH_TOKEN_2C(">=", Greater_Equals);
        PUSH_TOKEN_2C("||", Log_Or);
        PUSH_TOKEN_2C("&&", Log_And);

        PUSH_TOKEN_1C("<", Less);
        PUSH_TOKEN_1C(">", Greater);
        PUSH_TOKEN_1C("^", Bit_Xor);
        PUSH_TOKEN_1C("|", Bit_Or);
        PUSH_TOKEN_1C("&", Bit_And);
        PUSH_TOKEN_1C("~", Invert);
        PUSH_TOKEN_1C("!", Not);
        PUSH_TOKEN_1C("%", Mod);
        PUSH_TOKEN_1C("=", Equals);
        PUSH_TOKEN_1C(":", Colon);
        PUSH_TOKEN_1C(";", Semicolon);
        PUSH_TOKEN_1C("(", Open_Paren);
        PUSH_TOKEN_1C(")", Close_Paren);
        PUSH_TOKEN_1C("[", Open_Square);
        PUSH_TOKEN_1C("]", Close_Square);
        PUSH_TOKEN_1C("{", Open_Curly);
        PUSH_TOKEN_1C("}", Close_Curly);
        PUSH_TOKEN_1C(",", Comma);
        PUSH_TOKEN_1C("+", Plus);
        PUSH_TOKEN_1C("-", Minus);
        PUSH_TOKEN_1C("*", Star);
        PUSH_TOKEN_1C("/", Slash);
        PUSH_TOKEN_1C(".", Dot);

        src.report_at_src_loc("error", src.curr_src_loc(), "Unexpected character '%c'\n", src.peek());
        return false;
    }
    return true;
}

bool Lexer::match_ident(Source_Code &code, const std::string &ident)
{
    for (size_t i = 0; i < ident.size(); ++i)
    {
        if (code.peek(i) != ident[i])
        {
            return false;
        }
    }
    if (is_ident_char(code.peek(ident.size())))
    {
        return false;
    }
    return true;
}

bool Lexer::is_ident_start_char(int c)
{
    return isalpha(c) || c == '_';
}

bool Lexer::is_ident_char(int c)
{
    return is_ident_start_char(c) || is_digit(c);
}

bool Lexer::is_digit(int c)
{
    return isdigit(c);
}

bool Lexer::is_wspace(int c)
{
    return isspace(c);
}
