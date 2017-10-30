#ifndef MALANG_LEXER_HPP
#define MALANG_LEXER_HPP

#include <string>
#include <vector>

#include "source_code.hpp"

enum class Token_Id : size_t
{
    Invalid,
    Integer,
    Real,
    String,
    Equals,             // =
    Colon,              // :
    Semicolon,          // ;
    Open_Paren,         // (
    Close_Paren,        // )
    Open_Square,        // [
    Close_Square,       // ]
    Open_Curly,         // {
    Close_Curly,        // }
    Comma,              // ,
    Plus,               // +
    Minus,              // -
    Star,               // *
    Slash,              // /
    Equals_Equals,      // ==
    Not_Equals,         // !=
    Less,               // <
    Less_Equals,        // <=
    Greater,            // >
    Greater_Equals,     // >=
    Identifier,
    Dot,                // .
    Mod,                // %
    L_Shift,            // <<
    R_Shift,            // >>
    Compare,            // <=>
    Bit_And,            // &
    Bit_Xor,            // ^
    Bit_Or,             // |
    Log_And,            // and &&
    Log_Or,             // or ||
    Invert,             // ~
    Not,                // !
    Right_Arrow,        // ->

    Plus_At,            // +@
    Minus_At,           // -@
    Op_Index_Get,       // []
    Op_Index_Set,       // []=

    K_b_op,             // b_op
    K_u_op,             // u_op
    K_for,              // for
    K_while,            // while
    K_return,           // return
    K_if,               // if
    K_else,             // else
    K_fn,               // fn
    K_class,            // class
    K_match,            // match
    K_true,             // true
    K_false,            // false
    K_extend,           // extend

    NUM_TOKEN_TYPES
};

std::string to_string(Token_Id tk_type);

class Token 
{
public:
    Token()
        : m_id(Token_Id::Invalid)
        , m_string("")
        , m_src_loc()
    {}
    Token(Token_Id id, const std::string &string, Source_Location src_loc)
        : m_id(id)
        , m_string(string)
        , m_src_loc(src_loc)
    {}

    double to_real() const;
    int64_t to_int() const;
    std::string to_string() const;
    Token_Id id() const;
    Source_Location src_loc() const;

    std::string debug() const;
private:
    Token_Id m_id;
    std::string m_string;
    Source_Location m_src_loc;
};

struct Lexer 
{
    std::vector<Token> tokens;

    bool lex(Source_Code *code);
    void dump();
    static bool match_ident(Source_Code *code, const std::string &ident);
    static bool is_ident_start_char(int c);
    static bool is_ident_char(int c);
    static bool is_digit(int c);
    static bool is_wspace(int c);
    static int to_escaped(int c);
};


#endif /* MALANG_LEXER_HPP */
