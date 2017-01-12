#include <windows.h>
#include "electron.h"
#include "lib\native.h"
using namespace v8;
using namespace node;

struct task_async_data {
	uv_async_t main_async;
	core_main_task call;
	void *data;
	HANDLE event;
	void *ret;
};

uv_loop_t *loop;			//主loop
task_async_data main_async;
static void main_async_cb(uv_async_t* handle)
{
	if (main_async.call) {
		main_async.ret = main_async.call(main_async.data);
		PulseEvent(main_async.event);
	}
}
void main_async_call(core_main_task call, void *data) {
	main_async.call = call;
	main_async.data = data;
	uv_async_send((uv_async_t*)&main_async);
}
void *main_async_wait() {
	WaitForSingleObject(main_async.event, INFINITE);
	return main_async.ret;
}
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	wkeInitialize();
	//初始化循环
	loop = uv_default_loop();
	//初始化async
	main_async.event = CreateEvent(NULL, FALSE, FALSE, NULL);
	uv_async_init(loop, (uv_async_t*)&main_async, main_async_cb);

	wchar_t* argv1[] = { L"electron.exe", L"init.js" };
	node::nodeargc *node = node::RunNodeThread(2, argv1, NULL);

	MSG msg;
	bool more;
	while (true) {
		more = uv_run(loop, UV_RUN_NOWAIT);
		if (GetMessageW(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			break;
		}
	}
}
int main() {
	return wWinMain(0, 0, 0, 0);
}