#include "config.h"
#include "third_party/WebKit/Source/platform/heap/SafePoint.h"

#ifdef _M_X64

unsigned char pushAllRegistersShellCode[] = {
    0x6A, 0x00, // push 0    
    0x56,     // push rsi 
    0x57, // push rdi  
    0x53, // push rbx  
    0x55, // push rbp  
    0x41, 0x54, // push        r12  
    0x41, 0x55, // push        r13  
    0x41, 0x56, // push        r14
    0x41, 0x57, // push        r15
    0x4D, 0x8B, 0xC8, // mov r9,r8  
    0x4C, 0x8B, 0xC4, // mov r8,rsp  
    0x41, 0xFF, 0xD1, // call r9
    0x48, 0x83, 0xC4, 0x48, // add rsp,48h  
    0xC3, // ret
};

namespace blink {
using PushAllRegistersCallback = void(*)(SafePointBarrier*, ThreadState*, intptr_t*);
}

extern "C" void pushAllRegisters(blink::SafePointBarrier* p1, blink::ThreadState* p2, blink::PushAllRegistersCallback p3)
{
    void* pushAllRegistersPtr = (void*)pushAllRegistersShellCode;
    blink::PushAllRegistersCallback cb = static_cast<blink::PushAllRegistersCallback>(pushAllRegistersPtr);

    static BOOL isInit = FALSE;
    if (!isInit) {
        DWORD oldProtect = 0;
        ::VirtualProtect(pushAllRegistersPtr, sizeof(pushAllRegistersShellCode), PAGE_EXECUTE_READWRITE, &oldProtect);
        isInit = TRUE;
    }
    cb(p1, p2, (intptr_t *)p3);
}

#else

extern "C" __declspec(naked) void __cdecl pushAllRegisters(void*, void*, void*)
{
    // Push all callee - saves registers to get them
    // on the stack for conservative stack scanning.
    // We maintain 16 - byte alignment at calls(required on
    // Mac).There is a 4 - byte return address on the stack
    // and we push 28 bytes which maintains 16 - byte alignment
    //; at the call.
    __asm {
        push ebx;
        push ebp;
        push esi;
        push edi;
        ;; Pass the two first arguments unchanged and the
        ;; stack pointer after pushing callee - save registers
        ;; to the callback.
        mov ecx, [esp + 28];
        push esp;
        push [esp + 28];
        push [esp + 28];
        call ecx;
        ;; Pop arguments and the callee - saved registers.
        ;; None of the callee - saved registers were modified
        ;; so we do not need to restore them.
        add esp, 28;
        ret;
    }
}

#endif