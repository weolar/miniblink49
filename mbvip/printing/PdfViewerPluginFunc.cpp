
#include "PdfViewerPluginFunc.h"

#include "wke/wkedefine.h"
#include "PdfViewerPlugin.h"

namespace printing {

// Browser function table，可以通过它来得到浏览器提供的功能
NPNetscapeFuncs* g_npBrowserFunctions = nullptr;

/*******各种接口的声明*********/
//在NPAPI编程的接口中你会发现有NP_打头的，有NPP_打头的，有NPN_打头的
//NP是npapi的插件库提供给浏览器的最上层的接口
//NPP即NP Plugin，是插件本身提供给浏览器调用的接口，主要被用来填充NPPluginFuncs的结构体
//NPN即NP Netscape，是浏览器提供给插件使用的接口，这些接口一般都在NPNetscapeFuncs结构体中

//NPP Functions
NPError PdfViewer_NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved);
NPError PdfViewer_NPP_Destroy(NPP instance, NPSavedData** save);
NPError PdfViewer_NPP_SetWindow(NPP instance, NPWindow* window);
NPError PdfViewer_NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
NPError PdfViewer_NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
int32_t PdfViewer_NPP_WriteReady(NPP instance, NPStream* stream);
int32_t PdfViewer_NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
void PdfViewer_NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void PdfViewer_NPP_Print(NPP instance, NPPrint* platformPrint);
int16_t PdfViewer_NPP_HandleEvent(NPP instance, void* event);
void PdfViewer_NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData);
NPError PdfViewer_NPP_GetValue(NPP instance, NPPVariable variable, void *value);
NPError PdfViewer_NPP_SetValue(NPP instance, NPNVariable variable, void *value);

//Functions for scriptablePluginClass
NPObject* PdfViewer_pluginAllocate(NPP npp, NPClass *aClass);
void PdfViewer_pluginDeallocate(NPObject *npobj);
bool PdfViewer_pluginHasMethod(NPObject *obj, NPIdentifier methodName);
bool PdfViewer_pluginInvoke(NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool PdfViewer_hasProperty(NPObject *obj, NPIdentifier propertyName);
bool PdfViewer_getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result);

struct NPObjectEx : public NPObject {
    PdfViewerPlugin* impl;
};

static struct NPClass scriptablePluginClass = {
    NP_CLASS_STRUCT_VERSION,
    PdfViewer_pluginAllocate,
    PdfViewer_pluginDeallocate,
    NULL,
    PdfViewer_pluginHasMethod,
    PdfViewer_pluginInvoke,
    NULL,
    PdfViewer_hasProperty,
    PdfViewer_getProperty,
    NULL,
    NULL,
};

//接口的实现
NPError __stdcall PdfViewerPluginNPInitialize(NPNetscapeFuncs* browserFuncs)
{
    g_npBrowserFunctions = browserFuncs;
    return NPERR_NO_ERROR;
}

NPError __stdcall PdfViewerPluginNPGetEntryPoints(NPPluginFuncs* pluginFuncs)
{
    pluginFuncs->version = 11;
    pluginFuncs->size = sizeof(pluginFuncs);
    pluginFuncs->newp = PdfViewer_NPP_New;
    pluginFuncs->destroy = PdfViewer_NPP_Destroy;
    pluginFuncs->setwindow = PdfViewer_NPP_SetWindow;
    pluginFuncs->newstream = PdfViewer_NPP_NewStream;
    pluginFuncs->destroystream = PdfViewer_NPP_DestroyStream;
    pluginFuncs->asfile = PdfViewer_NPP_StreamAsFile;
    pluginFuncs->writeready = PdfViewer_NPP_WriteReady;
    pluginFuncs->write = (NPP_WriteProcPtr)PdfViewer_NPP_Write;
    pluginFuncs->print = PdfViewer_NPP_Print;
    pluginFuncs->event = PdfViewer_NPP_HandleEvent;
    pluginFuncs->urlnotify = PdfViewer_NPP_URLNotify;
    pluginFuncs->getvalue = PdfViewer_NPP_GetValue;
    pluginFuncs->setvalue = PdfViewer_NPP_SetValue;

    return NPERR_NO_ERROR;
}

void __stdcall PdfViewerPluginNPShutdown(void)
{
}

// static void npVariantTypeObjectToDirValue(PdfViewerPlugin* impl, NPObject* objectValue, base::DictionaryValue* dirParams)
// {
//     NPIdentifier* identifier = nullptr;
//     uint32_t count = 0;
//     NPP instance = impl->getInstance();
//     g_npBrowserFunctions->enumerate(instance, objectValue, &identifier, &count);
// 
//     for (uint32_t i = 0; i < count; ++i) {
//         char* method = g_npBrowserFunctions->utf8fromidentifier(identifier[i]);
//         NPVariant result;
//         g_npBrowserFunctions->getproperty(instance, objectValue, identifier[i], &result);
// 
//         switch (result.type) {
//         case NPVariantType_Bool:
//             dirParams->SetBoolean(method, result.value.boolValue);
//             break;
//         case NPVariantType_Int32:
//             dirParams->SetInteger(method, result.value.intValue);
//             break;
//         case NPVariantType_Double:
//             dirParams->SetDouble(method, result.value.doubleValue);
//             break;
//         case NPVariantType_String:
//             dirParams->SetString(method, std::string(result.value.stringValue.UTF8Characters, result.value.stringValue.UTF8Length));
//             break;
// 
//         case NPVariantType_Object:
//             base::DictionaryValue* dirParamsObj = new base::DictionaryValue();
//             npVariantTypeObjectToDirValue(impl, result.value.objectValue, dirParamsObj);
//             dirParams->Set(method, dirParamsObj);
//             break;
//         }
// 
//         g_npBrowserFunctions->releasevariantvalue(&result);
//         g_npBrowserFunctions->memfree(method);
//     }
//     g_npBrowserFunctions->memfree(identifier);
// }

static void toResultString(const utf8* str, NPVariant* result)
{
    size_t length = strlen(str);
    utf8* strBuffer = (utf8*)g_npBrowserFunctions->memalloc(length);
    memcpy(strBuffer, str, length);
    STRINGN_TO_NPVARIANT(strBuffer, length, *result);
}

bool PdfViewer_pluginHasMethod(NPObject* obj, NPIdentifier methodName)
{
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

static bool doExecuteJavaScript(PdfViewerPlugin* impl, const NPVariant* args, uint32_t argCount, NPVariant* result)
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

bool PdfViewer_pluginInvoke(NPObject* obj, NPIdentifier methodName, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    return false;
}

NPObject* PdfViewer_pluginAllocate(NPP npp, NPClass* npClass)
{
    NPObjectEx* npObject = reinterpret_cast<NPObjectEx*>(malloc(sizeof(NPObjectEx)));

    npObject->impl = (PdfViewerPlugin*)npp->pdata;

    npObject->_class = npClass;
    npObject->referenceCount = 1;

    return npObject;
}

void PdfViewer_pluginDeallocate(NPObject* npobj)
{
    free(npobj);
}

bool PdfViewer_hasProperty(NPObject *obj, NPIdentifier propertyName)
{
    return false;
}

bool PdfViewer_getProperty(NPObject *obj, NPIdentifier propertyName, NPVariant *result)
{
    return false;
}

//////////////////////////////////////////////////////////////////////////

//NPP Functions Implements
NPError PdfViewer_NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{
    bool b = (0 == strcmp(pluginType, "application/pdf"));
    if (b && !instance)
        return NPERR_NO_ERROR;
    else if (!b)
        return NPERR_INVALID_PLUGIN_ERROR;

    const char* url = nullptr;
    for (int i = 0; i < argc; ++i) {
        char* name = argn[i];
        if (0 == strcmp(name, "src"))
            url = argv[i];
    }

    wkeWebView webview = wkeGetWebViewByNData(instance->ndata);
    PdfViewerPlugin* impl = new PdfViewerPlugin(webview, url);
    instance->pdata = impl;

//     NPObject* npObj = g_npBrowserFunctions->createobject(instance, &scriptablePluginClass);
//     impl->setNpObj(npObj);
    impl->setInstance(instance);

    g_npBrowserFunctions->setvalue(instance, NPPVpluginWindowBool, (void*)false);

    return NPERR_NO_ERROR;
}

NPError PdfViewer_NPP_Destroy(NPP instance, NPSavedData** save)
{
    PdfViewerPlugin* impl = (PdfViewerPlugin*)instance->pdata;
    delete impl;
    instance->pdata = nullptr;

    return NPERR_NO_ERROR;
}

NPError PdfViewer_NPP_SetWindow(NPP instance, NPWindow* window)
{
    PdfViewerPlugin* impl = (PdfViewerPlugin*)instance->pdata;
    impl->onSetWinow(*window);

    return NPERR_NO_ERROR;
}

int16_t PdfViewer_NPP_HandleEvent(NPP instance, void* event)
{
    NPEvent* evt = (NPEvent*)event;
    PdfViewerPlugin* impl = (PdfViewerPlugin*)instance->pdata;
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

NPError PdfViewer_NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    *stype = NP_NORMAL;
    PdfViewerPlugin* impl = (PdfViewerPlugin*)instance->pdata;

    std::vector<char>* pdfData = new std::vector<char>();
    stream->pdata = pdfData;

    return NPERR_NO_ERROR;
}

NPError PdfViewer_NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    PdfViewerPlugin* impl = (PdfViewerPlugin*)instance->pdata;

    std::vector<char>* pdfData = (std::vector<char>*)stream->pdata;
    impl->resetPdf(pdfData);

    return NPERR_NO_ERROR;
}

int32_t PdfViewer_NPP_WriteReady(NPP instance, NPStream* stream)
{
    return 0xfffffff;
}

int32_t PdfViewer_NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    std::vector<char>* pdfData = (std::vector<char>*)stream->pdata;

    size_t oldSize = pdfData->size();
    pdfData->resize(oldSize + len);
    memcpy(&pdfData->at(0) + oldSize, buffer, len);

    return len;
}

void PdfViewer_NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
}

void PdfViewer_NPP_Print(NPP instance, NPPrint* platformPrint)
{
}

void PdfViewer_NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
}

NPError PdfViewer_NPP_GetValue(NPP instance, NPPVariable variable, void* value)
{
    NPObject* pluginInstance = NULL;
    switch (variable) {
    case NPPVpluginScriptableNPObject:
    {
        // If we didn't create any plugin instance, we create it.
        PdfViewerPlugin* impl = (PdfViewerPlugin*)instance->pdata;
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

NPError PdfViewer_NPP_SetValue(NPP instance, NPNVariable variable, void* value)
{
    return NPERR_GENERIC_ERROR;
}

}