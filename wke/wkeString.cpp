#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
//////////////////////////////////////////////////////////////////////////

#include <config.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFStringUtil.h>
#include "third_party/WebKit/public/platform/WebString.h"

//cexer: 必须包含在后面，因为其中的 windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wkeString.h"

//////////////////////////////////////////////////////////////////////////

namespace wke {

CString::CString(const WTF::String& str)
    : m_string(str)
    , m_utf8(NULL)
    , m_wide(NULL)
{

}

CString::CString(const blink::WebString& str)
    : m_string(str)
    , m_utf8(NULL)
    , m_wide(NULL)
{

}

CString::CString(const utf8* str, size_t len /*= 0*/)
    : m_utf8(NULL)
    , m_wide(NULL)
{
    m_string = WTF::String::fromUTF8(str, len);
}

CString::CString(const wchar_t* str, size_t len /*= 0*/)
    : m_utf8(NULL)
    , m_wide(NULL)
{
    WTF::String(str, len).swap(m_string);
}

CString::~CString()
{
    _free();
}

CString& CString::operator = (const WTF::String& str)
{
    if (&m_string != &str) {
        _dirty();
        m_string = str;
    }
    return *this;
}

CString& CString::operator=(const CString& str)
{
    return operator=(str.m_string);
}

const utf8* CString::string() const
{
    if (!m_utf8) {
        Vector<char> wtfUtf8 = WTF::ensureStringToUTF8(m_string, false);
        size_t wtfUtf8Len = wtfUtf8.size();
        if (0 == wtfUtf8.size())
            return "";

        m_utf8 = new utf8[wtfUtf8Len + 1];
        if (wtfUtf8Len != 0)
            memcpy(m_utf8, wtfUtf8.data(), wtfUtf8Len);

        m_utf8[wtfUtf8Len] = 0;
    }

    return m_utf8;
}

const wchar_t* CString::stringW() const
{
    if (!m_wide) {
        Vector<UChar> stringBuf = WTF::ensureUTF16UChar(m_string, false);
        if (0 == stringBuf.size())
            return L"";

        const wchar_t* wtfWide = stringBuf.data();
        size_t wtfWideLen = stringBuf.size();

        m_wide = new wchar_t[wtfWideLen + 1];
        memcpy(m_wide, wtfWide, (wtfWideLen + 1)* sizeof(wchar_t));

        m_wide[wtfWideLen] = 0;
    }

    return m_wide;
}

const WTF::String& CString::original() const
{
    return m_string;
}

void CString::setString(const utf8* str, size_t len /*= 0*/)
{
    _dirty();
    m_string = WTF::String::fromUTF8(str, len);
}

void CString::setString(const wchar_t* str, size_t len /*= 0*/)
{
    _dirty();
    WTF::String(str, len).swap(m_string);
}

void CString::_dirty()
{
    _free();
}

void CString::_free()
{
    if (m_wide) {
        delete [] m_wide;
        m_wide = NULL;
    }

    if (m_utf8)
    {
        delete [] m_utf8;
        m_utf8 = NULL;
    }
}

std::vector<std::vector<char>*>* s_sharedStringBuffers = nullptr;
std::vector<std::vector<wchar_t>*>* s_sharedStringBuffersW = nullptr;

const char* createTempCharString(const char* str, size_t length)
{
    if (!str || 0 == length)
        return "";
    std::vector<char>* stringBuffer = new std::vector<char>(length);
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
    std::vector<wchar_t>* stringBuffer = new std::vector<wchar_t>(length);
    memcpy(&stringBuffer->at(0), str, length * sizeof(wchar_t));
    stringBuffer->push_back(L'\0');

    if (!s_sharedStringBuffersW)
        s_sharedStringBuffersW = new std::vector<std::vector<wchar_t>*>();
    s_sharedStringBuffersW->push_back(stringBuffer);
    return &stringBuffer->at(0);
}

void freeTempCharStrings()
{
    if (s_sharedStringBuffers) {
        for (size_t i = 0; i < s_sharedStringBuffers->size(); ++i) {
            delete s_sharedStringBuffers->at(i);
        }
        s_sharedStringBuffers->clear();
    }

    if (s_sharedStringBuffersW) {
        for (size_t i = 0; i < s_sharedStringBuffersW->size(); ++i) {
            delete s_sharedStringBuffersW->at(i);
        }
        s_sharedStringBuffersW->clear();
    }
}

};

#endif
