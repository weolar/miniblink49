#ifndef WKE_STRING_H
#define WKE_STRING_H

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#include <config.h>
#include <wtf/text/WTFString.h>
#include "third_party/WebKit/public/platform/WebString.h"

//cexer: 必须包含在后面，因为其中的 windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wke.h"

//////////////////////////////////////////////////////////////////////////

namespace wke {

class CString {
public:
    CString(const WTF::String& str);
    CString(const blink::WebString& str);
    CString(const utf8* str, size_t len);
    CString(const wchar_t* str, size_t len);
    ~CString();

    CString& operator=(const WTF::String& str);
    CString& operator=(const CString& that);

public:
    const utf8* string() const;
    const wchar_t* stringW() const;
    const WTF::String& original() const;
    
    void setString(const utf8* str, size_t len = 0);
    void setString(const wchar_t* str, size_t len = 0);

protected:
    void _free();
    void _dirty();

    WTF::String m_string;
    mutable utf8* m_utf8;
    mutable wchar_t* m_wide;
};

const char* createTempCharString(const char* str, size_t length);
const wchar_t* createTempWCharString(const wchar_t* str, size_t length);
void freeTempCharStrings();

};

#endif
#endif // WKE_STRING_H