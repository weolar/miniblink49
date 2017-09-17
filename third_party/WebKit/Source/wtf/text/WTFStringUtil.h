
#ifndef WTFStringUtil_h
#define WTFStringUtil_h

#include <string>
#include <vector>
#include "wtf/text/WTFString.h"

namespace WTF {

String ensureUTF16String(const String& string);
Vector<UChar> ensureUTF16UChar(const String& string, bool isNullTermination);
//Vector<UChar> ensureStringToUChars(const String& string);
Vector<char> ensureStringToUTF8(const String& string, bool isNullTermination);
String ensureStringToUTF8String(const String& string);
std::string WTFStringToStdString(const WTF::String& str);

bool isTextUTF8(const char *str, int length);

bool splitStringToVector(const String& strData, const char strSplit, bool needTrim, WTF::Vector<String>& out);

void MByteToWChar(LPCSTR lpcszStr, DWORD cbMultiByte, std::vector<UChar>* out, UINT codePage);
void WCharToMByte(LPCWSTR lpWideCharStr, DWORD cchWideChar, std::vector<char>* out, UINT codePage);

}

#endif // WTFStringUtil_h