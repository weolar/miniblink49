
#include "common/BindJsQuery.h"
#include "common/LiveIdDetect.h"
#include "common/ThreadCall.h"
#include "content/browser/PostTaskHelper.h"
#include <string>

namespace common {

BindJsQuery::BindJsQuery(int64_t hostId, wkeWebFrameHandle frameId)
{
    m_hostId = hostId;

    m_frameId = frameId;
    strncpy(typeName, "MbQueryFunc", 11);
    propertyGet = nullptr;
    propertySet = nullptr;
    finalize = nullptr;
    callAsFunction = nullptr;

    callAsFunction = jsQueryCallback;
    finalize = jsQueryFinalizeCallback;
}

jsValue WKE_CALL_TYPE BindJsQuery::jsQueryCallback(jsExecState es, jsValue object, jsValue* args, int argCount)
{
    BindJsQuery* self = (BindJsQuery*)::jsGetData(es, object);
    if (3 != argCount)
        return jsUndefined();

    int customMsg = jsToInt(es, args[0]);
    const utf8* request = jsToString(es, args[1]);
    int queryId = jsToInt(es, args[2]);

    void* host = (void*)common::LiveIdDetect::get()->getPtrLocked(self->m_hostId);
    if (!host)
        return jsUndefined();
    common::LiveIdDetect::get()->unlock(self->m_hostId, self);

    self->onJsQueryInBlinkThread(self->m_frameId, customMsg, request, queryId);
    return jsUndefined();
}

void BindJsQuery::onJsQueryInBlinkThread(wkeWebFrameHandle frameId, int customMsg, const utf8* request, int queryId)
{
    int64_t id = m_hostId;
    BindJsQuery* self = this;
    std::string* requestString = new std::string(request);

    std::pair<wkeWebFrameHandle, int>* idInfo = new std::pair<wkeWebFrameHandle, int>(frameId, queryId);

    content::postTaskToUiThread(FROM_HERE, NULL, [self, id, customMsg, requestString, idInfo] {
        void* unuse = common::LiveIdDetect::get()->getPtrLocked(id);
        if (!unuse) {
            delete idInfo;
            delete requestString;
            return;
        }

        //std::function<void(mbJsExecState es, int64_t idInfo, int customMsg, const utf8* request)>* closure = self->getClosure().m_jsQueryClosure;
        QueryFn* closure = self->m_closure;
        if (closure)
            (*closure)((mbJsExecState)nullptr, (int64_t)idInfo, customMsg, requestString->c_str());
        delete requestString;

        common::LiveIdDetect::get()->unlock(id, unuse);
    });
}

void WKE_CALL_TYPE BindJsQuery::jsQueryFinalizeCallback(jsData* data)
{
    delete (BindJsQuery*)data;
}

static const char* injectScript = 
"window.__g_callbackMap__ = {};\n"
"window.__g_callbackMapIdGen__ = 0;\n"
"window.__onMbQuery__ = function(id, customMsg, response) {\n"
"    var cb = window.__g_callbackMap__[id];\n"
"    //console.log('__onMbQuery__ cb:' + customMsg);\n"
"    if (cb) {\n"
"        cb(customMsg, response);\n"
"        delete window.__g_callbackMap__[id];\n"
"    }\n"
"}\n"
"window.__setMbQuery__ = function(func) {\n"
"    window.mbQuery = function(customMsg, request, cb) {\n"
"        var id = -1\n"
"        if ('function' == typeof cb) {\n"
"            id = ++window.__g_callbackMapIdGen__;"
"            window.__g_callbackMap__[id] = cb;\n"
"        }\n"
"        func(customMsg, request, id);\n"
"        //console.log('mbQuery cb:' + typeof cb);\n"
"    }\n"
"}\n"
"function __NumberFormat__() {}\n"
"__NumberFormat__.prototype.resolvedOptions = function() { \n"
"    return {'locale': 'zh-cn'}; \n"
"}\n"
"__NumberFormat__.prototype.format = function(num) { return num; }\n"
"__NumberFormat__.supportedLocalesOf = function(locales, options) { return ['zh-cn']; }\n"
// "window.Intl = {};\n"
// "Intl.NumberFormat = __NumberFormat__;"
// "Intl.DateTimeFormat = {};\n"
// "Intl.DateTimeFormat.supportedLocalesOf = function(loc) { return ['zh-cn']; };\n"
;

void BindJsQuery::bindFun(int64_t hostId, QueryFn* queryFn, wkeWebView webView, wkeWebFrameHandle frameId)
{
    jsExecState es = wkeGetGlobalExecByFrame(webView, frameId);
    wkeRunJsByFrame(webView, frameId, injectScript, false);

    BindJsQuery* jsQueryFunc = new BindJsQuery(hostId, frameId);
    jsValue jsObj = ::jsFunction(es, jsQueryFunc);

    jsQueryFunc->m_closure = queryFn;

    jsValue jsArgs[1] = { jsObj };
    ::jsCallGlobal(es, ::jsGetGlobal(es, "__setMbQuery__"), jsArgs, 1);
}

}