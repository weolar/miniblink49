#include "electron.h"

#include "common/ThreadCall.h"
#include "common/NodeRegisterHelp.h"
#include "common/NodeThread.h"
#include "common/AtomCommandLine.h"
#include "third_party/zlib/unzip.h"
#include "NodeBlink.h"
#include <windows.h>

#if USING_VC6RT == 1
void __cdecl operator delete(void * p, unsigned int)
{
    ::free(p);
}

extern "C" int __security_cookie = 0;
#endif

#pragma comment(lib,"zlib.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Psapi.lib")

namespace atom {

unzFile gPeResZip = nullptr;

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_web_contents);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_app);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_electron);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_window);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_menu);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_browser_dialog);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_renderer_ipc);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_common_v8_util);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_common_shell);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_common_original_fs);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_common_screen);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_renerer_webframe);
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DECLARE_IN_MAIN(atom_common_intl_collator);

static void registerNodeMod() {
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_web_contents);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_app);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_electron);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_window);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_menu);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_browser_dialog);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_renderer_ipc);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_common_v8_util);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_common_shell);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_common_original_fs);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_common_screen);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_renerer_webframe);
    NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_DEFINDE_IN_MAIN(atom_common_intl_collator);
}

static void initPeRes(HINSTANCE hInstance) {
    PIMAGE_DOS_HEADER pDosH = (PIMAGE_DOS_HEADER)hInstance;
    if (pDosH->e_magic != IMAGE_DOS_SIGNATURE)
        return; // DOS头不正确
    PIMAGE_NT_HEADERS32 pNtH = (PIMAGE_NT_HEADERS32)((DWORD)hInstance + (DWORD)pDosH->e_lfanew);
    if (pNtH->Signature != IMAGE_NT_SIGNATURE)
        return; // NT头不正确
    PIMAGE_SECTION_HEADER pSecHTemp = IMAGE_FIRST_SECTION(pNtH); // 区段头

    for (size_t index = 0; index < pNtH->FileHeader.NumberOfSections; index++) {
        if (memcmp(pSecHTemp->Name, ".pack\0\0\0", 8) == 0) { // 比较区段名
            // 找到资源包区段
            gPeResZip = unzOpen(NULL, hInstance + pSecHTemp->VirtualAddress + pSecHTemp->PointerToRawData, pSecHTemp->Misc.VirtualSize);
        }
        ++pSecHTemp;
    }
}

} // atom

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    atom::AtomCommandLine::initAW();
    atom::ThreadCall::setMainThread();
    atom::initPeRes(hInstance); // 初始化PE打包的资源
    atom::registerNodeMod();

    // E:\mycode\vscode-master\out\main.js
    // E:\mycode\miniblink49\trunk\electron\lib\test\css3-3d-cube-loading\main.js
    std::vector<std::wstring> argv = atom::AtomCommandLine::wargv();
    if (-1 != argv[1].find(L"bootstrap", 0)) {
        //MessageBoxW(0, 0, 0, 0);
    }

    atom::NodeArgc* node = atom::runNodeThread();
    
    uv_loop_t* loop = uv_default_loop();
    atom::ThreadCall::messageLoop(loop, nullptr, nullptr);
    atom::ThreadCall::shutdown();

    //delete atom::kResPath;

	return 0;
}

int main() {
    return wWinMain(::GetModuleHandle(NULL), 0, 0, 0);
}