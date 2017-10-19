#ifndef MALANG_IR_LABEL_MAP_HPP
#define MALANG_IR_LABEL_MAP_HPP

#include <map>
#include "../source_code.hpp"

struct Label_Map
{
    struct IR_Label *make_label(const std::string &name, const Source_Location &src_loc);
    struct IR_Label *get_label(const std::string &name);

    struct IR_Named_Block *make_named_block(const std::string &name, const std::string &end_name, const Source_Location &src_loc);
    struct IR_Named_Block *get_named_block(const std::string &name);
private:
    std::map<std::string, struct IR_Label*> m_map;
};

#endif /* MALANG_IR_LABEL_MAP_HPP */
