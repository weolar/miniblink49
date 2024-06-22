
#if USING_VC6RT == 1

#include <ostream>
#include <sstream>
#include <xstring>

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
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & a, unsigned char const * b)
{
    if (!b)
        return a;

    size_t len = strlen((const char*)b);
    if (0 == len)
        return a;

    a.write((const char*)b, len);
    return a;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & a, unsigned char b)
{
    a.put(b);
    return a;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< (basic_ostream<char, char_traits<char> > & a, __int64 b)
{
    char buf[32] = { 0 };

    const char* fmt = "%I64d";
    if (a.flags() & ios_base::hex)
        fmt = "%llu";
    sprintf(buf, fmt, b);

    a.write(buf, strlen(buf));
    return a;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator << (basic_ostream<char, char_traits<char> >& a, unsigned __int64 b)
{
    char buf[32] = { 0 };

    const char* fmt = "%I64u";
    if (a.flags() & ios_base::hex)
        fmt = "%llx";
    sprintf(buf, fmt, b);

    a.write(buf, strlen(buf));
    return a;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char> >(basic_ostream<char, char_traits<char> > & a, char b)
{
    a.put(b);
    return a;
}

template<>
basic_ostream<char, char_traits<char> >& __cdecl operator << <char, char_traits<char> >(
    basic_ostream<char, char_traits<char> >& a, char const* b) // weolar 多重定义
{
    size_t len = strlen(b);
    if (0 == len)
        return a;

    a.write(b, len);
    return a;
}

template<>
basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, char_traits<char>, allocator<char> >(
    basic_ostream<char, char_traits<char> > & a, basic_string<char, char_traits<char>, allocator<char> > const & b) // weolar 无定义
{
    if (0 == b.size())
        return a;

    a.write(b.c_str(), b.size());
    return a;
}

// basic_ostream<char, char_traits<char> > & __cdecl operator<< <char, struct std::char_traits<char> >(
//     basic_ostream<char, char_traits<char> > & a, char const * b) {
//     ostringstream aCopy;
//     aCopy.write(b, strlen(b));
//     a << aCopy.str();
//     return a;
// }

// basic_istream<char, char_traits<char> > & __cdecl getline<char, char_traits<char>, allocator<char> >(
//     basic_istream<char, struct char_traits<char> > &,
//     basic_string<char, struct char_traits<char>, allocator<char> > &, char)
// {
// 
// }

// basic_string<char, char_traits<char>, allocator<char> >& basic_string<char, char_traits<char>, allocator<char> >::push_back(char c)
// {
//     return append(1, c);
// }
// 
// basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >& basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >::push_back(unsigned short c)
// {
//     return append(1, c);
// }
// 
// void basic_string<unsigned short, char_traits<unsigned short>, allocator<unsigned short> >::clear(void)
// {
// 
// }

// basic_string<char, char_traits<char>, allocator<char> >& basic_string<char, char_traits<char>, allocator<char> >::push_back(char)
// {
// 
// }

}

//void __stdcall `eh vector constructor iterator'(void *,unsigned int,unsigned int,void (__thiscall*)(void *),void (__thiscall*)(void *))

#if defined __cplusplus_cli
#define CALEETYPE __clrcall
#else
#define CALEETYPE __stdcall
#endif
#define __RELIABILITY_CONTRACT
#define SECURITYCRITICAL_ATTRIBUTE
#define ASSERT_UNMANAGED_CODE_ATTRIBUTE

#if defined __cplusplus_cli
#define CALLTYPE __clrcall 
#elif defined _M_IX86
#define CALLTYPE __thiscall
#else
#define CALLTYPE __stdcall
#endif

__RELIABILITY_CONTRACT
void CALEETYPE __ArrayUnwind(
    void*       ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
    int         count,              // Number of elements in the array
    void(CALLTYPE *pDtor)(void*)    // The destructor to call
    );

__RELIABILITY_CONTRACT
inline void CALEETYPE __ehvec_ctor(
    void*       ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
                                    //  int         count,              // Number of elements in the array
    size_t      count,              // Number of elements in the array
    void(CALLTYPE *pCtor)(void*),   // Constructor to call
    void(CALLTYPE *pDtor)(void*)    // Destructor to call should exception be thrown
    ) {
    size_t i = 0;      // Count of elements constructed
    int success = 0;

    __try {
        // Construct the elements of the array
        for (; i < count; i++) {
            (*pCtor)(ptr);
            ptr = (char*)ptr + size;
        }
        success = 1;
    } __finally {
        if (!success)
            __ArrayUnwind(ptr, size, (int)i, pDtor);
    }
}

__RELIABILITY_CONTRACT
SECURITYCRITICAL_ATTRIBUTE
inline void CALEETYPE __ehvec_dtor(
    void*       ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
                                    //  int         count,              // Number of elements in the array
    size_t      count,              // Number of elements in the array
    void(CALLTYPE *pDtor)(void*)    // The destructor to call
    ) {
    //_Analysis_assume_(count > 0);

    int success = 0;

    // Advance pointer past end of array
    ptr = (char*)ptr + size*count;

    __try {
        // Destruct elements
        while (count-- > 0) {
            ptr = (char*)ptr - size;
            (*pDtor)(ptr);
        }
        success = 1;
    } __finally {
        if (!success)
            __ArrayUnwind(ptr, size, (int)count, pDtor);
    }
}

#endif // USING_VC6RT