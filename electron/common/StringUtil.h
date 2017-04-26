
#ifndef atom_StringUtil_h
#define atom_StringUtil_h

#include <windows.h>
#include <string>
#include <vector>

namespace atom {

class StringUtil {
public:
    static std::wstring UTF8ToUTF16(const std::string& utf8) {
        std::wstring utf16;
        size_t n = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), nullptr, 0);
        std::vector<wchar_t> wbuf(n);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], n);
        utf16.resize(n);
        utf16.assign(&wbuf[0], n);
        return utf16;
    }

    static std::string UTF16ToUTF8(const std::wstring& utf16) {
        std::string utf8;
        size_t n = ::WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), -1, NULL, 0, NULL, NULL);
        std::vector<char> buf(n + 1);
        ::WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), -1, &buf[0], n, NULL, NULL);
        utf8.resize(n);
        utf8.assign(&buf[0], n);
        return utf8;
    }
};

} // atom

#endif // atom_StringUtil_h