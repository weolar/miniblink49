#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/browser/CefContext.h"

bool CefCurrentlyOn(CefThreadId threadId) {
    return CefContext::Get()->CurrentlyOn(threadId);
}
#endif
