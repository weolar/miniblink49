#include "src\node.h"
#include <Windows.h>
#include <process.h>

#pragma comment(lib,"openssl.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Psapi.lib")
typedef struct {
	char** argv;
	int argc;
} nodeargc;
unsigned __stdcall nodeThread(void * p) {
	// Now that conversion is done, we can finally start.
	int ret = node::Start(((nodeargc *)p)->argc, ((nodeargc *)p)->argv);
	free(p);
	return ret;
}
extern "C" bool __declspec(dllexport) RunNode(int argc, wchar_t *wargv[]) {
	// Convert argv to to UTF8
	nodeargc *p = (nodeargc *)malloc(sizeof(nodeargc));
	p->argv = new char*[argc + 1];
	for (int i = 0; i < argc; i++) {
		// Compute the size of the required buffer
		DWORD size = WideCharToMultiByte(CP_UTF8,
			0,
			wargv[i],
			-1,
			nullptr,
			0,
			nullptr,
			nullptr);
		if (size == 0) {
			// This should never happen.
			fprintf(stderr, "Could not convert arguments to utf8.");
			exit(1);
		}
		// Do the actual conversion
		p->argv[i] = new char[size];
		DWORD result = WideCharToMultiByte(CP_UTF8,
			0,
			wargv[i],
			-1,
			p->argv[i],
			size,
			nullptr,
			nullptr);
		if (result == 0) {
			// This should never happen.
			fprintf(stderr, "Could not convert arguments to utf8.");
			exit(1);
		}
	}
	p->argv[argc] = nullptr;
	p->argc = argc;
	unsigned threadIdentifier = 0;
	_beginthreadex(0, 0, &nodeThread, p, 0, &threadIdentifier);
}