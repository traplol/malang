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
        SET_TK_TYPE_MAP(StmtTerminator);
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
        SET_TK_TYPE_MAP(Plus_Equals);
        SET_TK_TYPE_MAP(Minus_Equals);
        SET_TK_TYPE_MAP(Star_Equals);
        SET_TK_TYPE_MAP(Slash_Equals);
        SET_TK_TYPE_MAP(Mod_Equals);
        SET_TK_TYPE_MAP(L_Shift_Equals);
        SET_TK_TYPE_MAP(R_Shift_Equals);
        SET_TK_TYPE_MAP(Bit_And_Equals);
        SET_TK_TYPE_MAP(Bit_Xor_Equals);
        SET_TK_TYPE_MAP(Bit_Or_Equals);
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
        SET_TK_TYPE_MAP(Plus_At);
        SET_TK_TYPE_MAP(Minus_At);
        SET_TK_TYPE_MAP(Op_Index_Get);
        SET_TK_TYPE_MAP(Op_Index_Set);
        SET_TK_TYPE_MAP(K_for);
        SET_TK_TYPE_MAP(K_while);
        SET_TK_TYPE_MAP(K_return);
        SET_TK_TYPE_MAP(K_if);
        SET_TK_TYPE_MAP(K_else);
        SET_TK_TYPE_MAP(K_fn);
        SET_TK_TYPE_MAP(K_match);
        SET_TK_TYPE_MAP(K_true);
        SET_TK_TYPE_MAP(K_false);
        SET_TK_TYPE_MAP(K_extend);
        SET_TK_TYPE_MAP(K_type);
        SET_TK_TYPE_MAP(K_alias);
        SET_TK_TYPE_MAP(K_break);
        SET_TK_TYPE_MAP(K_continue);
        SET_TK_TYPE_MAP(Back_Slash);
#undef SET_TK_TYPE_MAP
        // this is a runtime check that will warn if we forgot any
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

static
bool parse_number(std::vector<Token> &tokens, Source_Code *src)
{
    std::stringstream num;
    auto line = src->line();
    auto line_char = src->line_char();
    bool is_real = false;
    while (src->peek() != src->end_of_file)
    {
        if (Lexer::is_digit(src->peek()))
        {
            num << (char)src->next();
            continue;
        }
        if (src->peek() == '.')
        {
            // valid: 123.stuff
            if (!Lexer::is_digit(src->peek(1))) { break; }
            // valid: 123.4.stuff
            if (is_real) { break; }
            is_real = true;
            num << (char)src->next();
            continue;
        }
        if (Lexer::is_ident_char(src->peek()))
        {
            // invalid: 123a
            src->report_at_src_loc("error", src->curr_src_loc(), "Unexpected character '%c' while parsing number\n", src->peek());
            return false;
        }
        // breaks on non-digits and non-ident chars
        break;
    }
    if (is_real)
    {
        tokens.push_back(Token(Token_Id::Real, num.str(),
                               {src, src->filename(), line, line_char}));
    }
    else
    {
        tokens.push_back(Token(Token_Id::Integer, num.str(),
                               {src, src->filename(), line, line_char}));
    }
    return true;
}

static
bool parse_string(std::vector<Token> &tokens, Source_Code *src)
{
    std::stringstream str;
    auto line = src->line();
    auto line_char = src->line_char();
    if (src->peek() != '"')
        return false;
    src->next(); // eat the first "
    while (src->peek() != src->end_of_file)
    {
        auto c = (char)src->next();
        if (c == '\\')
        {
            auto escaped = src->next();
            if (escaped == src->end_of_file)
                return false;
            str << (char)Lexer::to_escaped(escaped);
            continue;
        }
        if (c == '"')
        {
            break;
        }
        str << c;
    }
    tokens.push_back(Token(Token_Id::String, str.str(),
                           {src, src->filename(), line, line_char}));
    return true;
}

bool Lexer::lex(Source_Code *src)
{
    init_tk_type_map();
    tokens.clear();
    while (src->peek() != src->end_of_file)
    {
        if (src->peek() == '#')
        {
            while (src->peek() != '\n' && src->peek() != src->end_of_file)
                src->next();
            continue;
        }
        if (src->peek() == '\n')
        {
            if (!tokens.empty())
            {
                auto prev = tokens[tokens.size()-1];
                if (prev.id() == Token_Id::Back_Slash)
                {
                    // no need for the back slash to be in the token stream
                    tokens.pop_back();
                }
                else if (Lexer::is_replace_newline_with_semicolon_prev(prev.id()))
                {
                    // look ahead and make sure the next token would be ok with inserting
                    // a semicolon
                    if (!Lexer::should_not_insert_semicolon_next(src))
                    {
                        tokens.push_back(
                            Token(Token_Id::StmtTerminator, ";", src->curr_src_loc()));
                    }
                }
            }
            src->next();
            continue;
        }
        if (is_wspace(src->peek()))
        {
            src->next();
            continue;
        }
        if (src->peek() == '"')
        {
            if (!parse_string(tokens, src))
            {
                return false;
            }
            continue;
        }
        if (is_digit(src->peek()))
        {
            if (!parse_number(tokens, src))
            {
                return false;
            }
            continue;
        }
#define PUSH_KEY_IDENT(str, id) \
        if (match_ident(src, (str), 0))           \
        { \
            tokens.push_back(Token(Token_Id::id, (str), src->curr_src_loc())); \
            src->advance(sizeof(str) - 1); \
            continue; \
        }
        
        PUSH_KEY_IDENT("alias", K_alias);
        PUSH_KEY_IDENT("and", Log_And);
        PUSH_KEY_IDENT("break", K_break);
        PUSH_KEY_IDENT("continue", K_continue);
        PUSH_KEY_IDENT("extend", K_extend);
        PUSH_KEY_IDENT("else", K_else);
        PUSH_KEY_IDENT("false", K_false);
        PUSH_KEY_IDENT("for", K_for);
        PUSH_KEY_IDENT("fn", K_fn);
        PUSH_KEY_IDENT("if", K_if);
        PUSH_KEY_IDENT("match", K_match);
        PUSH_KEY_IDENT("or", Log_Or);
        PUSH_KEY_IDENT("return", K_return);
        PUSH_KEY_IDENT("true", K_true);
        PUSH_KEY_IDENT("type", K_type);
        PUSH_KEY_IDENT("while", K_while);

        if (is_ident_start_char(src->peek()))
        {
            std::stringstream ident;
            auto line = src->line();
            auto line_char = src->line_char();
            while (is_ident_char(src->peek()))
            {
                ident << (char)src->next();
            }
            tokens.push_back(Token(Token_Id::Identifier, ident.str(), {src, src->filename(), line, line_char}));
            continue;
        }
#define PUSH_TOKEN_BODY(str, id) \
{ \
tokens.push_back(Token(Token_Id::id, (str), src->curr_src_loc())); \
for (size_t i = 0; i < sizeof(str)-1; ++i) { src->next(); } \
continue; \
}
        // @FixMe: this needs lookahead after the token because >=< will parse into [Greater_Equals, Less]
#define PUSH_TOKEN_1C(str, id) if (src->peek() == (str)[0]) PUSH_TOKEN_BODY(str, id)
#define PUSH_TOKEN_2C(str, id) if (src->peek() == (str)[0] && src->peek(1) == (str)[1]) PUSH_TOKEN_BODY(str, id)
#define PUSH_TOKEN_3C(str, id) if (src->peek() == (str)[0] && src->peek(1) == (str)[1] && src->peek(2) == (str[2])) PUSH_TOKEN_BODY(str, id)

        PUSH_TOKEN_3C("<<=", L_Shift_Equals);
        PUSH_TOKEN_3C(">>=", R_Shift_Equals);
        PUSH_TOKEN_3C("<=>", Compare);
        PUSH_TOKEN_3C("[]=", Op_Index_Set);
        PUSH_TOKEN_2C("[]", Op_Index_Get);


        PUSH_TOKEN_2C("+=", Plus_Equals);
        PUSH_TOKEN_2C("-=", Minus_Equals);
        PUSH_TOKEN_2C("*=", Star_Equals);
        PUSH_TOKEN_2C("/=", Slash_Equals);
        PUSH_TOKEN_2C("%=", Mod_Equals);
        PUSH_TOKEN_2C("&=", Bit_And_Equals);
        PUSH_TOKEN_2C("^=", Bit_Xor_Equals);
        PUSH_TOKEN_2C("|=", Bit_Or_Equals);

        PUSH_TOKEN_2C("-@", Minus_At);
        PUSH_TOKEN_2C("+@", Plus_At);
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
        PUSH_TOKEN_1C(";", StmtTerminator);
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
        PUSH_TOKEN_1C("\\", Back_Slash);

        src->report_at_src_loc("error", src->curr_src_loc(), "Unexpected character '%c' (%x)\n", src->peek(), (int)src->peek());
        return false;
    }
    return true;
}

bool Lexer::match_ident(Source_Code *code, const std::string &ident, int offset)
{
    for (size_t i = 0; i < ident.size(); ++i)
    {
        if (code->peek(i+offset) != ident[i])
        {
            return false;
        }
    }
    if (is_ident_char(code->peek(ident.size())))
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

int Lexer::to_escaped(int c)
{
    switch (c)
    {
        default: return c;
        case 't': return '\t';
        case 'n': return '\n';
        case 'r': return '\r';
    }
}

bool Lexer::is_replace_newline_with_semicolon_prev(Token_Id tk)
{
    switch (tk)
    {
        default: return false;
            // could put K_return in here too
        case Token_Id::Identifier:
        case Token_Id::Integer:
        case Token_Id::Real:
        case Token_Id::String:
        case Token_Id::Close_Paren:
        case Token_Id::Close_Curly:
        case Token_Id::Close_Square:
        case Token_Id::K_return:
            return true;
    }
}


bool Lexer::should_not_insert_semicolon_next(Source_Code *src)
{
    int i = 0;
    while (src->peek(i) != src->end_of_file
           && Lexer::is_wspace(src->peek(i)))
    {
        ++i;
    }
    if (Lexer::match_ident(src, "else", i))
    {
        return true;
    }
    if (src->peek() == '{') {
        return true;
    }
    return false;
}
