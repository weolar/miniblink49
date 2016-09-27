
#include "wtf/text/WTFStringUtil.h"
#include "wtf/text/UTF8.h"

namespace WTF {

String ensureUTF16String(const String& string)
{
    if (string.isNull() || string.isEmpty())
        return String(L"");
    if (!string.is8Bit())
        return String(string.characters16(), string.length());

    const LChar* stringStart = string.characters8();
    size_t length = string.length();
    if (charactersAreAllASCII(stringStart, length))
        return String::make16BitFrom8BitSource(stringStart, length);

    Vector<UChar, 1024> buffer(length);
    UChar* bufferStart = buffer.data();

    UChar* bufferCurrent = bufferStart;
    const char* stringCurrent = reinterpret_cast<const char*>(stringStart);
    if (WTF::Unicode::convertUTF8ToUTF16(&stringCurrent, reinterpret_cast<const char *>(stringStart + length),
        &bufferCurrent, bufferCurrent + buffer.size()) != WTF::Unicode::conversionOK)
        return String();

    unsigned utf16Length = bufferCurrent - bufferStart;
    ASSERT(utf16Length < length);
    return StringImpl::create(bufferStart, utf16Length);
}

Vector<UChar> ensureUTF16UChar(const String& string)
{
    String out = ensureUTF16String(string);
    return out.charactersWithNullTermination();
}

// 如果string里是8bit的话，必须是utf8编码
Vector<UChar> ensureStringToUChars(const String& string)
{
    if (string.isNull() || string.isEmpty())
        return Vector<UChar>();

    if (!string.is8Bit())
        return string.charactersWithNullTermination();

    //ASSERT(WTF::Unicode::isLegalUTF8(string.characters8(), string.length()));
    if (string.containsOnlyASCII())
        return string.charactersWithNullTermination();

    String retVal = String::fromUTF8(string.characters8(), string.length());
    if (retVal.isNull() || retVal.isEmpty())
        return Vector<UChar>();

    ASSERT(!retVal.is8Bit());
    return retVal.charactersWithNullTermination();
}

Vector<char> ensureStringToUTF8(const String& string)
{
    Vector<char> out;
    if (string.isNull() || string.isEmpty())
        return out;
    if (string.is8Bit()) {
        out.resize(string.length());
        memcpy(out.data(), string.characters8(), string.length());
        out.append('\0');
    } else {
        CString utf8 = string.utf8();
        out.resize(utf8.length());
        memcpy(out.data(), utf8.data(), utf8.length());
    }

    return out;
}

String ensureStringToUTF8String(const String& string)
{
    Vector<char> out = ensureStringToUTF8(string);
    return String(out.data(), out.size());
}

static bool isWhiteSpace(UChar c)
{
    return L' ' == c || L'\r' == c || L'\n' == c;
}

void stringTrim(String& stringInOut, bool leftTrim, bool rightTrim)
{
    if (stringInOut.isNull() || stringInOut.isEmpty())
        return;

    if (leftTrim) {
        while (!stringInOut.isNull() && !stringInOut.isEmpty()) {
            UChar c = stringInOut[0];
            if (!isWhiteSpace(c))
                break;

            stringInOut.remove(0);
        }
    }

    if (rightTrim) {
        while (!stringInOut.isNull() && !stringInOut.isEmpty()) {
            UChar c = stringInOut[stringInOut.length() - 1];
            if (!isWhiteSpace(c))
                break;

            stringInOut.remove(stringInOut.length() - 1);
        }
    }
}

bool splitStringToVector(const String& strData, const char strSplit, bool needTrim, WTF::Vector<String>& out)
{
    ASSERT(strData.is8Bit());
    size_t nIndex = WTF::kNotFound;
    size_t nStartIndex = 0;
    size_t nCount = 0;
    String strItem;
    String strBuf = strData;

    out.clear();
    strBuf = strData;

    do {
        nIndex = strBuf.find(strSplit, nStartIndex);
        if (WTF::kNotFound == nIndex) {
            if (nStartIndex <= strBuf.length()) {
                nCount = strBuf.length() - nStartIndex;
                strItem = strBuf.substring(nStartIndex, nCount);
                if (strItem.isNull() || strItem.isEmpty())
                    continue;
                stringTrim(strItem, needTrim, needTrim);

                out.append(strItem);
            }
            break;
        }

        nCount = nIndex - nStartIndex;
        strItem = strBuf.substring(nStartIndex, nCount);
        stringTrim(strItem, needTrim, needTrim);
        out.append(strItem);

        nStartIndex = nIndex + 1;
        if (nStartIndex > strBuf.length())
            break;
    } while (nIndex != WTF::kNotFound);

    return true;
}

std::string WTFStringToStdString(const WTF::String& str)
{
    CString utf8 = str.utf8();
    std::string result(utf8.data());
    return result;
}

bool isTextUTF8(const char *str, int length)
{
	int i = 0;
	DWORD nBytes = 0; // UFT8可用1-6个字节编码,ASCII用一个字节
	UCHAR chr = 0;
	bool bAllAscii = true; // 如果全部都是ASCII, 说明不是UTF-8
	for (i = 0; i < length; i++) {
		chr = (UCHAR) * (str + i);

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


} // WTF