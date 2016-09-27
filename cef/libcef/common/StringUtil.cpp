
#include "libcef/common/StringUtil.h"

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
    if (src.isNull() || src.isEmpty())
        return;
    DebugBreak();
//     WTF::String srcSting = src;
//     if (srcSting.is8Bit())
//         srcSting = String::fromUTF8(srcSting.characters8(), srcSting.length());
// 
//     size_t srcLen = srcSting.length();
//     CefStringUTF16::FromString(srcSting.characters16(), srcLen, true);
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

    WTF::String srcCopy = src;
    const LChar* characters8 = srcCopy.characters8();
    unsigned length = srcCopy.length();
    if (charactersAreAllASCII(characters8, length))
        srcCopy.ensure16Bit();
    else
        srcCopy = String::fromUTF8(characters8, length);

    ASSERT(!srcCopy.is8Bit());
    output.FromString(srcCopy.characters16(), srcCopy.length(), true);
}

} // cef