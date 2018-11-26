#if USING_VC6RT == 1

#include <ostream>
#include <sstream>

namespace std {

basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char>, allocator<char> >(
    basic_ostream<char, char_traits<char> > & a, basic_string<char, char_traits<char>, allocator<char> > const & b) // weolar Œﬁ∂®“Â
{
    ostringstream aCopy;
    aCopy.write(b.c_str(), b.size());
    a << aCopy;
    return a;
}

basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, struct std::char_traits<char> >(
    basic_ostream<char, char_traits<char> > & a, char const * b)
{
    ostringstream aCopy;
    aCopy.write(b, strlen(b));
    a << aCopy;
    return a;
}

}

#endif // USING_VC6RT