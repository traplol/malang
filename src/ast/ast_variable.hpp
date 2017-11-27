#ifndef MALANG_AST_AST_VARIABLE_HPP
#define MALANG_AST_AST_VARIABLE_HPP

#include "ast_value.hpp"

struct Variable_Node : public Ast_LValue
{
    using Name_Qualifiers = std::vector<std::string>;
    ~Variable_Node();
    Variable_Node(const Source_Location &src_loc, const std::string &name, const Name_Qualifiers &qualifiers)
        : Ast_LValue(src_loc)
        , m_name(name)
        , m_qualifiers(qualifiers)
    {}
    AST_NODE_OVERRIDES;

    const std::string &local_name() const { return m_name; }
    const Name_Qualifiers &qualifiers() const { return m_qualifiers; }
    const std::string &name();
    bool is_qualified() const { return m_qualifiers.size() != 0; }
private:
    std::string m_name;
    std::string m_full_name;
    Name_Qualifiers m_qualifiers;
    
};

#endif /* MALANG_AST_AST_VARIABLE_HPP */
