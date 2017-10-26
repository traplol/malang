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
    Equals,
    Colon,
    Semicolon,
    Open_Paren,
    Close_Paren,
    Open_Square,
    Close_Square,
    Open_Curly,
    Close_Curly,
    Comma,
    Plus,
    Minus,
    Star,
    Slash,
    Equals_Equals,
    Not_Equals,
    Less,
    Less_Equals,
    Greater,
    Greater_Equals,
    Identifier,
    Dot,
    Mod,
    L_Shift,
    R_Shift,
    Compare,
    Bit_And,
    Bit_Xor,
    Bit_Or,
    Log_And,
    Log_Or,
    Invert,
    Not,
    Right_Arrow,

    K_b_op,
    K_u_op,
    //K_proc,
    K_for,
    K_while,
    K_return,
    K_if,
    K_else,
    K_fn,
    K_class,
    K_match,
    K_true,
    K_false,

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
};


#endif /* MALANG_LEXER_HPP */
