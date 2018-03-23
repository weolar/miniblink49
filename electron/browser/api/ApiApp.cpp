
#include "browser/api/ApiApp.h"

#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "common/ThreadCall.h"
#include "gin/object_template_builder.h"
#include "browser/api/WindowList.h"
#include "base/values.h"
#include "wke.h"
#include "nodeblink.h"
#include "base/strings/string_util.h"
#include "base/files/file_path.h"
#include <shlobj.h>

namespace atom {

App* App::m_instance = nullptr;

App* App::getInstance() {
    return m_instance;
}

App::App(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
    gin::Wrappable<App>::InitWith(isolate, wrapper);
    ASSERT(!m_instance);
    m_instance = this;
}

App::~App() {
    DebugBreak();
}

void App::init(v8::Local<v8::Object> target, v8::Isolate* isolate) {
    v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

    prototype->SetClassName(v8::String::NewFromUtf8(isolate, "App"));
    gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
    builder.SetMethod("quit", &App::quitApi);
    builder.SetMethod("exit", &App::exitApi);
    builder.SetMethod("focus", &App::focusApi);
    builder.SetMethod("getVersion", &App::getVersionApi);
    builder.SetMethod("setVersion", &App::setVersionApi);
    builder.SetMethod("getName", &App::getNameApi);
    builder.SetMethod("setName", &App::setNameApi);
    builder.SetMethod("isReady", &App::isReadyApi);
    builder.SetMethod("addRecentDocument", &App::addRecentDocumentApi);
    builder.SetMethod("clearRecentDocuments", &App::clearRecentDocumentsApi);
    builder.SetMethod("setAppUserModelId", &App::setAppUserModelIdApi);
    builder.SetMethod("isDefaultProtocolClient", &App::isDefaultProtocolClientApi);
    builder.SetMethod("setAsDefaultProtocolClient", &App::setAsDefaultProtocolClientApi);
    builder.SetMethod("removeAsDefaultProtocolClient", &App::removeAsDefaultProtocolClientApi);
    builder.SetMethod("setBadgeCount", &App::setBadgeCountApi);
    builder.SetMethod("getBadgeCount", &App::getBadgeCountApi);
    builder.SetMethod("getLoginItemSettings", &App::getLoginItemSettingsApi);
    builder.SetMethod("setLoginItemSettings", &App::setLoginItemSettingsApi);
    builder.SetMethod("setUserTasks", &App::setUserTasksApi);
    builder.SetMethod("getJumpListSettings", &App::getJumpListSettingsApi);
    builder.SetMethod("setJumpList", &App::setJumpListApi);
    builder.SetMethod("setPath", &App::setPathApi);
    builder.SetMethod("getPath", &App::getPathApi);
    builder.SetMethod("setDesktopName", &App::setDesktopNameApi);
    builder.SetMethod("getLocale", &App::getLocaleApi);
    builder.SetMethod("makeSingleInstance", &App::makeSingleInstanceApi);
    builder.SetMethod("releaseSingleInstance", &App::releaseSingleInstanceApi);
    builder.SetMethod("relaunch", &App::relaunchApi);
    builder.SetMethod("isAccessibilitySupportEnabled", &App::isAccessibilitySupportEnabled);
    builder.SetMethod("disableHardwareAcceleration", &App::disableHardwareAcceleration);

    constructor.Reset(isolate, prototype->GetFunction());
    target->Set(v8::String::NewFromUtf8(isolate, "App"), prototype->GetFunction());
}

void App::nullFunction() {
    OutputDebugStringA("nullFunction\n");
}

void quit() {
    ::TerminateProcess(::GetCurrentProcess(), 0);
    WindowList::closeAllWindows();

    ThreadCall::exitMessageLoop(ThreadCall::getBlinkThreadId());
    ThreadCall::exitMessageLoop(ThreadCall::getUiThreadId());
}

void App::quitApi() {
    OutputDebugStringA("quitApi\n");
    quit();

    if (ThreadCall::isUiThread()) {
        quit();
        return;
    }

    ThreadCall::callUiThreadAsync([] {
        quit();
    });
}

void App::exitApi() {
    quitApi();
}

void App::focusApi() {
    OutputDebugStringA("focusApi\n");
}

bool App::isReadyApi() const {
    OutputDebugStringA("isReadyApi\n");
    return true;
}

void App::addRecentDocumentApi(const std::string& path) {
    OutputDebugStringA("addRecentDocumentApi\n");
}

void App::clearRecentDocumentsApi() {
    OutputDebugStringA("clearRecentDocumentsApi\n");
}

void App::setAppUserModelIdApi(const std::string& id) {
    OutputDebugStringA("setAppUserModelIdApi\n");
}

// const std::string& protocol, const std::string& path, const std::string& args
bool App::isDefaultProtocolClientApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("isDefaultProtocolClientApi\n");
    return true;
}

//const std::string& protocol, const std::string& path, const std::string& args
bool App::setAsDefaultProtocolClientApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("setAsDefaultProtocolClientApi\n");
    if (0 == args.Length())
        return false;

    std::string protocol;
    if (!gin::ConvertFromV8(args.GetIsolate(), args[0], &protocol))
        return false;
    
    return true;
}

// const std::string& protocol, const std::string& path, const std::string& args
bool App::removeAsDefaultProtocolClientApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("removeAsDefaultProtocolClientApi\n");
    return true;
}

bool App::setBadgeCountApi(int count) {
    OutputDebugStringA("setBadgeCountApi\n");
    return false;
}

int App::getBadgeCountApi() {
    OutputDebugStringA("getBadgeCountApi\n");
    return 0;
}

// const base::DictionaryValue& obj
int App::getLoginItemSettingsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("getLoginItemSettingsApi\n");
    DebugBreak();
    return 0;
}

// const base::DictionaryValue& obj, const std::string& path, const std::string& args
void App::setLoginItemSettingsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("setLoginItemSettingsApi");
    DebugBreak();
}

bool App::setUserTasksApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("setUserTasksApi\n");
    return true;
}

void App::setDesktopNameApi(const std::string& desktopName) { 
    OutputDebugStringA("App::setDesktopNameApi\n");
}

v8::Local<v8::Value> App::getJumpListSettingsApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("getJumpListSettingsApi\n");
    base::DictionaryValue obj;

    obj.SetInteger("minItems", 1);

    base::ListValue* removedItems = new base::ListValue();
    obj.Set("removedItems", removedItems);

    v8::Local<v8::Value> result = gin::Converter<base::DictionaryValue>::ToV8(args.GetIsolate(), obj);
    return result;
}

// const base::DictionaryValue&
void App::setJumpListApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("setJumpListApi\n");
}

std::string App::getLocaleApi() {
    return "zh-cn";
}

void App::makeSingleInstanceApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    OutputDebugStringA("makeSingleInstanceApi\n");
}

void App::releaseSingleInstanceApi() {
    OutputDebugStringA("releaseSingleInstanceApi\n");
}

void App::relaunchApi(const base::DictionaryValue& options) {
    OutputDebugStringA("relaunchApi\n");
}

void App::setPathApi(const std::string& name, const std::string& path) { 
    if (name == "userData" || name == "cache" || name == "userCache" || name == "documents"
        || name == "downloads" || name == "music" || name == "videos" || name == "pepperFlashSystemPlugin") {
        m_pathMap.insert(std::make_pair(name, path));
    }
}

bool getTempDir(base::FilePath* path) {
    wchar_t temp_path[MAX_PATH + 1];
    DWORD path_len = ::GetTempPath(MAX_PATH, temp_path);
    if (path_len >= MAX_PATH || path_len <= 0)
        return false;
    // TODO(evanm): the old behavior of this function was to always strip the
    // trailing slash.  We duplicate this here, but it shouldn't be necessary
    // when everyone is using the appropriate FilePath APIs.
    *path = base::FilePath(temp_path).StripTrailingSeparators();
    return true;
}

base::FilePath getHomeDir() {
    wchar_t result[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, result)) && result[0])
        return base::FilePath(result);

    // Fall back to the temporary directory on failure.
    base::FilePath temp;
    if (getTempDir(&temp))
        return temp;

    // Last resort.
    return base::FilePath(L"C:\\");
}

std::string App::getPathApi(const std::string& name) const {
    base::FilePath path;
    std::wstring systemBuffer;
    systemBuffer.assign(MAX_PATH, L'\0');
    std::wstring pathBuffer;
    if (name == "appData") {
        if ((::SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, &systemBuffer[0])) < 0)
            return "";
    } else if (name == "userData" || name == "documents"
        || name == "downloads" || name == "music" || name == "videos" || name == "pepperFlashSystemPlugin") {
        std::map<std::string, std::string>::const_iterator it = m_pathMap.find(name);
        if (it == m_pathMap.end())
            return "";
        return it->second;
    } else if (name == "home")
        systemBuffer = getHomeDir().value();
    else if (name == "temp") {
        if (!getTempDir(&path))
            return "";
            systemBuffer = path.value();
    } else if (name == "userDesktop" || name == "desktop") {
        if (FAILED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, &systemBuffer[0])))
            return "";
    } else if (name == "exe") {
        ::GetModuleFileName(NULL, &systemBuffer[0], MAX_PATH);
    } else if (name == "module")
        ::GetModuleFileName(NULL, &systemBuffer[0], MAX_PATH);
    else if (name == "cache" || name == "userCache") {
        if ((::SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, &systemBuffer[0])) < 0)
            return "";
       
        pathBuffer = systemBuffer.c_str();
        pathBuffer += L"\\electron";
    } else
        return "";

    pathBuffer = systemBuffer.c_str();
    return base::WideToUTF8(pathBuffer);
}

void App::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    if (args.IsConstructCall()) {
        new App(isolate, args.This());
        args.GetReturnValue().Set(args.This());
        return;
    }
}

void App::onWindowAllClosed() {
    if (ThreadCall::isUiThread()) {
        emit("window-all-closed");
        return;
    }

    App* self = this;
    ThreadCall::callUiThreadAsync([self] {
        self->emit("window-all-closed");
    });
}

v8::Persistent< v8::Function> App::constructor;
gin::WrapperInfo App::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeAppApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    App::init(target, env->isolate());
}

static const char BrowserAppNative[] = "console.log('BrowserAppNative');;";
static NodeNative nativeBrowserAppNative{ "App", BrowserAppNative, sizeof(BrowserAppNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_app, initializeAppApi, &nativeBrowserAppNative)

} 