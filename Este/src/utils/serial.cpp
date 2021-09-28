#include "Este\serial.hpp"

std::ofstream db;

void Serial::Init_serializers(
	std::string db_filepath
)
{
	new (&db) decltype(db)(
		db_filepath.c_str(),
		std::ios::out | std::ios::binary);
}

std::ofstream& Serial::getDb()
{
	return db;
}

std::string Serial::escape(std::string const s) {
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