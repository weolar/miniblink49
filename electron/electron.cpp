
#include "electron.h"

#include "common/ThreadCall.h"
#include "common/NodeRegisterHelp.h"
#include "NodeBlink.h"
#include "lib/native.h"
#include "base/thread.h"
#include <windows.h>
#include <shlwapi.h>
#include <xstring>

using namespace v8;
using namespace node;

#if USING_VC6RT == 1
void __cdecl operator delete(void * p, unsigned int)
{
    ::free(p);
}

extern "C" int __security_cookie = 0;
#endif

namespace atom {

uv_timer_t gcTimer;

static std::wstring* kResPath = nullptr;

std::wstring getResPath(const std::wstring& name) {
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
    sourceW = getResPath(sourceW);

    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (::GetFileAttributesExW(sourceW.c_str(), GetFileExInfoStandard, &attrs) == 0)
        return;

    HANDLE fileHandle = ::CreateFileW(sourceW.c_str(), FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle == INVALID_HANDLE_VALUE)
        return;

    std::vector<char>* buffer = new std::vector<char>(); // 内存泄漏
    buffer->resize(attrs.nFileSizeLow);

    DWORD bytesRead;
    int retval = ::ReadFile(fileHandle, &buffer->at(0), attrs.nFileSizeLow, &bytesRead, 0);
    ::CloseHandle(fileHandle);

    if (retval == 0 || bytesRead != attrs.nFileSizeLow)
        return;

    nativePath->source = &buffer->at(0);
    nativePath->sourceLen = bytesRead;
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_web_contents)
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_app)
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_electron)
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_window)

static void registerNodeMod() {
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_web_contents);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_app);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_electron);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_window);
}

static void gcTimerCallBack(uv_timer_t* handle) {
    return;

    v8::Isolate *isolate = (v8::Isolate*)(handle->data);
    if (isolate)
        isolate->LowMemoryNotification();
}

void nodeInitCallBack(NodeArgc* n) {
    gcTimer.data = n->childEnv->isolate();
    uv_timer_init(n->childLoop, &gcTimer);
    uv_timer_start(&gcTimer, gcTimerCallBack, 1000 * 10, 1);

    uv_loop_t* loop = n->childLoop;
    atom::ThreadCall::init(loop);
    atom::ThreadCall::messageLoop(loop, n->v8platform, v8::Isolate::GetCurrent());
}

void nodePreInitCallBack(NodeArgc* n) {
    base::SetThreadName("NodeCore");
    ThreadCall::createBlinkThread(n->v8platform);
}

} // atom

int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    atom::registerNodeMod();
       
    std::wstring initScript = atom::getResPath(L"init.js");
    const wchar_t* argv1[] = { L"electron.exe", initScript.c_str() };
    node::NodeArgc* node = node::runNodeThread(2, argv1, atom::nodeInitCallBack, atom::nodePreInitCallBack, nullptr);
    
    uv_loop_t* loop = uv_default_loop();
    atom::ThreadCall::messageLoop(loop, nullptr, nullptr);
    atom::ThreadCall::shutdown();

    delete atom::kResPath;

	return 0;
}

int main() {
    return wWinMain(0, 0, 0, 0);
}