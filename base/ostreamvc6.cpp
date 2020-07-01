
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

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & os, unsigned char const * str)
{
    if (!str)
        return os;

    os << ((const char *)str);
    return os;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & os, unsigned char c)
{
    os << ((char)c);
    return os;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< (basic_ostream<char, char_traits<char> > & a, __int64 b)
{
    ostringstream aCopy;
    char buf[32];
    sprintf(buf, "%I64d", b);
    aCopy << buf;
    a << aCopy;
    return a;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator << (basic_ostream<char, char_traits<char> >& a, unsigned __int64 b)
{
    ostringstream aCopy;
    char buf[32];
    sprintf(buf, "%I64u", b);
    aCopy << buf;
    a << aCopy;
    return a;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & a, char b)
{
    ostringstream aCopy;
    char bCopy[3] = { b, 0, 0 };
    aCopy.write(bCopy, 1);
    a << aCopy;
    return a;
}

template<>
basic_ostream<char, char_traits<char> >& __cdecl operator << <char, char_traits<char> >(
    basic_ostream<char, char_traits<char> > & os, char const * b) // weolar 多重定义
{
    ostringstream aCopy;
    size_t len = strlen(b);
    aCopy.write(b, len);
    os << aCopy;
    return os;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char>, allocator<char> >(
    basic_ostream<char, char_traits<char> > & a, basic_string<char, char_traits<char>, allocator<char> > const & b) // weolar 无定义
{
    ostringstream aCopy;
    aCopy.write(b.c_str(), b.size());
    a << aCopy;
    return a;
}

// basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, struct std::char_traits<char> >(
//     basic_ostream<char, char_traits<char> > & a, char const * b) {
//     ostringstream aCopy;
//     aCopy.write(b, strlen(b));
//     a << aCopy;
//     return a;
// }

// basic_istream<char, char_traits<char> > & __cdecl getline<char, char_traits<char>, allocator<char> >(
//     basic_istream<char, struct char_traits<char> > &,
//     basic_string<char, struct char_traits<char>, allocator<char> > &, char)
// {
// 
// }

}

#endif // USING_VC6RT