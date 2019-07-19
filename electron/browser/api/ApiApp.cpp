
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "browser/api/ApiApp.h"

#include "common/NodeRegisterHelp.h"
#include "common/ThreadCall.h"
#include "common/AtomCommandLine.h"
#include "common/api/EventEmitter.h"
#include "browser/api/WindowList.h"
#include "browser/api/WindowInterface.h"
#include "base/values.h"
#include "base/json/json_writer.h"
#include "gin/object_template_builder.h"
#include "wke.h"
#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"

#include "base/strings/string_util.h"
#include "base/files/file_path.h"
#include <shlobj.h>
#include <Shlwapi.h>

typedef BOOL(__stdcall *FN_ChangeWindowMessageFilterEx)(HWND hwnd, UINT message, DWORD action, void* pChangeFilterStruct);

namespace {

static void onOnUvCreateProcessCallback(
    wkeWebView webView, 
    void* param, 
    const WCHAR* applicationPath,
    const WCHAR* arguments, 
    STARTUPINFOW* startup
    ) {
    OutputDebugStringW(L"onOnUvCreateProcessCallback:");
    OutputDebugStringW(applicationPath);
    OutputDebugStringW(L"\n");

    if (nullptr != wcsstr(applicationPath, L"git.exe"))
        startup->wShowWindow = SW_HIDE;
    if (nullptr != wcsstr(applicationPath, L"Microsoft.VSCode.CPP.Extension.exe"))
        startup->wShowWindow = SW_HIDE;
    if (nullptr != wcsstr(applicationPath, L"watcher\\win32\\CodeHelper.exe"))
        startup->wShowWindow = SW_HIDE;
}

}

namespace atom {

App* App::m_instance = nullptr;
static const wchar_t kMutexName[] = L"LocalAtomProcessSingletonStartup!";
const wchar_t kHiddenWindowPropName[] = L"mb_app_hidden_window";
const int BUFSIZE = 4096;

App* App::getInstance() {
    return m_instance;
}

App::App(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
    gin::Wrappable<App>::InitWith(isolate, wrapper);
    ASSERT(!m_instance);
    m_instance = this;
    m_version = "1.3.3";
    m_singleInstanceHandle = nullptr;

    wkeNodeOnCreateProcess(nullptr, onOnUvCreateProcessCallback, nullptr);
}

App::~App() {
    ::CloseHandle(m_singleInstanceHandle);
    m_singleInstanceHandle = nullptr;

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
    builder.SetMethod("makeSingleInstanceImpl", &App::makeSingleInstanceImplApi);
    builder.SetMethod("releaseSingleInstance", &App::releaseSingleInstanceApi);
    builder.SetMethod("_relaunch", &App::relaunchApi);
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
    
    const v8::StackTrace::StackTraceOptions options = static_cast<v8::StackTrace::StackTraceOptions>(
        v8::StackTrace::kLineNumber
        | v8::StackTrace::kColumnOffset
        | v8::StackTrace::kScriptId
        | v8::StackTrace::kScriptNameOrSourceURL
        | v8::StackTrace::kFunctionName);

    int stackNum = 50;
    v8::HandleScope handleScope(isolate());
    v8::Local<v8::StackTrace> stackTrace(v8::StackTrace::CurrentStackTrace(isolate(), stackNum, options));
    int count = stackTrace->GetFrameCount();

    char* output = (char*)malloc(0x100);
    sprintf(output, "FatalException: %d\n", count);
    OutputDebugStringA(output);
    free(output);

    for (int i = 0; i < count; ++i) {
        v8::Local<v8::StackFrame> stackFrame = stackTrace->GetFrame(i);
        int frameCount = stackTrace->GetFrameCount();
        int line = stackFrame->GetLineNumber();
        v8::Local<v8::String> scriptName = stackFrame->GetScriptNameOrSourceURL();
        v8::Local<v8::String> funcName = stackFrame->GetFunctionName();

        std::string scriptNameWTF;
        std::string funcNameWTF;

        if (!scriptName.IsEmpty()) {
            v8::String::Utf8Value scriptNameUtf8(scriptName);
            scriptNameWTF = *scriptNameUtf8;
        }

        if (!funcName.IsEmpty()) {
            v8::String::Utf8Value funcNameUtf8(funcName);
            funcNameWTF = *funcNameUtf8;
        }
        std::vector<char> output;
        output.resize(1000);
        sprintf(&output[0], "line:%d, [", line);
        OutputDebugStringA(&output[0]);

        if (!scriptNameWTF.empty()) {
            OutputDebugStringA(scriptNameWTF.c_str());
        }
        OutputDebugStringA("] , [");

        if (!funcNameWTF.empty()) {
            OutputDebugStringA(funcNameWTF.c_str());
        }
        OutputDebugStringA("]\n");
    }
    OutputDebugStringA("\n");

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

static unsigned int hashString(const wchar_t* p) {
    int prime = 25013;
    unsigned int h = 0;
    unsigned int g;
    for (; *p; p++) {
        h = (h << 4) + *p;
        g = h & 0xF0000000;
        if (g) {
            h ^= (g >> 24);
            h ^= g;
        }
    }
    return h % prime;
}

static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void registerHiddenWindowClass(LPCWSTR lpszClassName) {
    WNDCLASS wndClass = { 0 };
    if (!GetClassInfoW(NULL, lpszClassName, &wndClass)) {
        wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
        wndClass.lpfnWndProc = &staticWindowProc;
        wndClass.cbClsExtra = 200;
        wndClass.cbWndExtra = 200;
        wndClass.hInstance = GetModuleHandleW(NULL);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = NULL;
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = lpszClassName;
        ATOM r = RegisterClass(&wndClass);
        r = r;
    }
}

static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int id = -1;
    App* self = (App*)::GetPropW(hWnd, kHiddenWindowPropName);
    if (!self && message == WM_CREATE) {
        LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
        self = (App*)cs->lpCreateParams;
        ::SetPropW(hWnd, kHiddenWindowPropName, (HANDLE)self);
        return 0;
    }

    if (!self)
        return ::DefWindowProcW(hWnd, message, wParam, lParam);

    if (message == WM_COPYDATA) {
        COPYDATASTRUCT* copyData = (COPYDATASTRUCT*)lParam;
        if (copyData->dwData == WindowInterface::kSingleInstanceMessage) {
            self->onCopyData(copyData);
        }
    }

    return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

void App::onCopyData(const COPYDATASTRUCT* copyData) {
    if (copyData->dwData != WindowInterface::kSingleInstanceMessage || 0 == copyData->cbData)
        return;
    if (m_singleInstanceCall.IsEmpty())
        return;

    std::string json((const char*)copyData->lpData, copyData->cbData);

    v8::Function* callback = nullptr;
    v8::Local<v8::Value> f = m_singleInstanceCall.Get(isolate());
    callback = v8::Function::Cast(*(f));

    v8::MaybeLocal<v8::String> argString = v8::String::NewFromUtf8(isolate(), json.c_str(), v8::NewStringType::kNormal, json.length());

    v8::Local<v8::Value> argv[1];
    argv[0] = argString.ToLocalChecked();
    callback->Call(v8::Undefined(isolate()), 1, argv);
}

static std::wstring getNormalizeFilePath() {
    std::vector<wchar_t> path;
    path.resize(BUFSIZE + 1);
    ::GetModuleFileNameW(::GetModuleHandleW(NULL), &path[0], BUFSIZE);

    std::vector<wchar_t> buffer;
    buffer.resize(BUFSIZE + 1);
    // ::GetLongPathName(path.data(), &buffer[0], BUFSIZE);

    WCHAR** lppPart = { nullptr };
    ::GetFullPathName(&path[0], BUFSIZE, &buffer[0], lppPart);

    int i = 0;
    for (; i < BUFSIZE; ++i) {
        wchar_t c = buffer[i];
        if (c >= L'A' && c <= L'Z')
            buffer[i] += 32;
        else if (c == L'/')
            buffer[i] = L'\\';
        if (L'\0' == c)
            break;
    }

    return std::wstring(&buffer[0], i);
}

static void notifSingleProcess(HWND hWnd) {
    std::vector<std::string> argv = atom::AtomCommandLine::argv();

    std::vector<wchar_t> buffer;
    buffer.resize(MAX_PATH + 1);
    ::GetModuleFileNameW(::GetModuleHandleW(NULL), &buffer[0], MAX_PATH);
    ::PathRemoveFileSpecW(&buffer[0]);

    std::string workingDirectory = base::WideToUTF8(base::string16(&buffer[0]));

    base::ListValue value;
    std::string json;

    value.AppendStrings(argv);
    value.AppendString(workingDirectory);
    base::JSONWriter::Write(value, &json);

    COPYDATASTRUCT copyData;
    copyData.dwData = WindowInterface::kSingleInstanceMessage;
    copyData.cbData = json.length();
    copyData.lpData = (PVOID)json.c_str();
    ::SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&copyData);
}

bool App::makeSingleInstanceImplApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Function* callback = nullptr;
    if (args[0]->IsFunction())
        m_singleInstanceCall.Reset(args.GetIsolate(), args[0]);
    else
        m_singleInstanceCall.Reset();

    std::wstring filePath = getNormalizeFilePath();
    unsigned int pathHash = hashString(filePath.c_str());

    std::vector<wchar_t> buffer;
    buffer.resize(BUFSIZE);
    swprintf(&buffer[0], L"MbAppMakeSingleInstanceHiddenWindow_%d\n", pathHash);

    m_singleInstanceHandle = ::CreateMutex(NULL, FALSE, kMutexName);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        HWND hWnd = ::FindWindowEx(NULL, NULL, &buffer[0], nullptr);
        if (hWnd)
            notifSingleProcess(hWnd);
        ::CloseHandle(m_singleInstanceHandle);
        m_singleInstanceHandle = nullptr;
        return true;
    }

    registerHiddenWindowClass(&buffer[0]);
    m_hiddenWindow = ::CreateWindowExW(0,
        &buffer[0], L"title",
        WS_OVERLAPPED, 0, 0, 1, 1, NULL, NULL, ::GetModuleHandleW(NULL), this);
    ::ShowWindow(m_hiddenWindow, SW_HIDE);

    // NB: Ensure that if the primary app gets started as elevated
    // admin inadvertently, secondary windows running not as elevated
    // will still be able to send messages
    HMODULE hinstLib = LoadLibraryW(L"User32.dll");
    static FN_ChangeWindowMessageFilterEx ChangeWindowMessageFilterFunc = nullptr;
    static bool isFind = false;
    if (!isFind) {
        isFind = true;
        ChangeWindowMessageFilterFunc = (FN_ChangeWindowMessageFilterEx)GetProcAddress(hinstLib, "ChangeWindowMessageFilterEx");
        if (ChangeWindowMessageFilterFunc)
            ChangeWindowMessageFilterFunc(m_hiddenWindow, WM_COPYDATA, /*MSGFLT_ALLOW*/1, NULL);
    }   

    return false;
}

void App::releaseSingleInstanceApi() {
    ::CloseHandle(m_singleInstanceHandle);
    m_singleInstanceHandle = nullptr;
}


static std::wstring addQuoteForArg(const std::wstring& arg)
{
    // We follow the quoting rules of CommandLineToArgvW.
    // http://msdn.microsoft.com/en-us/library/17w5ykft.aspx
    std::wstring quotable_chars(L" \\\"");
    if (arg.find_first_of(quotable_chars) == std::wstring::npos) {
        // No quoting necessary.
        return arg;
    }

    std::wstring out;
    out += (L'"');
    for (size_t i = 0; i < arg.size(); ++i) {
        if (arg[i] == '\\') {
            // Find the extent of this run of backslashes.
            size_t start = i, end = start + 1;
            for (; end < arg.size() && arg[end] == '\\'; ++end) {}
            size_t backslash_count = end - start;

            // Backslashes are escapes only if the run is followed by a double quote.
            // Since we also will end the string with a double quote, we escape for
            // either a double quote or the end of the string.
            if (end == arg.size() || arg[end] == '"') {
                // To quote, we need to output 2x as many backslashes.
                backslash_count *= 2;
            }
            for (size_t j = 0; j < backslash_count; ++j)
                out += L'\\';

            // Advance i to one before the end to balance i++ in loop.
            i = end - 1;
        } else if (arg[i] == '"') {
            out += L'\\';
            out += L'"';
        } else {
            out += (arg[i]);
        }
    }
    out += L'"';

    return out;
}

static std::wstring argvToCommandLineString(const std::vector<std::wstring>& argv)
{
    std::wstring commandLine;
    for (const std::wstring& arg : argv) {
        if (!commandLine.empty())
            commandLine += L' ';
        commandLine += addQuoteForArg(arg);
    }
    return commandLine;
}

const char* kWaitEventName = "ElectronRelauncherWaitEvent";
const WCHAR* kRelauncherTypeArg = L"--type=relauncher";
const WCHAR* kRelauncherArgSeparator = L"---";

static PROCESS_INFORMATION* launchProcess(const base::string16& cmdline) {
    STARTUPINFO startup_info = {0};
    DWORD flags = 0;

    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_SHOW;

    PROCESS_INFORMATION* tempProcessInfo = new PROCESS_INFORMATION();
    memset(tempProcessInfo, 0, sizeof(PROCESS_INFORMATION));
    base::string16 writableCmdline = cmdline;

    if (!::CreateProcessW(NULL, &writableCmdline[0], NULL, NULL, FALSE, flags, NULL, NULL, &startup_info, tempProcessInfo)) {
        tempProcessInfo->hProcess = INVALID_HANDLE_VALUE;
        return tempProcessInfo;
    }
    
    return tempProcessInfo;
}

static std::wstring getWaitEventName(DWORD pid) {
    std::vector<char> buffer;
    buffer.resize(0x1000);
    memset(&buffer[0], 0, 0x1000);
    sprintf(&buffer[0], "%s-%d", kWaitEventName, static_cast<int>(pid));
    return base::UTF8ToWide(&buffer[0]);
}

static bool relaunchAppWithHelper(const base::FilePath& helper, const std::vector<std::wstring>& relauncher_args, const std::vector<std::wstring>& argv) {
    std::vector<std::wstring> relaunchArgv;
    relaunchArgv.push_back(helper.value());
#if 0 // https://github.com/electron/electron/pull/5837/files
    relaunchArgv.push_back(kRelauncherTypeArg);
    relaunchArgv.insert(relaunchArgv.end(), relauncher_args.begin(), relauncher_args.end());
    relaunchArgv.push_back(kRelauncherArgSeparator);
    relaunchArgv.insert(relaunchArgv.end(), argv.begin(), argv.end());
#endif
    PROCESS_INFORMATION* process = launchProcess(argvToCommandLineString(relaunchArgv));
    if (!process || INVALID_HANDLE_VALUE == process->hProcess) {
        if (process)
            delete process;
        return false;
    }
    
    // The relauncher process is now starting up, or has started up. The
    // original parent process continues.
    // Synchronize with the relauncher process.
    std::wstring name = getWaitEventName(process->dwProcessId);
    HANDLE waitEvent = ::CreateEventW(NULL, TRUE, FALSE, name.c_str());
    if (waitEvent != NULL) {
        ::WaitForSingleObject(waitEvent, 1000);
        ::CloseHandle(waitEvent);
    }

    return true;
}

static bool relaunchApp(const std::vector<std::wstring>& argv) {
    // Use the currently-running application's helper process. The automatic
    // update feature is careful to leave the currently-running version alone,
    // so this is safe even if the relaunch is the result of an update having
    // been applied. In fact, it's safer than using the updated version of the
    // helper process, because there's no guarantee that the updated version's
    // relauncher implementation will be compatible with the running version's.
    base::FilePath childPath;
    std::vector<WCHAR> currentExePath;
    currentExePath.resize(MAX_PATH);
    ::GetModuleFileName(NULL, &currentExePath[0], MAX_PATH);
    childPath = base::FilePath::FromUTF16Unsafe(base::StringPiece16(&currentExePath[0]));

    std::vector<std::wstring> relauncherArgs;
    return relaunchAppWithHelper(childPath, relauncherArgs, argv);
}

void App::relaunchApi(const base::DictionaryValue& options) {
    std::string argsStr;
    std::string execPathStr;
    bool isOverrideArgv = false;
    const base::Value* args = nullptr;
    const base::ListValue* argsList = nullptr;
    std::vector<std::wstring> argsArray;

    if (options.Get("args", &args)) {
        args->GetAsList(&argsList);
        for (size_t i = 0; i < argsList->GetSize(); ++i) {
            std::string arg;
            if (!argsList->GetString(i, &arg))
                continue;
            if (arg.size() > 0)
                argsArray.push_back(base::UTF8ToWide(arg));
        }
        if (argsArray.size() > 0)
            isOverrideArgv = true;
    }
    
    const base::Value* execPath = nullptr;
    if (options.Get("execPath", &execPath)) {
        execPath->GetAsString(&execPathStr);
        if (execPathStr.size() > 0)
            isOverrideArgv = true;
    }

    if (!isOverrideArgv) {
        const std::vector<std::wstring>& argv = atom::AtomCommandLine::wargv();
        relaunchApp(argv);
    }

    std::vector<std::wstring> argv;
    argv.reserve(1 + argsArray.size());

    if (execPathStr.empty()) {
        std::vector<WCHAR> currentExePath;
        currentExePath.resize(MAX_PATH);
        if (GetModuleFileName(NULL, &currentExePath[0], MAX_PATH))
            argv.push_back(&currentExePath[0]);
    } else {
        argv.push_back(base::UTF8ToWide(execPathStr));
    }

    argv.insert(argv.end(), argsArray.begin(), argsArray.end());
    relaunchApp(argv);
}

void App::setPathApi(const std::string& name, const std::string& path) { 
    if (!(name == "userData" || name == "cache" || name == "userCache" || name == "documents"
        || name == "downloads" || name == "music" || name == "videos" || name == "pepperFlashSystemPlugin"))
        return;
    
    std::map<std::string, std::string>::iterator it = m_pathMap.find(name);
    if (it == m_pathMap.end())
        m_pathMap.insert(std::make_pair(name, path));
    else
        it->second = path;
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