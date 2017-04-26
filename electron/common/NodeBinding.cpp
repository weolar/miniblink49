#include "common/NodeBinding.h"

#include "uv.h"
#include "nodeblink.h"
#include "gin/dictionary.h"
#include "base/file_path.h"
#include "common/StringUtil.h"
#include "common/AtomVersion.h"
#include "common/ChromeVersion.h"
#include "common/AtomCommandLine.h"
#include "common/api/EventEmitterCaller.h"
#include <xstring>
#include <vector>
#include <memory>
#include <shlwapi.h>

namespace atom {

namespace {

void crash(const v8::FunctionCallbackInfo<v8::Value>& info) {
    DebugBreak();
}

void hang(const v8::FunctionCallbackInfo<v8::Value>& info) {
    for (;;) {
        ::Sleep(1000);
    };
}

void getProcessMemoryInfo(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    //std::unique_ptr<base::ProcessMetrics> metrics(base::ProcessMetrics::CreateCurrentProcessMetrics());

    gin::Dictionary dict = gin::Dictionary::CreateEmpty(isolate);
    dict.Set("workingSetSize", /*static_cast<double>(metrics->GetWorkingSetSize() >> 10)*/1000);
    dict.Set("peakWorkingSetSize", /*static_cast<double>(metrics->GetPeakWorkingSetSize() >> 10)*/1000);

    //size_t private_bytes, shared_bytes;
    //if (metrics->GetMemoryBytes(&private_bytes, &shared_bytes)) {
        dict.Set("privateBytes", /*static_cast<double>(private_bytes >> 10)*/1000);
        dict.Set("sharedBytes", /*static_cast<double>(shared_bytes >> 10)*/1000);
    //}

    info.GetReturnValue().Set(gin::Converter<gin::Dictionary>::ToV8(isolate, dict));
}

void getSystemMemoryInfo(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
//     base::SystemMemoryInfoKB mem_info;
//     if (!base::GetSystemMemoryInfo(&mem_info)) {
//         args->ThrowError("Unable to retrieve system memory information");
//         return v8::Undefined(isolate);
//     }

    gin::Dictionary dict = gin::Dictionary::CreateEmpty(isolate);
    dict.Set("total", /*mem_info.total*/10);
    dict.Set("free", /*mem_info.free*/10);

    // NB: These return bogus values on macOS
#if !defined(OS_MACOSX)
    dict.Set("swapTotal", /*mem_info.swap_total*/10);
    dict.Set("swapFree", /*mem_info.swap_free*/10);
#endif

    info.GetReturnValue().Set(gin::Converter<gin::Dictionary>::ToV8(isolate, dict));
}

// Called when there is a fatal error in V8, we just crash the process here so
// we can get the stack trace.
void fatalErrorCallback(const char* location, const char* message) {
    //crash(info);
    DebugBreak();
}

void log(const v8::FunctionCallbackInfo<v8::Value>& info) {
    //std::cout << message << std::flush;
    crash(info);
}

} // namespace

NodeBindings::NodeBindings(bool isBrowser, uv_loop_t* uvLoop)
    : m_isBrowser(isBrowser)
    , m_uvLoop(uvLoop)
    , m_env(nullptr)
    , m_callNextTickAsync(new uv_async_t()) {

}

NodeBindings::~NodeBindings() {
    if (!m_env)
        return;
    nodeDeleteNodeEnvironment(m_env);
    delete m_callNextTickAsync;
}

static std::wstring* kResPath = nullptr;

std::wstring getResourcesPath(const std::wstring& name) {
    std::wstring out;
    if (kResPath) {
        out += *kResPath + name;
        return out;
    }

    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    ::GetModuleFileName(nullptr, &path[0], MAX_PATH);

    ::PathRemoveFileSpecW(&path[0]);
    out += &path[0];

    std::wstring temp(out);
    temp += L"\\node.exp";
    if (!::PathFileExists(temp.c_str()))
        out += L"\\electron.asar\\";
    else
        out += L"\\..\\..\\electron\\lib\\";
    
    kResPath = new std::wstring(out);
    out += name;
    return out;
}

void loadNodeScriptFromRes(void* path) {
    NodeNative* nativePath = (NodeNative*)path;
    std::wstring sourceW;
    for (size_t i = 0; i < nativePath->sourceLen; ++i)
        sourceW += nativePath->source[i];

    if (L'.' == sourceW[0] && L'/' == sourceW[1])
        sourceW = sourceW.substr(2, sourceW.size() - 2);
    sourceW += L".js";
    sourceW = getResourcesPath(sourceW);

    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (::GetFileAttributesExW(sourceW.c_str(), GetFileExInfoStandard, &attrs) == 0)
        return;

    HANDLE fileHandle = ::CreateFileW(sourceW.c_str(), FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle == INVALID_HANDLE_VALUE)
        return;

    std::vector<char>* buffer = new std::vector<char>(); // �ڴ�й©
    buffer->resize(attrs.nFileSizeLow);

    DWORD bytesRead;
    int retval = ::ReadFile(fileHandle, &buffer->at(0), attrs.nFileSizeLow, &bytesRead, 0);
    ::CloseHandle(fileHandle);

    if (retval == 0 || bytesRead != attrs.nFileSizeLow)
        return;

    nativePath->source = &buffer->at(0);
    nativePath->sourceLen = bytesRead;
}


// Convert the given vector to an array of C-strings. The strings in the
// returned vector are only guaranteed valid so long as the vector of strings
// is not modified.
std::unique_ptr<const char*[]> stringVectorToArgArray(const std::vector<std::string>& vector) {
    std::unique_ptr<const char*[]> argsArray(new const char*[vector.size()]);
    for (size_t i = 0; i < vector.size(); ++i) {
        argsArray[i] = vector[i].c_str();
    }
    return argsArray;
}

void NodeBindings::initNodeEnv() {
    std::vector<std::string> args = AtomCommandLine::argv();
    int argsSize = args.size();

    std::unique_ptr<const char*[]> c_argv = stringVectorToArgArray(args);
    int exec_argc;
    const char** exec_argv = nullptr;
    node::Init(&argsSize, c_argv.get(), &exec_argc, &exec_argv);
}

void NodeBindings::bindFunction(gin::Dictionary* dict) {
    NodeBindings* self = this;
    dict->SetMethod("crash", &crash);
    dict->SetMethod("hang", &hang);
    dict->SetMethod("log", &log);
    dict->SetMethod("getProcessMemoryInfo", &getProcessMemoryInfo);
    dict->SetMethod("getSystemMemoryInfo", &getSystemMemoryInfo);
#if defined(OS_POSIX)
    dict->SetMethod("setFdLimit", &base::SetFdLimit);
#endif
    dict->SetMethod("activateUvLoop", [self] (const v8::FunctionCallbackInfo<v8::Value>& info) { self->activateUVLoop(info.GetIsolate()); });

#if defined(MAS_BUILD)
    dict->Set("mas", true);
#endif

    gin::Dictionary versions = gin::Dictionary::CreateEmpty(dict->isolate());
    if (dict->Get("versions", &versions)) {
        versions.Set("atom-project-name", std::string(ATOM_VERSION_STRING));
        versions.Set("atom-shell", std::string(ATOM_VERSION_STRING));  // For compatibility.
        versions.Set("chrome", std::string(CHROME_VERSION_STRING));
        versions.Set("electron", std::string(ATOM_VERSION_STRING));
    }
}

node::Environment* NodeBindings::createEnvironment(v8::Local<v8::Context> context) {
    uv_async_init(m_uvLoop, m_callNextTickAsync, onCallNextTick);
    m_callNextTickAsync->data = this;

    std::vector<std::string> args = AtomCommandLine::argv();

    // Feed node the path to initialization script.
    std::wstring processType = m_isBrowser ? FILE_PATH_LITERAL("browser") : FILE_PATH_LITERAL("renderer");
    std::wstring resourcesPath = getResourcesPath(FILE_PATH_LITERAL(""));
    std::wstring scriptPath = resourcesPath // .append(FILE_PATH_LITERAL("electron.asar"))
        .append(processType)
        .append(FILE_PATH_LITERAL("\\init.js"));
    std::string scriptPathStr = StringUtil::UTF16ToUTF8(scriptPath);
    args.insert(args.begin() + 1, scriptPathStr.c_str());

    std::unique_ptr<const char*[]> c_argv = stringVectorToArgArray(args);
    m_env = node::CreateEnvironment(context->GetIsolate(), m_uvLoop, context, args.size(), c_argv.get(), 0, nullptr);
    if (!m_isBrowser)
        m_env->set_is_blink_core();

//     const char* argv1[] = { "electron.exe", "E:\\mycode\\miniblink49\\trunk\\electron\\lib\\init.js" };
//     node::Environment* env = node::CreateEnvironment(context->GetIsolate(), m_uvLoop, context, 2, argv1, 2, argv1);

    // Node turns off AutorunMicrotasks, but we need it in web pages to match the
    // behavior of Chrome.
//     if (!m_isBrowser)
//         context->GetIsolate()->SetAutorunMicrotasks(true);

    gin::Dictionary process(context->GetIsolate(), m_env->process_object());
    process.Set("type", StringUtil::UTF16ToUTF8(processType));
    process.Set("resourcesPath", StringUtil::UTF16ToUTF8(resourcesPath));
    // Do not set DOM globals for renderer process.
    if (!m_isBrowser)
        process.Set("_noBrowserGlobals", StringUtil::UTF16ToUTF8(resourcesPath));

    bindFunction(&process);

    // The path to helper app.
//     base::FilePath helper_exec_path;
//     PathService::Get(content::CHILD_PROCESS_EXE, &helper_exec_path);
//     process.Set("helperExecPath", helper_exec_path);
    return m_env;
}

void NodeBindings::loadEnvironment() {
    node::LoadEnvironment(m_env);
    mate::emitEvent(m_env->isolate(), m_env->process_object(), "loaded");
}

void NodeBindings::activateUVLoop(v8::Isolate* isoloate) {
    node::Environment* env = node::Environment::GetCurrent(isoloate);
    if (std::find(m_pendingNextTicks.begin(), m_pendingNextTicks.end(), env) != m_pendingNextTicks.end())
        return;

    m_pendingNextTicks.push_back(env);
    uv_async_send(m_callNextTickAsync);
}

// static
void NodeBindings::onCallNextTick(uv_async_t* handle) {
    NodeBindings* self = static_cast<NodeBindings*>(handle->data);

    for (std::list<node::Environment*>::const_iterator it = self->m_pendingNextTicks.begin();
        it != self->m_pendingNextTicks.end(); ++it) {
        node::Environment* env = *it;
        v8::HandleScope handleScope(env->isolate());

        // KickNextTick, copied from node.cc:
        node::Environment::AsyncCallbackScope callbackScope(env);
        if (callbackScope.in_makecallback())
            continue;

        node::Environment::TickInfo* tickInfo = env->tick_info();
        if (tickInfo->length() == 0)
            env->isolate()->RunMicrotasks();

        v8::Local<v8::Object> process = env->process_object();
        if (tickInfo->length() == 0)
            tickInfo->set_index(0);

        env->tick_callback_function()->Call(process, 0, nullptr).IsEmpty();
    }

    self->m_pendingNextTicks.clear();
}

} // atom