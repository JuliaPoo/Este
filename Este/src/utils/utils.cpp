#include "Este\utils.hpp"

#include <algorithm>
#include <cctype>

bool EsteUtils::hasSuffix(std::string const& str, std::string const& suffix)
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

std::string EsteUtils::json_escape(std::string const s) 
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