

#if USING_VC6RT == 1

#include <ostream>
#include <ostreamvc6.h>

namespace std {

ostream& operator<<(std::ostream& os, __int64 val)
{
    char buf[32];
    sprintf(buf, "%I64d", val);
    os << buf;
    return os;
}

ostream& operator<<(ostream& os, unsigned __int64 val)
{
    char buf[32];
    sprintf(buf, "%I64d", val);
    os << buf;
    return os;
}

basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & os, unsigned char const * str)
{
	if (!str)
		return os;

	os << ((const char *)str);
	return os;
}

basic_ostream<char, char_traits<char> > & __cdecl operator<<<char, char_traits<char> >(basic_ostream<char, char_traits<char> > & os, unsigned char c)
{
	os << ((char)c);
	return os;
}

}

#endif // USING_VC6RT