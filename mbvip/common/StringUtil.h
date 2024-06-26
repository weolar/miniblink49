
#ifndef common_StringUtil_h
#define common_StringUtil_h

#include <string>
#include <vector>

namespace common {

std::string utf16ToUtf8(const wchar_t* lpszSrc);
std::string utf16ToMulByte(const wchar_t* lpszSrc, unsigned int codepage);

std::wstring utf8ToUtf16(const std::string& utf8);
std::wstring mulByteToUtf16(const std::string& str, unsigned int codepage);
bool isTextUTF8(const char *str, size_t length);

std::string base64Encode(const unsigned char* bytesToEncode, unsigned int inLen);
std::vector<unsigned char> base64Decode(const std::string& encodedString);

std::wstring createPathFromDllPath(const std::wstring& fileName);

bool isLocalDebugMachine();

void setPluginDirectory(const std::wstring& dir);
std::wstring getPluginDirectory();

}

#endif // base_StringUtil_h