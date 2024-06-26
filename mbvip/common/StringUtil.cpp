
#include "common/StringUtil.h"

#include <string>
#include <vector>
#include <windows.h>
#include <Shlwapi.h>

extern HMODULE g_hModule;

namespace common {

std::string utf16ToUtf8(const wchar_t* lpszSrc)
{
    return utf16ToMulByte(lpszSrc, CP_UTF8);
}

std::string utf16ToMulByte(const wchar_t* lpszSrc, unsigned int codepage)
{
    std::string sResult;
    if (lpszSrc != NULL) {
        int  nUTF8Len = WideCharToMultiByte(codepage, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
        char* pUTF8 = new char[nUTF8Len + 1];
        if (pUTF8 != NULL) {
            ZeroMemory(pUTF8, nUTF8Len + 1);
            WideCharToMultiByte(codepage, 0, lpszSrc, -1, pUTF8, nUTF8Len, NULL, NULL);
            sResult = pUTF8;
            delete[] pUTF8;
        }
    }
    return sResult;
}

std::wstring utf8ToUtf16(const std::string& utf8)
{
    return mulByteToUtf16(utf8, CP_UTF8);
}

std::wstring mulByteToUtf16(const std::string& str, unsigned int codepage)
{
    std::wstring utf16;
    size_t n = ::MultiByteToWideChar(codepage, 0, str.c_str(), (int)str.size(), nullptr, 0);
    if (0 == n)
        return L"";
    std::vector<wchar_t> wbuf(n);
    ::MultiByteToWideChar(codepage, 0, str.c_str(), (int)str.size(), &wbuf[0], n);
    utf16.resize(n + 5);
    utf16.assign(&wbuf[0], n);
    return utf16;
}


static const std::string base64Chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool isBase64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64Encode(const unsigned char* bytesToEncode, unsigned int inLen)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char charArray3[3];
    unsigned char charArray4[4];

    while (inLen--) {
        charArray3[i++] = *(bytesToEncode++);
        if (i == 3) {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64Chars[charArray4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            charArray3[j] = '\0';

        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
        charArray4[3] = charArray3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64Chars[charArray4[j]];

        while ((i++ < 3))
            ret += '=';

    }

    return ret;
}

std::vector<unsigned char> base64Decode(const std::string& encodedString)
{
    size_t inLen = encodedString.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char charArray4[4], charArray3[3];
    std::vector<unsigned char> ret;

    while (inLen-- && (encodedString[in_] != '=') && isBase64(encodedString[in_])) {
        charArray4[i++] = encodedString[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                charArray4[i] = (unsigned char)base64Chars.find(charArray4[i]);

            charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
            charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
            charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

            for (i = 0; (i < 3); i++)
                ret.push_back(charArray3[i]);
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            charArray4[j] = 0;

        for (j = 0; j < 4; j++)
            charArray4[j] = (unsigned char)base64Chars.find(charArray4[j]);

        charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
        charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
        charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

        for (j = 0; (j < i - 1); j++)
            ret.push_back(charArray3[j]);
    }

    return ret;
}

bool isTextUTF8(const char *str, size_t length)
{
    size_t i = 0;
    DWORD nBytes = 0; // UFT8可用1-6个字节编码,ASCII用一个字节
    UCHAR chr = 0;
    bool bAllAscii = true; // 如果全部都是ASCII, 说明不是UTF-8
    for (i = 0; i < length; ++i) {
        chr = (UCHAR)* (str + i);

        if ((chr & 0x80) != 0) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
            bAllAscii = false;

        if (nBytes == 0) { // 如果不是ASCII码,应该是多字节符,计算字节数

            if (chr >= 0x80) {
                if (chr >= 0xFC && chr <= 0xFD)
                    nBytes = 6;
                else if (chr >= 0xF8)
                    nBytes = 5;
                else if (chr >= 0xF0)
                    nBytes = 4;
                else if (chr >= 0xE0)
                    nBytes = 3;
                else if (chr >= 0xC0)
                    nBytes = 2;
                else {
                    return false;
                }
                nBytes--;
            }
        } else { // 多字节符的非首字节,应为 10xxxxxx
            if ((chr & 0xC0) != 0x80)
                return false;
            nBytes--;
        }
    }
    if (nBytes > 0) //违返规则
        return false;

    if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8
        return false;
    return true;
}

std::wstring createPathFromDllPath(const std::wstring& fileName)
{
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path.at(0), 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(g_hModule, &path.at(0), MAX_PATH);
    ::PathRemoveFileSpecW(&path.at(0));

    ::PathAppendW(&path.at(0), fileName.c_str());

    std::wstring result(&path.at(0), wcslen(&path.at(0)));

    if (L'\\' != result[result.size() - 1])
        result += L'\\';

    return result;
}

bool isLocalDebugMachine()
{
    return !!::PathFileExistsW(L"E:\\mycode\\mtmb\\mtmb\\printing\\Printing.cpp");
}

static std::wstring* g_pluginDirectory;

void setPluginDirectory(const std::wstring& dir)
{
    if (0 == dir.size())
        return;

    if (g_pluginDirectory)
        delete g_pluginDirectory;
    g_pluginDirectory = new std::wstring(dir);

    if (g_pluginDirectory->at(g_pluginDirectory->size() - 1) != L'\\')
        *g_pluginDirectory += L'\\';
}

std::wstring getPluginDirectory()
{
    if (g_pluginDirectory)
        return *g_pluginDirectory;
    
    g_pluginDirectory = new std::wstring(createPathFromDllPath(L"plugins\\"));
    return *g_pluginDirectory;
}

}