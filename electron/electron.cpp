
#include "electron.h"

#include "lib/native.h"
#include "NodeRegisterHelp.h"

#include <windows.h>

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

struct TaskAsyncData {
	uv_async_t async;
    CoreMainTask call;
    void* data;
    HANDLE event;
    void* ret;
	uv_mutex_t mutex;
    uv_thread_t main_thread_id;
};
TaskAsyncData* g_asyncData = nullptr;
uv_timer_t gcTimer;

static void callbackInUiThread(uv_async_t* handle) {
    if (g_asyncData->call) {
        g_asyncData->ret = g_asyncData->call(g_asyncData->data);
        ::PulseEvent(g_asyncData->event);
    }
}

void callUiThreadAsync(CoreMainTask call, void* data) {
	uv_mutex_lock(&g_asyncData->mutex);
    g_asyncData->call = call;
    g_asyncData->data = data;
    uv_async_send((uv_async_t*)g_asyncData);
}

void* waitForCallUiThreadAsync() {
    ::WaitForSingleObject(g_asyncData->event, INFINITE);
	uv_mutex_unlock(&g_asyncData->mutex);
    return g_asyncData->ret;
}

void* callUiThreadSync(CoreMainTask call, void* data) {
    callUiThreadAsync(call, data);
	void* ret = waitForCallUiThreadAsync();
    return ret;
}

bool callUiThreadSync(v8::FunctionCallback call, const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (uv_thread_self() == g_asyncData->main_thread_id)
        return false;
    callUiThreadAsync((CoreMainTask)call, (void *)&args);
    waitForCallUiThreadAsync();
    return true;
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_web_contents)
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_app)
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_window)

static void registerNodeMod() {
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_web_contents);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_app);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_window);
}

static void gcTimerCallBack(uv_timer_t* handle) {
    v8::Isolate *isolate = (v8::Isolate *)(handle->data);
    if (isolate)
        isolate->LowMemoryNotification();
}
void nodeInitCallBack(NodeArgc* n) {
    gcTimer.data = n->childEnv->isolate();
    uv_timer_init(n->childLoop, &gcTimer);
    uv_timer_start(&gcTimer, gcTimerCallBack, 1000 * 10, 1);
}

} // atom

int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    atom::registerNodeMod();

    wkeInitialize();

    uv_loop_t* loop = uv_default_loop();

    atom::g_asyncData = new atom::TaskAsyncData();
    atom::g_asyncData->event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    uv_async_init(loop, (uv_async_t*)atom::g_asyncData, atom::callbackInUiThread);
	uv_mutex_init(&atom::g_asyncData->mutex);
    atom::g_asyncData->main_thread_id = uv_thread_self();

    wchar_t* argv1[] = { L"electron.exe", L"init.js" };
    node::NodeArgc* node = node::runNodeThread(2, argv1, atom::nodeInitCallBack, NULL);

    MSG msg;
    bool more;
    while (true) {
        more = (0 != uv_run(loop, UV_RUN_NOWAIT));
        if (GetMessageW(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else
            break;
    }

	wkeFinalize();

    delete atom::g_asyncData;

	return msg.message;
}

int main() {
    return wWinMain(0, 0, 0, 0);
}