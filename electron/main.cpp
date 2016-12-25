#include <windows.h>
#include "wke.h"

extern "C" bool __declspec(dllexport) RunNodeThread(int argc, wchar_t *wargv[]);
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	wkeInitialize();

	wchar_t* argv[] = { L"cefclient.exe",L"node_modules\\HiChat\\server.js" };
	RunNodeThread(2, argv);
	wchar_t* argv1[] = { L"cefclient.exe",L"server.js" };
	RunNodeThread(2, argv1);
	wchar_t* argv2[] = { L"cefclient.exe",L"server1.js" };
	RunNodeThread(2, argv2);

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