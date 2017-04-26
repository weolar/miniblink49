
#ifndef base_string_util_h
#define base_string_util_h

template <class Char> inline Char ToLowerASCII(Char c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

template <class str> inline void StringToLowerASCII(str* s) {
    for (typename str::iterator i = s->begin(); i != s->end(); ++i)
        *i = ToLowerASCII(*i);
}

template <class str> inline str StringToLowerASCII(const str& s) {
    // for std::string and std::wstring  
    str output(s);
    StringToLowerASCII(&output);
    return output;
}

template<class STR>
inline bool DoIsStringASCII(const STR& str) {
    for (size_t i = 0; i < str.length(); i++) {
        unsigned short c = str[i];
        if (c > 0x7F)
            return false;
    }
    return true;
}

inline bool IsStringASCII(const std::string& str) {
    return DoIsStringASCII(str);
}

inline bool IsStringASCII(const std::wstring& str) {
    return DoIsStringASCII(str);
}

#endif // base_string_util_h
