#if USING_VC6RT == 1

#include <ostream>
#include <sstream>

namespace std {

basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char>, allocator<char> >(
    basic_ostream<char, char_traits<char> > & a, basic_string<char, char_traits<char>, allocator<char> > const & b) // weolar �޶���
{
    ostringstream aCopy;
    aCopy.write(b.c_str(), b.size());
    a << aCopy;
    return a;
}

}

#endif // USING_VC6RT