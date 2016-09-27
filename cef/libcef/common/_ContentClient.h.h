
#ifndef CEF_LIBCEF_COMMON_CONTENT_CLIENT_H_
#define CEF_LIBCEF_COMMON_CONTENT_CLIENT_H_

#include "include/cef_app.h"

#include "base/compiler_specific.h"
#include "third_party/WebKit/Source/wtf/Vector.h"

class CefContentClient {
public:
    explicit CefContentClient(CefRefPtr<CefApp> application);
    ~CefContentClient();

    // Returns the singleton CefContentClient instance.
    static CefContentClient* Get();

    CefRefPtr<CefApp> application() const { return m_application; }

private:
    struct SchemeInfo {
        std::string scheme_name;

        // Registers a non-HTTP URL scheme which can be sent CORS requests.
        bool is_standard;

        // Registers a URL scheme that can be saved to disk.
        bool is_savable;

        // Registers a URL scheme to be treated as a local scheme (i.e., with the
        // same security rules as those applied to "file" URLs). This means that
        // normal pages cannot link to or access URLs of this scheme.
        bool is_local;

        // Registers a URL scheme to be treated as display-isolated. This means
        // that pages cannot display these URLs unless they are from the same
        // scheme. For example, pages in other origin cannot create iframes or
        // hyperlinks to URLs with the scheme. For schemes that must be accessible
        // from other schemes set this value to false and use CORS
        // "Access-Control-Allow-Origin" headers to further restrict access.
        bool is_display_isolated;
    };
    typedef WTF::Vector<SchemeInfo*> SchemeInfoList;

    CefRefPtr<CefApp> m_application;

    // Custom schemes handled by the client.
    SchemeInfoList m_schemeInfoList;
    bool m_bSchemeInfoListLocked;

    //WTF::Vector<WTF::String> m_standardSchemes;
};

#endif // CEF_LIBCEF_COMMON_CONTENT_CLIENT_H_
