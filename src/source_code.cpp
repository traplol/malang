#include <sstream>
#include "source_code.hpp"

std::string Source_Location::to_string() const
{
    std::stringstream ss;
    ss << filename << ":" << line_no << ":" << char_no;
    return ss.str();
}

void Source_Location::report(const char *type, const char *fmt, ...) const
{
    va_list vargs;
    va_start(vargs, fmt);
    vreport(type, fmt, vargs);
    va_end(vargs);
}
void Source_Location::vreport(const char *type, const char *fmt, va_list vargs) const
{
    if (source_code)
    {
        source_code->vreport_at_src_loc(type, *this, fmt, vargs);
    }
    else
    {
        printf("%s: %s:%d:%d\n\n", type, filename.c_str(), line_no, char_no);
        printf("\t");
        vprintf(fmt, vargs);
        printf("\n");
    }
}

int Source_Code::next()
{
    if (m_idx >= m_code.size())
    {
        return end_of_file;
    }
    int c = m_code[m_idx++];
    if (c == '\n')
    {
        ++m_cur_line;
        m_cur_line_char = 1;
    }
    else
    {
        ++m_cur_line_char;
    }
    return c;
}
void Source_Code::advance(size_t n)
{
    while (n--)
    {
        next();
    }
}
int Source_Code::peek(size_t n) const
{
    if (m_idx+n >= m_code.size())
    {
        return end_of_file;
    }
    return m_code[m_idx+n];
}
int Source_Code::line() const
{
    return m_cur_line;
}
int Source_Code::line_char() const
{
    return m_cur_line_char;
}
auto Source_Code::position() const
{
    return m_idx;
}
auto Source_Code::size() const
{
    return m_code.size();
}
auto Source_Code::length() const
{
    return m_code.length();
}
char &Source_Code::operator[](size_t i)
{
    return m_code[i];
}
auto Source_Code::begin() 
{
    return m_code.begin();
}
auto Source_Code::end() 
{
    return m_code.end();
}

const std::string &Source_Code::code() const
{
    return m_code;
}
const std::string &Source_Code::filename() const
{
    return m_filename;
}

Source_Location Source_Code::curr_src_loc() const
{
    return { this, m_filename, m_cur_line, m_cur_line_char };
}

void Source_Code::report_at_src_loc(const char *type, const Source_Location &src_loc, const char *fmt, ...) const
{
    va_list vargs;
    va_start(vargs, fmt);
    vreport_at_src_loc(type, src_loc, fmt, vargs);
    va_end(vargs);
}

void Source_Code::vreport_at_src_loc(const char *type, const Source_Location &src_loc, const char *fmt, va_list vargs) const
{
    printf("%s: %s:%d:%d\n\n", type, m_filename.c_str(), src_loc.line_no, src_loc.char_no);
    if (fmt[0] != 0)
    {
        printf("\t");
        vprintf(fmt, vargs);
        printf("\n");
    }
    int cur_line = 1;
    size_t line_idx = 0;
    size_t i = 0;
    for (; i < m_code.size(); ++i)
    {
        if (cur_line == src_loc.line_no)
        {
            line_idx = i;
            ++i;
            break;
        }
        if (m_code[i] == '\n')
        {
            ++cur_line;
        }
    }
    for (; i < m_code.size(); ++i)
    { // find end of this line
        if (m_code[i] == '\n')
        {
            break;
        }
    }
    auto reported_line = m_code.substr(line_idx, i - line_idx);
    printf("\t%s\n", reported_line.c_str());
    printf("\t");
    auto char_idx = src_loc.char_no - 1;
    while (char_idx-- > 0)
    {
        printf("~");
    }
    printf("^\n\n");
}
