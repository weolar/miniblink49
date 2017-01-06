#include <windows.h>
#include "wke.h"
#include "nodeblink.h"
#include "lib\native.h"
using namespace v8;
using namespace node;

void InitCallBack(node::nodeargc* p) {
	node::Environment*env = node::NodeGetEnvironment(p);
	//执行electron初始化脚本
	Local<String> script_name = FIXED_ONE_BYTE_STRING(env->isolate(), "init.js");
	Local<Value> f_value = node::ExecuteString(env, script_name, String::NewFromUtf8(
		env->isolate(),
		reinterpret_cast<const char*>(init),
		NewStringType::kNormal,
		sizeof(init)).ToLocalChecked());
	// The bootstrap_node.js file returns a function 'f'
	//CHECK(f_value->IsFunction());
	//Local<Function> f = Local<Function>::Cast(f_value);
	//f->Call(Null(env->isolate()), 0, NULL);
}


int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	wkeInitialize();


	wchar_t* argv1[] = { L"electron.exe" };
	node::nodeargc *node = node::RunNodeThread(1, argv1, InitCallBack);

	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.message;
}
int main() {
	return wWinMain(0, 0, 0, 0);
}