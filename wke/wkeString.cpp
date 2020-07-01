#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
//////////////////////////////////////////////////////////////////////////

#include <config.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFStringUtil.h>
#include "third_party/WebKit/public/platform/WebString.h"
#include "wke/wkeString.h"
#include "wke/wkeUtil.h"

_jsKeys::~_jsKeys()
{
    for (size_t i = 0; i < length; ++i) {
        char* key = *(char**)(keys + i);
        delete[] key;
    }
    delete keys;
}

namespace wke {

CString::CString(const WTF::String& str)
{
    setString(str);
}

CString::CString(const blink::WebString& str)
{
    setString((String)str);
}

CString::CString(const utf8* str, size_t len /*= 0*/)
{
    setString(str, len);
}

CString::CString(const wchar_t* str, size_t len /*= 0*/)
{
    setString(str, len);
}

CString::~CString()
{

}

void CString::setString(const WTF::String& str)
{
    checkThreadCallIsValid(__FUNCTION__);

    if (str.isNull() || str.isEmpty())
        return;

    if (str.is8Bit()) {
        m_str.resize(str.length());
        memcpy(&m_str.at(0), str.characters8(), str.length());
    } else {
        WTF::WCharToMByte(str.characters16(), str.length(), &m_str, CP_UTF8);
    }

    m_str.push_back('\0');
}

size_t CString::length()
{
	return m_str.size();
}

CString& CString::operator = (const WTF::String& str)
{
    setString(str);
    return *this;
}

CString& CString::operator=(const CString& str)
{
    m_str = str.m_str;
    return *this;
}

const utf8* CString::string() const
{
    if (0 == m_str.size() || 1 == m_str.size())
        return "";

    return &m_str.at(0);
}

const wchar_t* CString::stringW() const
{
    if (0 == m_str.size() || 1 == m_str.size())
        return L"";

    std::vector<UChar> result;
    WTF::MByteToWChar(&m_str.at(0), m_str.size(), &result, CP_UTF8);
    if (0 == result.size())
        return L"";

    return createTempWCharString((const wchar_t*)&result.at(0), result.size());
}

void CString::setString(const utf8* str, size_t len /*= 0*/)
{
    if (!str)
        return;
    if (0 == len)
        len = strlen(str);
    if (0 == len)
        return;

    m_str.resize(len);
    memcpy(&m_str.at(0), str, len);
    m_str.push_back('\0');
}

void CString::setString(const wchar_t* str, size_t len /*= 0*/)
{
    if (!str)
        return;
    if (0 == len)
        len = wcslen(str);
    if (0 == len)
        return;
    WTF::WCharToMByte(str, len, &m_str, CP_UTF8);
    m_str.push_back('\0');
}

void CString::_free()
{

}

std::vector<std::vector<char>*>* s_sharedStringBuffers = nullptr;
std::vector<std::vector<wchar_t>*>* s_sharedStringBuffersW = nullptr;
std::vector<jsKeys*>* s_sharedJsKeys = nullptr;

const char* createTempCharString(const char* str, size_t length)
{
    if (!str || 0 == length)
        return "";
    std::vector<char>* stringBuffer = new std::vector<char>(length + 1);
    memcpy(&stringBuffer->at(0), str, length * sizeof(char));
    stringBuffer->push_back('\0');

    if (!s_sharedStringBuffers)
        s_sharedStringBuffers = new std::vector<std::vector<char>*>();
    s_sharedStringBuffers->push_back(stringBuffer);
    return &stringBuffer->at(0);
}

const wchar_t* createTempWCharString(const wchar_t* str, size_t length)
{
    if (!str || 0 == length)
        return L"";
    std::vector<wchar_t>* stringBuffer = new std::vector<wchar_t>(length + 1);
    memcpy(&stringBuffer->at(0), str, length * sizeof(wchar_t));
    stringBuffer->push_back(L'\0');

    if (!s_sharedStringBuffersW)
        s_sharedStringBuffersW = new std::vector<std::vector<wchar_t>*>();
    s_sharedStringBuffersW->push_back(stringBuffer);
    return &stringBuffer->at(0);
}

jsKeys* createTempJsKeys(size_t length)
{
    if (!s_sharedJsKeys)
        s_sharedJsKeys = new std::vector<jsKeys*>();

    jsKeys* result = new jsKeys();
    result->length = length;
    result->keys = new const char*[length];
    s_sharedJsKeys->push_back(result);
    return result;
}

template<class T>
static void freeShareds(std::vector<T*>* s_shared)
{
    if (!s_shared)
        return;
    
    for (size_t i = 0; i < s_shared->size(); ++i) {
        delete s_shared->at(i);
    }
    s_shared->clear();
}

void freeTempCharStrings()
{
    freeShareds(s_sharedJsKeys);
    freeShareds(s_sharedStringBuffers);
    freeShareds(s_sharedStringBuffersW);
//     if (s_sharedJsKeys) {
//         for (size_t i = 0; i < s_sharedJsKeys->size(); ++i) {
//             delete s_sharedJsKeys->at(i);
//         }
//         s_sharedJsKeys->clear();
//     }
// 
//     if (s_sharedStringBuffers) {
//         for (size_t i = 0; i < s_sharedStringBuffers->size(); ++i) {
//             delete s_sharedStringBuffers->at(i);
//         }
//         s_sharedStringBuffers->clear();
//     }
// 
//     if (s_sharedStringBuffersW) {
//         for (size_t i = 0; i < s_sharedStringBuffersW->size(); ++i) {
//             delete s_sharedStringBuffersW->at(i);
//         }
//         s_sharedStringBuffersW->clear();
//     }
}

};

#endif
