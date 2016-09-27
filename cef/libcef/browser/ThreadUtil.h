
#ifndef CEF_LIBCEF_BROWSER_THREAD_UTIL_H_
#define CEF_LIBCEF_BROWSER_THREAD_UTIL_H_

#include "include/wrapper/cef_helpers.h"

#define CEF_CURRENTLY_ON(id) CefCurrentlyOn(id)
#define CEF_CURRENTLY_ON_UIT() CEF_CURRENTLY_ON(TID_UI)
#define CEF_CURRENTLY_ON_IOT() CEF_CURRENTLY_ON(TID_IO)
#define CEF_CURRENTLY_ON_FILET() CEF_CURRENTLY_ON(TID_FILE)
#define CEF_CURRENTLY_ON_RT() CEF_CURRENTLY_ON(TID_UI)

#define CEF_REQUIRE(id) ASSERT(CEF_CURRENTLY_ON(id))
#define CEF_REQUIRE_UIT() CEF_REQUIRE(TID_UI)
#define CEF_REQUIRE_IOT() CEF_REQUIRE(TID_IO)
#define CEF_REQUIRE_RT() ASSERT(CEF_CURRENTLY_ON_RT())

#define CEF_REQUIRE_FILET() CEF_REQUIRE(TID_FILE)

#define CEF_UIT TID_UI

#define CEF_POST_TASK(id, task) \
    CefPostTask(id, task)

#define CEF_POST_DELAYED_TASK(id, task, delay_ms) \
    CefPostDelayedTask(id, task, (delay_ms))

#define CEF_POST_BLINK_TASK(id, task) \
    CefPostBlinkTask(FROM_HERE, id, task)

#define CEF_POST_BLINK_DELAYED_TASK(id, task, delay_ms) \
    CefPostBlinkDelayedTask(FROM_HERE, id, task, (delay_ms))

#endif // CEF_LIBCEF_BROWSER_THREAD_UTIL_H_