#ifndef MALANG_DIR_HPP
#define MALANG_DIR_HPP

#include <string>
namespace plat
{
    std::string get_directory(const std::string &path);
    std::string get_abs_path(const std::string &path);
    std::string get_cwd();
    std::string get_mal_exe_path();
}

#endif /* MALANG_DIR_HPP */
