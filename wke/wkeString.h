#ifndef WKE_STRING_H
#define WKE_STRING_H

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#include "wkedefine.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include <vector>

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
    //const WTF::String& original() const;
    
    void setString(const utf8* str, size_t len = 0);
    void setString(const wchar_t* str, size_t len = 0);
    void setString(const WTF::String& str);

    size_t length();
protected:
    void _free();

    std::vector<char> m_str;
};

const char* createTempCharString(const char* str, size_t length);
const wchar_t* createTempWCharString(const wchar_t* str, size_t length);
jsKeys* createTempJsKeys(size_t length);
void freeTempCharStrings();

};

#endif
#endif // WKE_STRING_H