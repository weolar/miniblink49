#include <windows.h>
#include "electron.h"
#include "lib/native.h"

using namespace v8;
using namespace node;

namespace atom {

struct TaskAsyncData {
    CoreMainTask call;
    void* data;
    HANDLE event;
    void* ret;
	uv_mutex_t mutex;
};
TaskAsyncData mainAsync;

static void mainAsyncCallback(uv_async_t* handle) {
    if (mainAsync.call) {
        mainAsync.ret = mainAsync.call(mainAsync.data);
        ::PulseEvent(mainAsync.event);
    }
}

void mainAsyncCall(CoreMainTask call, void* data) {
	uv_mutex_lock(&mainAsync.mutex);
	mainAsync.call = call;
    mainAsync.data = data;
    uv_async_send((uv_async_t*)&mainAsync);
}

void* mainAsyncWait() {
    ::WaitForSingleObject(mainAsync.event, INFINITE);
	uv_mutex_unlock(&mainAsync.mutex);
    return mainAsync.ret;
}

void* mainSyncCall(CoreMainTask call, void* data) {
    mainAsyncCall(call, data);
	void* ret = mainAsyncWait();
    return ret;
}

} // atom

int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    wkeInitialize();

    uv_loop_t* loop = uv_default_loop();

    atom::mainAsync.event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    uv_async_init(loop, (uv_async_t*)&atom::mainAsync, atom::mainAsyncCallback);
	uv_mutex_init(&atom::mainAsync.mutex);

    wchar_t* argv1[] = { L"electron.exe", L"init.js" };
    node::NodeArgc* node = node::runNodeThread(2, argv1, NULL);

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
}

int main() {
    return wWinMain(0, 0, 0, 0);
}