#include <windows.h>
#include "wke.h"
#include "nodeblink.h"

int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	wkeInitialize();



	wchar_t* argv1[] = { L"cefclient.exe",L"server.js" };
	node::RunNodeThread(2, argv1);

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