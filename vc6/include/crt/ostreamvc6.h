#ifndef ostreamvc6_h
#define ostreamvc6_h

#if USING_VC6RT == 1

namespace std {

inline ostream& operator<<(std::ostream& os, __int64 val);
inline ostream& operator<<(ostream& os, unsigned __int64 val);

}

#endif

#endif // ostreamvc6_h