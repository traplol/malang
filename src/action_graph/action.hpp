#ifndef MALANG_ACTION_ACTION_HPP
#define MALANG_ACTION_ACTION_HPP

#include <string>

struct Action
{
    virtual std::string action_name() const;
    virtual std::string to_string() const;
};

#endif
