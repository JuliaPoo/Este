#include "Este\utils.hpp"

#include <algorithm>
#include <cctype>

void* EsteUtils::aligned_malloc(size_t required_bytes, size_t alignment)
{
    void* p1; // original block
    void** p2; // aligned block
    size_t offset = alignment - 1 + sizeof(void*);
    if ((p1 = (void*)malloc(required_bytes + offset)) == NULL)
    {
        return NULL;
    }
    p2 = (void**)(((size_t)(p1)+offset) & ~(alignment - 1));
    p2[-1] = p1;
    return p2;
}

void EsteUtils::aligned_free(void* p)
{
    free(((void**)p)[-1]);
}

bool EsteUtils::hasSuffix(const std::string& str, const std::string& suffix)
{
    if (str.length() >= suffix.length())
        return (0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix));
    else
        return false;
}

std::string& EsteUtils::toLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return str;
}

std::string EsteUtils::json_escape(const std::string s)
{
    std::string out = "\"";
    for (std::string::const_iterator i = s.begin(), end = s.end(); i != end; ++i) {
        unsigned char c = *i;
        if (' ' <= c and c <= '~' and c != '\\' and c != '"') {
            out += c;
        }
        else {
            out += '\\';
            switch (c) {
            case '"':  out += '\"';  break;
            case '\\': out += '\\'; break;
            case '\t': out += 't';  break;
            case '\r': out += 'r';  break;
            case '\n': out += 'n';  break;
            default:
                char const* const hexdig = "0123456789ABCDEF";
                out += 'x';
                out += hexdig[c >> 4];
                out += hexdig[c & 0xF];
            }
        }
    }
    out += '\"';
    return out;
}

std::string EsteUtils::csv_escape(const std::string s)
{
    std::string out = "\"";
    for (std::string::const_iterator i = s.begin(), end = s.end(); i != end; ++i) {
        unsigned char c = *i;
        if (' ' <= c and c <= '~' and c != '"') {
            out += c;
        }
        else {
            switch (c) {
            case '"':  out += "\"\"";  break;
            case '\t': out += "\\t";  break;
            case '\r': out += "\\r";  break;
            case '\n': out += "\\n";  break;
            default:
                char const* const hexdig = "0123456789ABCDEF";
                out += "\\x";
                out += hexdig[c >> 4];
                out += hexdig[c & 0xF];
            }
        }
    }
    out += '\"';
    return out;
}

std::string EsteUtils::toHex(const std::vector<uint8_t> s)
{
    std::string out;
    char const* const hexdig = "0123456789ABCDEF";
    for (auto c : s) {
        out += hexdig[c >> 4];
        out += hexdig[c & 0xf];
    }
    return out;
}