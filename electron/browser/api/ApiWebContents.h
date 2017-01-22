#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "electron.h"
#include "gin/dictionary.h"

using namespace v8;
using namespace node;

namespace atom {

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class WebContents : public node::ObjectWrap {
public:
    static void init(Local<Object> target, Environment* env);
    static WebContents* create(Isolate* isolate, gin::Dictionary options);

    explicit WebContents();
    ~WebContents();

private:
    // new方法
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    // _loadURL
    static void _loadURL(const v8::FunctionCallbackInfo<v8::Value>& args);

    //
    static void _getURL(const v8::FunctionCallbackInfo<v8::Value>& args);

    //
    static void getTitle(const v8::FunctionCallbackInfo<v8::Value>& args);

    //
    static void isLoading(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isLoadingMainFrame(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isWaitingForResponse(const v8::FunctionCallbackInfo<v8::Value>& args);

    //
    static void _stop(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void _goBack(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void _goForward(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void _goToOffset(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isCrashed(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void setUserAgent(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void getUserAgent(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void insertCSS(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void savePage(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void openDevTools(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void closeDevTools(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isDevToolsOpened(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isDevToolsFocused(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void enableDeviceEmulation(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void disableDeviceEmulation(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void toggleDevTools(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void inspectElement(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void setAudioMuted(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isAudioMuted(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void undo(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void redo(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void cut(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void copy(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void paste(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void pasteAndMatchStyle(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void _delete(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void selectAll(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void unselect(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void replace(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void replaceMisspelling(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void findInPage(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void stopFindInPage(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void focus(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isFocused(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void tabTraverse(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void _send(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void sendInputEvent(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void beginFrameSubscription(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void endFrameSubscription(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void startDrag(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void setSize(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isGuest(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isOffscreen(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void startPainting(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void stopPainting(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void isPainting(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void setFrameRate(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void getFrameRate(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void invalidate(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void getType(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void getWebPreferences(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void getOwnerBrowserWindow(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void hasServiceWorker(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void unregisterServiceWorker(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void inspectServiceWorker(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void print(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void _printToPDF(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void addWorkSpace(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void reNullWorkSpace(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void showDefinitionForSelection(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void copyImageAt(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void capturePage(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void setEmbedder(const v8::FunctionCallbackInfo<v8::Value>& args);
    
    static void nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
public:
    wkeWebView m_view;
};

} // atom