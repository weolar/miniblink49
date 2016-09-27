

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

}

#endif // USING_VC6RT