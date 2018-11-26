
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/common/StringUtil.h"

#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"

namespace cef {

void String16ToCefOriginalString16(const char16* src, cef_string_userfree_utf16_t output) {
    output->dtor = nullptr;
    output->str = nullptr;
    output->length = 0;

    size_t srcLen = wcslen(src);
    cef_string_userfree_utf16_t buf = cef_string_userfree_utf16_alloc();
    cef_string_utf16_set(src, srcLen, buf, true);

    output->dtor = buf->dtor;
    output->str = buf->str;
    output->length = buf->length;
}

void WebStringToCefStringUTF16(const blink::WebString& src, CefStringUTF16& output) {
    WebStringToCefString(src, output);
}

void WebStringToCefString(const blink::WebString& src, CefString& output) {
    output.clear();
    if (src.isNull() || src.isEmpty())
        return;

    WTFStringToCefString(src, output);
}

void WTFStringToCefString(const WTF::String& src, CefString& output) {
    output.clear();
    if (src.isNull() || src.isEmpty())
        return;

    if (!src.is8Bit()) {
        output.FromString(src.characters16(), src.length(), true);
        return;
    }

    Vector<UChar> buffer = ensureUTF16UChar(src, false);
    output.FromString(buffer.data(), buffer.size(), true);
}

} // cef

#endif
