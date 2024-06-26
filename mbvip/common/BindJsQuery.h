
#ifndef common_BindJsQuery_h
#define common_BindJsQuery_h

#include "wke/wkedefine.h"
#include "core/mb.h"
#include <functional>

namespace common {

class BindJsQuery : public jsData {
public:
    typedef std::function<void(mbJsExecState es, int64_t idInfo, int customMsg, const utf8* request)> QueryFn;

    BindJsQuery(int64_t hostId, wkeWebFrameHandle frameId);

    static jsValue WKE_CALL_TYPE jsQueryCallback(jsExecState es, jsValue object, jsValue* args, int argCount);
    void onJsQueryInBlinkThread(wkeWebFrameHandle frameId, int customMsg, const utf8* request, int queryId);
    static void WKE_CALL_TYPE jsQueryFinalizeCallback(jsData* data);

    static void bindFun(int64_t hostId, QueryFn* queryFn, wkeWebView webView, wkeWebFrameHandle frameId);

private:
    int64_t m_hostId;
    wkeWebFrameHandle m_frameId;
    QueryFn* m_closure;
};

}

#endif // common_BindJsQuery_h