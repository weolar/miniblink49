
#include "electron.h"

#include "common/ThreadCall.h"
#include "NodeRegisterHelp.h"
#include "NodeBlink.h"
#include "lib/native.h"
#include <windows.h>
#include "base/thread.h"

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

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_web_contents)
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_app)
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_window)

static void registerNodeMod() {
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_web_contents);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_app);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_window);
}

static void gcTimerCallBack(uv_timer_t* handle) {
    return;

    v8::Isolate *isolate = (v8::Isolate *)(handle->data);
    if (isolate)
        isolate->LowMemoryNotification();
}

void nodeInitCallBack(NodeArgc* n) {
    base::SetThreadName("NodeCore");
    gcTimer.data = n->childEnv->isolate();
    uv_timer_init(n->childLoop, &gcTimer);
    uv_timer_start(&gcTimer, gcTimerCallBack, 1000 * 10, 1);

    uv_loop_t* loop = n->childLoop;
    atom::ThreadCall::init(loop);
    atom::ThreadCall::messageLoop(loop);
}

} // atom

int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    atom::registerNodeMod();
    
    atom::ThreadCall::createBlinkThread();
    
    wchar_t* argv1[] = { L"electron.exe", L"init.js" };
    node::NodeArgc* node = node::runNodeThread(2, argv1, atom::nodeInitCallBack, nullptr);
    
    uv_loop_t* loop = uv_default_loop();
    atom::ThreadCall::messageLoop(loop);
    atom::ThreadCall::shutdown();

	return 0;
}

int main() {
    return wWinMain(0, 0, 0, 0);
}