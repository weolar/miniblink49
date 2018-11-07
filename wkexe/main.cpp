#include "app.h"

#ifndef _WIN64
int WINAPI WinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPSTR lpCmdLine,
    __in int nShowCmd
    )
#else
int main()
#endif
{
	wkeInitialize();
	{
		Application app;
		RunApplication(&app);
	}
	wkeFinalize();

	return 0;
}