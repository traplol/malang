#include <unistd.h>
#include "dir.hpp"

std::string plat::get_directory(const std::string &path)
{
    if (path.empty())
    {
        return "";
    }
    auto idx = path.find_last_of("/\\");
    return path.substr(0, idx);
}

std::string plat::get_abs_path(const std::string &path)
{
    auto abs = realpath(path.c_str(), NULL);
    if (abs)
    {
        std::string res(abs);
        free(abs);
        return res;
    }
    return std::string();
}

std::string plat::get_cwd()
{
    char buf[2048]{0};
    return getcwd(buf, sizeof(buf))
        ? buf
        : std::string();
}

std::string plat::get_mal_exe_path()
{
    char buf[2048]{0};
    if (-1 == readlink("/proc/self/exe", buf, sizeof(buf))) {
        return std::string();
    }
    return buf;
}
