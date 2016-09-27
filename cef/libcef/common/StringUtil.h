
#include "include/internal/cef_string_types.h"
#include "include/internal/cef_string_wrappers.h"
#include "include/internal/cef_string.h"
#include "third_party/WebKit/public/platform/WebString.h"

namespace cef {

void String16ToCefOriginalString16(const char16* src, cef_string_userfree_utf16_t output);
void WebStringToCefStringUTF16(const blink::WebString& src, CefStringUTF16& output);
void WebStringToCefString(const blink::WebString& src, CefString& output);
void WTFStringToCefString(const WTF::String& src, CefString& output);

}