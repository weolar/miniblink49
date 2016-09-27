#include "libcef/browser/CefContext.h"

bool CefCurrentlyOn(CefThreadId threadId) {
    return CefContext::Get()->CurrentlyOn(threadId);
}