#include <windows.h>
#include <process.h>
#include <vector>
#include "G:\\m75_xp\\src\\patch_code\\sync_xp.h"

static INIT_ONCE cpu_check_inited_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK _x86_check_features(PINIT_ONCE once, PVOID param, PVOID* context)
{
    OutputDebugStringA("_x86_check_features\n");
    return TRUE;
}

unsigned CALLBACK testSyn(void* param)
{
    int time = rand() % 100;
    ::Sleep(time);
    InitOnceExecuteOnceXp(&cpu_check_inited_once, _x86_check_features, NULL, NULL);
    return 0;
}

void testSynMain()
{
    srand(1);

    const int kMaxCount = 900;
    for (int i = 0; i < kMaxCount; ++i) {
        unsigned threadIdentifier = 0;
        HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, testSyn, nullptr, 0, &threadIdentifier));
    }
    OutputDebugStringA("testSynMain over\n");
    ::Sleep(100000);
}

void testGuard()
{
    std::vector<char> buffer;
    HANDLE hFile = CreateFileW(L"G:\\test\\pakage\\electron\\msvcp140.dll", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        DebugBreak();
        return;
    }

    DWORD fileSizeHigh;
    const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

    DWORD numberOfBytesRead = 0;
    buffer.resize(bufferSize);
    ::ReadFile(hFile, &buffer.at(0), bufferSize, &numberOfBytesRead, nullptr);

    IMAGE_DOS_HEADER* imgDosHdr = (IMAGE_DOS_HEADER*)&buffer.at(0);
    IMAGE_OPTIONAL_HEADER* imgOptHdr = (IMAGE_OPTIONAL_HEADER*)((DWORD)&buffer.at(0) + imgDosHdr->e_lfanew + 24);
    IMAGE_IMPORT_DESCRIPTOR* imgImportDes = (IMAGE_IMPORT_DESCRIPTOR*)((DWORD)&buffer.at(0) + imgOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    IMAGE_THUNK_DATA* imgThunkData = NULL;

    imgOptHdr->DllCharacteristics &= 49151;

    hFile = CreateFileW(L"G:\\test\\pakage\\electron\\msvcp140_1.dll", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    DWORD dwWritten = 0;
    WriteFile(hFile, &buffer.at(0), buffer.size(), &dwWritten, NULL);

    return;
}

