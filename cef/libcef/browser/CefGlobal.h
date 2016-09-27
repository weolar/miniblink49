
#ifndef CefGlobal_h
#define CefGlobal_h

typedef struct _cef_resource_bundle_handler_t cef_resource_bundle_handler_t;
typedef struct _cef_browser_process_handler_t cef_browser_process_handler_t;
typedef struct _cef_render_process_handler_t cef_render_process_handler_t;
typedef struct _cef_client_t cef_client_t;
typedef struct _cef_browser_t cef_browser_t;
typedef struct _cef_browser_t cef_browser_t;

class CefGlobal {
public:
    CefGlobal() {
        m_mainArgs = nullptr;
        m_resourceBundleHandler = nullptr;
        m_browserProcessHandler = nullptr;
        m_renderProcessHandler = nullptr;
        m_browser = nullptr;
    }
    const cef_main_args_t* m_mainArgs;

    cef_resource_bundle_handler_t* m_resourceBundleHandler;
    cef_browser_process_handler_t* m_browserProcessHandler;
    cef_render_process_handler_t* m_renderProcessHandler;

    //cef_client_t* m_client;
    cef_browser_t* m_browser;
};

extern CefGlobal* g_cefGlobal;

#endif // CefGlobal_h