#include "common/NodeBinding.h"

#include "uv.h"
#include "nodeblink.h"
#include "gin/dictionary.h"
#include "base/file_path.h"
#include "common/StringUtil.h"
#include "common/AtomCommandLine.h"
#include <xstring>
#include <vector>
#include <memory>
#include <shlwapi.h>

namespace atom {

NodeBindings::NodeBindings(bool isBrowser, uv_loop_t* uvLoop)
    : m_isBrowser(isBrowser)
    , m_uvLoop(uvLoop) {
}

NodeBindings::~NodeBindings() {
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

    std::vector<char>* buffer = new std::vector<char>(); // ÄÚ´æÐ¹Â©
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
std::unique_ptr<const char*[]> stringVectorToArgArray(
    const std::vector<std::string>& vector) {
    std::unique_ptr<const char*[]> array(new const char*[vector.size()]);
    for (size_t i = 0; i < vector.size(); ++i) {
        array[i] = vector[i].c_str();
    }
    return array;
}

node::Environment* NodeBindings::createEnvironment(v8::Local<v8::Context> context) {
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
    node::Environment* env = node::CreateEnvironment(context->GetIsolate(), m_uvLoop, context, args.size(), c_argv.get(), 0, nullptr);

//     const char* argv1[] = { "electron.exe", "E:\\mycode\\miniblink49\\trunk\\electron\\lib\\init.js" };
//     node::Environment* env = node::CreateEnvironment(context->GetIsolate(), m_uvLoop, context, 2, argv1, 2, argv1);

    // Node turns off AutorunMicrotasks, but we need it in web pages to match the
    // behavior of Chrome.
//     if (!m_isBrowser)
//         context->GetIsolate()->SetAutorunMicrotasks(true);

    gin::Dictionary process(context->GetIsolate(), env->process_object());
    process.Set("type", StringUtil::UTF16ToUTF8(processType));
    process.Set("resourcesPath", StringUtil::UTF16ToUTF8(resourcesPath));
    // Do not set DOM globals for renderer process.
    if (!m_isBrowser)
        process.Set("_noBrowserGlobals", StringUtil::UTF16ToUTF8(resourcesPath));

    // The path to helper app.
//     base::FilePath helper_exec_path;
//     PathService::Get(content::CHILD_PROCESS_EXE, &helper_exec_path);
//     process.Set("helperExecPath", helper_exec_path);
    return env;
}

} // atom