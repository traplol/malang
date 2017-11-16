#include <sstream>
#include <fstream>
#include "source_code.hpp"

bool read_file(const std::string &filename, std::string &contents)
{
    std::ifstream inf(filename);
    if (inf)
    {
        contents.assign(std::istreambuf_iterator<char>(inf), std::istreambuf_iterator<char>());
        return true;
    }
    else
    {
        return false;
    }
}

Source_Code::Source_Code(const std::string &filename)
    : m_cur_line(1)
    , m_cur_line_char(1)
    , m_idx(0)
    , m_filename(filename)
{
    m_good = read_file(filename, m_code);
}

bool Source_Code::good() const { return m_good; }

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

static
bool find_matching_bracket_right(const std::string &s, size_t start, char open, char close, size_t &out)
{
    int pcount = 1;
    bool found = false;
    size_t found_idx;
    for (auto i = start; i < s.size(); ++i)
    {
        if (pcount == 0)
        {
            found = true;
            found_idx = i;
            break;
        }
        if (s[i] == close)
        {
            --pcount;
        }
        else if (s[i] == open)
        {
            ++pcount;
        }
    }
    if (found)
    {
        out = found_idx;
    }
    return found;
}

static
bool find_matching_bracket_left(const std::string &s, size_t start, char open, char close, size_t &out)
{
    int pcount = 1;
    bool found = false;
    size_t found_idx;
    for (auto i = start; i != static_cast<size_t>(-1); --i)
    {
        if (s[i] == close)
        {
            ++pcount;
        }
        else if (s[i] == open)
        {
            --pcount;
        }
        if (pcount == 0)
        {
            found = true;
            found_idx = i;
            break;
        }
    }
    if (found)
    {
        out = found_idx;
    }
    return found;
}

static
bool find_wspace_right(const std::string &s, size_t start, size_t &out)
{
    bool found = false;
    size_t found_idx;
    for (auto i = start; i < s.size(); ++i)
    {
        if (iswspace(s[i]))
        {
            found_idx = i;
            found = true;
            break;
        }
    }
    if (found)
    {
        out = found_idx-1;
    }
    return found;
}

static
bool find_wspace_left(const std::string &s, size_t start, size_t &out)
{
    bool found = false;
    size_t found_idx;
    for (auto i = start; i != static_cast<size_t>(-1); --i)
    {
        if (iswspace(s[i]))
        {
            found_idx = i;
            found = true;
            break;
        }
    }
    if (found)
    {
        out = found_idx;
    }
    return found;
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
    auto const x = src_loc.char_no-1;
    auto c = reported_line[x];
    size_t u_line_start = 0;
    size_t u_line_end = 0;
    if (c == '(')
    {
        u_line_start = x;
        if (!find_matching_bracket_right(reported_line, x, '(', ')', u_line_end))
        {
            u_line_end = reported_line.size();
        }
    }
    else if (c == '[')
    {
        u_line_start = x;
        if (!find_matching_bracket_right(reported_line, x, '[', ']', u_line_end))
        {
            u_line_end = reported_line.size();
        }
    }
    else if (c == ')')
    {
        u_line_end = x;
        if (!find_matching_bracket_left(reported_line, x, '(', ')', u_line_start))
        {
            u_line_start = 0;
        }
    }
    else if (c == ']')
    {
        u_line_end = x;
        if (!find_matching_bracket_left(reported_line, x, '[', ']', u_line_start))
        {
            u_line_start = 0;
        }
    }
    else
    {
        if (!find_wspace_left(reported_line, x, u_line_start))
        {
            u_line_start = 0;
        }
        if (!find_wspace_right(reported_line, x, u_line_end))
        {
            u_line_end = 0;
        }
    }
    for (size_t i = 0; i <= u_line_start; ++i)
    {
        printf(" ");
    }
    for (size_t i = u_line_start; i < u_line_end; ++i)
    {
        printf("-");
    }
    printf("\n\n");
}
