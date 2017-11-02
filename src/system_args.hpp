#ifndef MALANG_SYSTEM_ARGS_HPP
#define MALANG_SYSTEM_ARGS_HPP

#include <string>

struct Args
{
    bool noisy = true;
    std::string filename;
    std::string code;
};

#endif /* MALANG_SYSTEM_ARGS_HPP */
