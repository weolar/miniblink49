#ifndef CEF_LIBCEF_COMMON_CEF_MESSAGES_H_
#define CEF_LIBCEF_COMMON_CEF_MESSAGES_H_

#include "base/values.h"
//#include <string>
#include "third_party/WebKit/Source/wtf/text/WTFString.h"

struct Cef_Request_Params {
    int64 frameId;

    // Message name.
    WTF::String name;

    // List of message arguments.
    base::ListValue arguments;
};

struct Cef_Response_Params {
    // True on success.
    bool success;

    // Response or error string depending on the value of |success|.
    WTF::String response;
};

#endif  // CEF_LIBCEF_COMMON_CEF_MESSAGES_H_