
#include "renderer/WebviewPlugin.h"
#include "renderer/WebviewPluginImpl.h"

#include "base/values.h"
#include <stdlib.h>
#include <string>

namespace atom {

// Browser function table，可以通过它来得到浏览器提供的功能
NPNetscapeFuncs* g_npBrowserFunctions = nullptr;

/*******各种接口的声明*********/
//在NPAPI编程的接口中你会发现有NP_打头的，有NPP_打头的，有NPN_打头的
//NP是npapi的插件库提供给浏览器的最上层的接口
//NPP即NP Plugin，是插件本身提供给浏览器调用的接口，主要被用来填充NPPluginFuncs的结构体
//NPN即NP Netscape，是浏览器提供给插件使用的接口，这些接口一般都在NPNetscapeFuncs结构体中

//NPP Functions
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved);
NPError NPP_Destroy(NPP instance, NPSavedData** save);
NPError NPP_SetWindow(NPP instance, NPWindow* window);
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
int32 NPP_WriteReady(NPP instance, NPStream* stream);
int32 NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer);
void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void NPP_Print(NPP instance, NPPrint* platformPrint);
int16_t NPP_HandleEvent(NPP instance, void* event);
void NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData);
NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value);
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value);

//Functions for scriptablePluginClass
NPObject* pluginAllocate(NPP npp, NPClass *aClass);
void pluginDeallocate(NPObject *npobj);
bool pluginHasMethod(NPObject *obj, NPIdentifier methodName);
bool pluginInvoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool hasProperty(NPObject *obj, NPIdentifier propertyName);
bool getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result);

struct NPObjectEx : public NPObject {
    WebviewPluginImpl* impl;
};

static struct NPClass scriptablePluginClass = {
    NP_CLASS_STRUCT_VERSION,
    pluginAllocate,
    pluginDeallocate,
    NULL,
    pluginHasMethod,
    pluginInvoke,
    NULL,
    hasProperty,
    getProperty,
    NULL,
    NULL,
};

//接口的实现
NPError __stdcall Webview_NP_Initialize(NPNetscapeFuncs* browserFuncs) {
    g_npBrowserFunctions = browserFuncs;
    return NPERR_NO_ERROR;
}

NPError __stdcall Webview_NP_GetEntryPoints(NPPluginFuncs* pluginFuncs) {
    pluginFuncs->version = 11;
    pluginFuncs->size = sizeof(pluginFuncs);
    pluginFuncs->newp = NPP_New;
    pluginFuncs->destroy = NPP_Destroy;
    pluginFuncs->setwindow = NPP_SetWindow;
    pluginFuncs->newstream = NPP_NewStream;
    pluginFuncs->destroystream = NPP_DestroyStream;
    pluginFuncs->asfile = NPP_StreamAsFile;
    pluginFuncs->writeready = NPP_WriteReady;
    pluginFuncs->write = (NPP_WriteProcPtr)NPP_Write;
    pluginFuncs->print = NPP_Print;
    pluginFuncs->event = NPP_HandleEvent;
    pluginFuncs->urlnotify = NPP_URLNotify;
    pluginFuncs->getvalue = NPP_GetValue;
    pluginFuncs->setvalue = NPP_SetValue;

    return NPERR_NO_ERROR;
}

void __stdcall Webview_NP_Shutdown(void) {
}

static void npVariantToListValue(WebviewPluginImpl* impl, const NPVariant& arg, base::ListValue* listParams);

static void npVariantTypeObjectToDirValue(WebviewPluginImpl* impl, NPObject* objectValue, base::DictionaryValue* dirParams) {
    NPIdentifier* identifier = nullptr;
    uint32_t count = 0;
    NPP instance = impl->getInstance();
    g_npBrowserFunctions->enumerate(instance, objectValue, &identifier, &count);

    for (uint32_t i = 0; i < count; ++i) {
        char* method = g_npBrowserFunctions->utf8fromidentifier(identifier[i]);
        NPVariant result;
        g_npBrowserFunctions->getproperty(instance, objectValue, identifier[i], &result);

        switch (result.type) {
        case NPVariantType_Bool:
            dirParams->SetBoolean(method, result.value.boolValue);
            break;
        case NPVariantType_Int32:
            dirParams->SetInteger(method, result.value.intValue);
            break;
        case NPVariantType_Double:
            dirParams->SetDouble(method, result.value.doubleValue);
            break;
        case NPVariantType_String:
            dirParams->SetString(method, std::string(result.value.stringValue.UTF8Characters, result.value.stringValue.UTF8Length));
            break;

        case NPVariantType_Object:
            base::DictionaryValue* dirParamsObj = new base::DictionaryValue();
            npVariantTypeObjectToDirValue(impl, result.value.objectValue, dirParamsObj);
            dirParams->Set(method, dirParamsObj);
            break;
        }
        
        g_npBrowserFunctions->releasevariantvalue(&result);
        g_npBrowserFunctions->memfree(method);
    }
    g_npBrowserFunctions->memfree(identifier);
}

void npVariantToListValue(WebviewPluginImpl* impl, const NPVariant& arg, base::ListValue* listParams) {
    switch (arg.type) {
    case NPVariantType_Bool:
        listParams->AppendBoolean(arg.value.boolValue);
        break;
    case NPVariantType_Int32:
        listParams->AppendInteger(arg.value.intValue);
        break;
    case NPVariantType_Double:
        listParams->AppendDouble(arg.value.doubleValue);
        break;
    case NPVariantType_String:
        listParams->AppendString(std::string(arg.value.stringValue.UTF8Characters, arg.value.stringValue.UTF8Length));
        break;

    case NPVariantType_Object:
        base::DictionaryValue* dirParams = new base::DictionaryValue();
        npVariantTypeObjectToDirValue(impl, arg.value.objectValue, dirParams);
        listParams->Append(dirParams);
        break;
    }
}

static void toResultString(const utf8* str, NPVariant* result) {
    size_t length = strlen(str);
    utf8* strBuffer = (utf8*)g_npBrowserFunctions->memalloc(length);
    memcpy(strBuffer, str, length);
    STRINGN_TO_NPVARIANT(strBuffer, length, *result);
}

bool pluginHasMethod(NPObject* obj, NPIdentifier methodName) {
    // This function will be called when we invoke method on this plugin elements.
    NPUTF8* name = g_npBrowserFunctions->utf8fromidentifier(methodName);
    bool result = false;

    const char* allowList[] = {
        "native_getURL",
        //"native_loadURL",
        "native_getTitle",
        "native_isLoading",
        "native_isLoadingMainFrame",
        "native_isWaitingForResponse",
        "native_stop",
        "native_reload",
        "native_reloadIgnoringCache",
        "native_canGoBack",
        "native_canGoForward",
        "native_canGoToOffset",
        "native_clearHistory",
        "native_goBack",
        "native_goForward",
        "native_goToIndex",
        "native_goToOffset",
        "native_isCrashed",
        "native_setUserAgent",
        "native_getUserAgent",
        "native_openDevTools",
        "native_closeDevTools",
        "native_isDevToolsOpened",
        "native_isDevToolsFocused",
        "native_inspectElement",
        "native_setAudioMuted",
        "native_isAudioMuted",
        "native_undo",
        "native_redo",
        "native_cut",
        "native_copy",
        "native_paste",
        "native_pasteAndMatchStyle",
        "native_delete",
        "native_selectAll",
        "native_unselect",
        "native_replace",
        "native_replaceMisspelling",
        "native_findInPage",
        "native_stopFindInPage",
        "native_getId",
        "native_downloadURL",
        "native_inspectServiceWorker",
        "native_print",
        "native_printToPDF",
        "native_showDefinitionForSelection",
        "native_capturePage",
        "native_insertCSS",
        "native_insertText",
        "native_send",
        "native_sendInputEvent",
        "native_setZoomFactor",
        "native_setZoomLevel",
        "native_setZoomLevelLimits",
        "native_executeJavaScript",
        nullptr
    };

    for (int i = 0; allowList[i]; ++i) {
        if (0 != strcmp(name, allowList[i]))
            continue;
        result = true;
        break;
    }

    g_npBrowserFunctions->memfree(name);
    return result;
}

static bool doExecuteJavaScript(WebviewPluginImpl* impl, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    std::string codeText = std::string(args[0].value.stringValue.UTF8Characters, args[0].value.stringValue.UTF8Length);
    bool hasUserGesture = args[1].value.boolValue;

    wkeWebView webview = impl->getWebview();
    wkeWebFrameHandle mainFrame = wkeWebFrameGetMainFrame(webview);
    jsValue ret = wkeRunJsByFrame(webview, mainFrame, codeText.c_str(), true);
    jsExecState es = wkeGlobalExec(webview);
    jsType type = jsTypeOf(ret);
    jsValue func = wkeRunJsByFrame(webview, mainFrame, "return function(args){ return JSON.stringify({\"ret\": args}); }", true);
    jsValue json = jsCall(es, func, jsUndefined(), &ret, 1);
    const utf8* jsonStr = jsToString(es, json);
    toResultString(jsonStr, result);
    return true;
}

bool pluginInvoke(NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    char* methodNameBuffer = g_npBrowserFunctions->utf8fromidentifier(methodName);
    std::string method = methodNameBuffer;
    g_npBrowserFunctions->memfree(methodNameBuffer);

    NPObjectEx* objex = (NPObjectEx*)obj;
    WebviewPluginImpl* impl = objex->impl;
    if ("native_loadURL" == method) {
        const NPString& url = (args[0].value.stringValue);
        const NPString& httpReferrer = (args[1].value.stringValue);
        const NPString& userAgent = (args[2].value.stringValue);
        const NPString& extraHeaders = (args[3].value.stringValue);

        std::string urlString(url.UTF8Characters, url.UTF8Length);
        std::string httpReferrerString(httpReferrer.UTF8Characters, httpReferrer.UTF8Length);
        std::string userAgentString(userAgent.UTF8Characters, userAgent.UTF8Length);
        std::string extraHeadersString(extraHeaders.UTF8Characters, extraHeaders.UTF8Length);

        impl->loadURL(urlString, httpReferrerString, userAgentString, extraHeadersString);
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }

    if ("native_getURL" == method) {
        impl->getURL();
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }

    if ("native_setPreloadURL" == method) {
        const NPString& preloadURL = (args[0].value.stringValue);
        if (preloadURL.UTF8Characters && preloadURL.UTF8Length) {
            std::string preloadURLString(preloadURL.UTF8Characters, preloadURL.UTF8Length);
            impl->setPreloadURL(preloadURLString);
        }
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }

    if ("native_setId" == method) {
        impl->setGuestId((int)(args[0].value.doubleValue));
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }
    if ("native_setSize" == method) {
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }

    if ("native_send" == method) {
        std::string channel;
        if (argCount >= 1) {
            if (NPVariantType_String != args[0].type)
                return false;
            channel = std::string(args[0].value.stringValue.UTF8Characters, args[0].value.stringValue.UTF8Length);
        }

        base::ListValue listParams;
        NPObject* objectValue = nullptr;
        for (uint32_t i = 1; i < argCount; ++i) {
            const NPVariant& arg = args[i];
            npVariantToListValue(impl, arg, &listParams);
        }

        impl->hostSendMessageToGuest(channel, listParams);
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }

    if ("native_getTitle" == method) {
        const utf8* titile = wkeGetTitle(impl->getWebview());
        toResultString(titile, result);
        return true;
   
    }
    if ("native_isLoading" == method) {
        bool b = wkeIsLoading(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(b, *result);
        return true;
    }
    if ("native_isLoadingMainFrame" == method) {
        bool b = wkeIsLoading(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(b, *result);
        return true;
    }
    if ("native_stop" == method) {
        wkeStopLoading(impl->getWebview());
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }
    if ("native_reload" == method || "native_reloadIgnoringCache" == method) {
        wkeReload(impl->getWebview());
        INT32_TO_NPVARIANT(0, *result);
        return true;
    }
    if ("native_canGoBack" == method) {
        bool b = wkeCanGoBack(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(b, *result);
        return true;
    }
    if ("native_canGoForward" == method) {
        bool b = wkeCanGoForward(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(b, *result);
        return true;
    }
    if ("native_canGoToOffset" == method) {
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_clearHistory" == method) {
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_goBack" == method) {
        wkeCanGoBack(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_goForward" == method) {
        wkeCanGoForward(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_goToIndex" == method || 
        "native_goToOffset" == method || 
        "native_isCrashed" == method ||
        "native_openDevTools" == method ||
        "native_closeDevTools" == method ||
        "native_inspectElement" == method ||
        "native_setAudioMuted" == method ||
        "native_isAudioMuted" == method
        ) {
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_setUserAgent" == method) {
        if (argCount >= 1 && NPVariantType_String == args[0].type) {
            std::string  ua = std::string(args[0].value.stringValue.UTF8Characters, args[0].value.stringValue.UTF8Length);
            wkeSetUserAgent(impl->getWebview(), ua.c_str());
        }       
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_getUserAgent" == method) {
        DebugBreak();
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_isDevToolsOpened" == method) {
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_isDevToolsFocused" == method) {
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_undo" == method) {
        wkeEditorUndo(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_redo" == method) {
        wkeEditorRedo(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_cut" == method) {
        wkeEditorCut(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_copy" == method) {
        wkeEditorCopy(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_paste" == method || "native_pasteAndMatchStyle" == method) {
        wkeEditorCopy(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_delete" == method) {
        wkeEditorDelete(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("selectAll" == method) {
        wkeEditorSelectAll(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_unselect" == method) {
        wkeEditorUnSelect(impl->getWebview());
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_replace" == method || "native_replaceMisspelling" == method || 
        "native_findInPage" == method || "native_stopFindInPage" == method ||
        "native_inspectServiceWorker" == method || "native_print" == method || 
        "native_printToPDF" == method || "native_showDefinitionForSelection" == method ||
        "native_capturePage" == method ||
        "native_setZoomLevel" == method ||
        "native_setZoomLevelLimits" == method ||
        "native_sendInputEvent" == method) {
        BOOLEAN_TO_NPVARIANT(true, *result);
        return true;
    }
    if ("native_getId" == method) {
        INT32_TO_NPVARIANT(impl->getGuestId(), *result);
        return true;
    }
    if ("native_downloadURL" == method) {
        char* downLoadUrl = (char*)g_npBrowserFunctions->memalloc(12);
        strcpy(downLoadUrl, "about:blank");
        STRINGN_TO_NPVARIANT(downLoadUrl, 11, *result);
        return true;
    }
    if ("native_setZoomFactor" == method) {
        if (argCount >= 1 && NPVariantType_Double == args[0].type) {
            wkeSetZoomFactor(impl->getWebview(), (float)args[0].value.doubleValue);
        }
    }
    if ("native_insertCSS" == method) {
        if (argCount >= 1) {
            if (NPVariantType_String == args[0].type) {
                std::string cssText = std::string(args[0].value.stringValue.UTF8Characters, args[0].value.stringValue.UTF8Length);
                wkeInsertCSSByFrame(impl->getWebview(), wkeWebFrameGetMainFrame(impl->getWebview()), cssText.c_str());
            }
        }
        BOOLEAN_TO_NPVARIANT(false, *result);
        return true;
    }
    if ("native_executeJavaScript" == method) {
        doExecuteJavaScript(impl, args, argCount, result);
        return true;
    }
    // 
    
    return false;
}

NPObject* pluginAllocate(NPP npp, NPClass* npClass) {
    NPObjectEx* npObject = reinterpret_cast<NPObjectEx*>(malloc(sizeof(NPObjectEx)));

    npObject->impl = (WebviewPluginImpl*)npp->pdata;

    npObject->_class = npClass;
    npObject->referenceCount = 1;

    return npObject;
}

void pluginDeallocate(NPObject* npobj) {
    free(npobj);
}

bool hasProperty(NPObject *obj, NPIdentifier propertyName) {
    return false;
}

bool getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result) {
    return false;
}

//////////////////////////////////////////////////////////////////////////

//NPP Functions Implements
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved) {
    wkeWebView webview = wkeGetWebviewByNData(instance->ndata);
    WebviewPluginImpl* impl = new WebviewPluginImpl(webview);
    instance->pdata = impl;

    NPObject* npObj = g_npBrowserFunctions->createobject(instance, &scriptablePluginClass);
    impl->setNpObj(npObj);
    impl->setInstance(instance);

    g_npBrowserFunctions->setvalue(instance, NPPVpluginWindowBool, (void*)false);

    return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save) {
    // If we created a plugin instance, we'll destroy and clean it up.
    NPObject* pluginInstance = (NPObject*)instance->pdata;
    if (!pluginInstance) {
        g_npBrowserFunctions->releaseobject(pluginInstance);
        pluginInstance = NULL;
    }

    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window) {
    WebviewPluginImpl* impl = (WebviewPluginImpl*)instance->pdata;
    impl->onSetWinow(*window);

    return NPERR_NO_ERROR;
}

int16_t NPP_HandleEvent(NPP instance, void* event) {
    NPEvent* evt = (NPEvent*)event;
    WebviewPluginImpl* impl = (WebviewPluginImpl*)instance->pdata;
    switch (evt->event) {
    case WM_WINDOWPOSCHANGED:
        impl->onSize(*((WINDOWPOS*)evt->lParam));
        break;

    case WM_PAINT:
        impl->onPaint((HDC)evt->wParam);
        break;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEWHEEL:
        impl->onMouseEvt(evt->event, evt->wParam, evt->lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
        impl->onKey(evt->event, evt->wParam, evt->lParam);
        break;

    }
    return 0;
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype) {
    *stype = NP_ASFILEONLY;
    return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason) {
    return NPERR_NO_ERROR;
}

int32 NPP_WriteReady(NPP instance, NPStream* stream) {
    return 0;
}

int32 NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer) {
    return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname) {
}

void NPP_Print(NPP instance, NPPrint* platformPrint) {
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData) {
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void* value) {
    NPObject* pluginInstance = NULL;
    switch (variable) {
    case NPPVpluginScriptableNPObject: {
        // If we didn't create any plugin instance, we create it.
        WebviewPluginImpl* impl = (WebviewPluginImpl*)instance->pdata;
        pluginInstance = impl->getNpObj();
        if (pluginInstance)
            g_npBrowserFunctions->retainobject(pluginInstance);

        *(NPObject **)value = pluginInstance;
    }
        break;
    default:
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void* value) {
    return NPERR_GENERIC_ERROR;
}

}