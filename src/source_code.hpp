#ifndef MALANG_SOURCE_CODE_HPP
#define MALANG_SOURCE_CODE_HPP

#include <stdarg.h>
#include <string>

struct Source_Location
{
    std::string to_string() const;

    const class Source_Code *source_code;
    std::string filename;
    int line_no;
    int char_no;

    void report(const char *type, const char *fmt, ...) const;
    void vreport(const char *type, const char *fmt, va_list vargs) const;
};

class Source_Code
{
public:
    Source_Code() 
        : m_good(false)
        , m_cur_line(1)
        , m_cur_line_char(1)
        , m_idx(0)
    {}
    Source_Code(const std::string &filename);
    Source_Code(const std::string &filename, const std::string &code)
        : m_good(true)
        , m_cur_line(1)
        , m_cur_line_char(1)
        , m_idx(0)
        , m_filename(filename)
        , m_code(code)
    {}

    static constexpr int end_of_file = -1;

    int next();
    void advance(size_t n);
    int peek(size_t n = 0) const;
    int line() const;
    int line_char() const;
    auto position() const;
    auto size() const;
    auto length() const;
    char &operator[](size_t i);
    auto begin();
    auto end();
    bool good() const;
    const std::string &code() const;
    const std::string &filename() const;
    void report_at_src_loc(const char *type, const Source_Location &src_loc, const char *fmt, ...) const;
    void vreport_at_src_loc(const char *type, const Source_Location &src_loc, const char *fmt, va_list vargs) const;
    Source_Location curr_src_loc() const;
private:
    bool m_good;
    int m_cur_line;
    int m_cur_line_char;
    size_t m_idx;
    std::string m_filename;
    std::string m_code;
};


#endif /* MALANG_SOURCE_CODE_HPP */
