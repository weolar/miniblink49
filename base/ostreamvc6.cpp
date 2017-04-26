
#if USING_VC6RT == 1

#include <ostream>
#include <sstream>

namespace std {

 //ostream& operator<<(std::ostream& os, __int64 val)
 //{
 //    char buf[32];
 //    sprintf(buf, "%I64d", val);
 //    os << buf;
 //    return os;
 //}
 //
 //ostream& operator<<(ostream& os, unsigned __int64 val)
 //{
 //    char buf[32];
 //    sprintf(buf, "%I64d", val);
 //    os << buf;
 //    return os;
 //}

basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & os, unsigned char const * str)
{
	if (!str)
		return os;

	os << ((const char *)str);
	return os;
}

basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & os, unsigned char c)
{
	os << ((char)c);
	return os;
}

basic_ostream<char, char_traits<char> > & __cdecl operator<< (basic_ostream<char, char_traits<char> > & a, __int64 b)
{
    ostringstream aCopy;
    char buf[32];
    sprintf(buf, "%I64d", b);
    aCopy << buf;
    a << aCopy;
    return a;
}

basic_ostream<char, char_traits<char> > & __cdecl operator << (basic_ostream<char, char_traits<char> >& a, unsigned __int64 b)
{
    ostringstream aCopy;
    char buf[32];
    sprintf(buf, "%I64u", b);
    aCopy << buf;
    a << aCopy;
    return a;
}

basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & a, char b)
{
    ostringstream aCopy;
    char bCopy[3] = { b, 0, 0 };
    aCopy.write(bCopy, 1);
    a << aCopy;
    return a;
}

basic_ostream<char, char_traits<char> >& __cdecl operator << <char, char_traits<char> >(
    basic_ostream<char, char_traits<char> > & os, char const * b) // weolar 多重定义
{
    ostringstream aCopy;
    size_t len = strlen(b);
    aCopy.write(b, len);
    os << aCopy;
    return os;
}

}

#endif // USING_VC6RT