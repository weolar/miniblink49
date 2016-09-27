#include "config.h"
#include "third_party/WebKit/Source/platform/heap/SafePoint.h"

extern "C" __declspec(naked) void pushAllRegisters(void*, void*, void*)
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