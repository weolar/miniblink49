;++
;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
;
; Module:
;
;   macamd64.w
;
; Astract:
;
;   Contains AMD64 public architecture constants and assembly macros.
;
; Author:
;
;   David N. Cutler (davec) 27-May-2000
;
;--

;++
;
; push_reg <reg>
;
; Macro Description:
;
;   This macro emits a single-byte push <reg> instruction in a
;   nested prologue, as well as the associated unwind code.
;
; Arguments:
;
;   reg - supplies the integer register to push
;
;--

push_reg macro Reg

        push    Reg
        .pushreg Reg

        endm

;++
;
; rex_push_reg <reg>
;
; Macro Description:
;
;   This macro emits a single-byte push <reg> instruction in a
;   nested prologue, as well as the associated unwind code.
;
;   This differs from push_reg only in that a redundant rex prefix
;   is added.  rex_push_reg must be used in lieu of push_reg when it
;   appears as the first instruction in a function, as the calling
;   standard dictates that functions must not begin with a single
;   byte instruction.
;
; Arguments:
;
;   reg - supplies the integer register to push
;
;--

rex_push_reg macro Reg

        db      048h
        push    Reg
        .pushreg Reg

        endm

;++
;
; push_eflags
;
; Macro Description:
;
;   This macro emits a single-byte pushfq instruction in a
;   nested prologue, as well as the associated unwind code.
;
; Arguments:
;
;   none
;
;--

push_eflags macro

        pushfq
        .allocstack 8

        endm

;++
;
; rex_push_eflags
;
; Macro Description:
;
;   This macro emits a single-byte pushfq instruction in a
;   nested prologue, as well as the associated unwind code.
;
;   This differs from push_eflags only in that a redundant rex prefix
;   is added.  rex_push_eflags must be used in lieu of push_eflags when it
;   appears as the first instruction in a function, as the calling
;   standard dictates that functions must not begin with a single
;   byte instruction.
;
; Arguments:
;
;   none
;
;--

rex_push_eflags macro

        db      048h
        pushfq
        .allocstack 8

        endm

;++
;
; alloc_stack <Size>
;
; Macro Description:
;
;   This macro emits an opcode to subtract <Size> from rsp, as well
;   as the associated unwind code.
;
; Arguments:
;
;   Size - The number of bytes to subtract from rsp.
;
;--

alloc_stack macro Size

        sub     rsp, Size
        .allocstack Size

        endm

;++
;
; save_reg   <Reg>, <Offset>
;
; Macro Description:
;
;   This macro emits an opcode to save the non-volatile 64-bit general purpose
;   register indicated by <Reg> at offset <Offset> relative to the current
;   position of the stack pointer.  It also generates the associated unwind
;   code.
;
; Arguments:
;
;   Reg - Supplies the integer register to save
;
;   Offset - Supplies the offset relative to the current position of the stack
;            pointer.
;
;--

save_reg macro Reg, Offset

        mov     Offset[rsp], Reg
        .savereg Reg, Offset

        endm

;++
;
; save_xmm128   <Reg>, <Offset>
;
; Macro Description:
;
;   This macro emits an opcode to save the 128-bit non-volatile xmm register
;   indicated by <Reg> at offset <Offset> relative to the current position
;   of the stack pointer.  It also generates the associated unwind code.
;
; Arguments:
;
;   Reg - Supplies the xmm register register to save
;
;   Offset - Supplies the offset relative to the current position of the stack
;            pointer.
;
;--

save_xmm128 macro Reg, Offset

        movdqa  Offset[rsp], Reg
        .savexmm128 Reg, Offset

        endm

;++
;
; push_frame
;
; Macro Description:
;
;   This macro emits unwind data indicating that a machine frame has been
;   pushed on the stack (usually by the CPU in response to a trap or fault).
;
; Arguments:
;
;   None.
;
;--

push_frame macro Code

        .pushframe Code

        endm

;++
;
; set_frame <Reg>, <Offset>
;
; Macro Description:
;
;   This macro emits an opcode and unwind data establishing the use of <Reg>
;   as the current stack frame pointer.
;
; Arguments:
;
;   Reg - Supplies the integer register to use as the current stack frame
;         pointer.
;
;   Offset - Supplies the optional offset of the frame pointer relative to
;            the stack frame.  In stack frames greater than 080h bytes,
;            a non-zero offset can help reduce the size of subsequent opcodes
;            that access portions of the stack frame by facilitating the use of
;            positive and negative single-byte displacements.
;
;            If not supplied, no offset is assumed.
;
;--

set_frame macro Reg, Offset

if Offset

        lea     Reg, Offset[rsp]

else

        mov     Reg, rsp

endif

        .setframe Reg, Offset

        endm

;++
;
; END_PROLOGUE
;
; Macro Description:
;
;   This macro marks the end of the prologue.  This must appear after all
;   of the prologue directives in a nested function.
;
; Arguments:
;
;   None.
;
;--

END_PROLOGUE macro

        .endprolog

        endm

;++
;
; LEAF_ENTRY <Name>, <Section>, <NoPad>
;
; Macro Description:
;
;   This macro indicates the beginning of a leaf function.
;
;   A leaf function is one that DOES NOT:
;
;   - manipulate non-volatile registers
;   - manipulate the stack pointer
;   - call other functions
;   - reference an exception handler
;   - contain a prologue
;   - have any unwind data associated with it
;
; Arguments:
;
;   Name - Supplies the name of the function
;
;   Section - Supplies the name of the section within which the function
;             is to appear
;
;   NoPad - If present, indicates that the function should not be prefixed
;           with 6 bytes of padding.  This is for internal use only - the
;           calling standard dictates that functions (nested and leaf) must
;           be prefixed with padding.
;
;--

LEAF_ENTRY macro Name, Section, NoPad

Section segment para 'CODE'

ifb <NoPad>

        db      6 dup (0cch)

endif

        align   16

        public  Name
Name    proc

        endm

;++
;
; LEAF_END <Name>, <Section>
;
; Macro Description:
;
;   This macro indicates the end of a leaf function.  It must be paired
;   with a LEAF_ENTRY macro that includes matching Name and Section
;   parameters.
;
; Arguments:
;
;   Name - Supplies the name of the function.  Must match that supplied to
;          the corresponding LEAF_ENTRY macro.
;
;   Section - Supplies the name of the section within which the function
;             is to appear.  Must match that supplied to the corresponding
;             LEAF_ENTRY macro.
;
;--

LEAF_END macro Name, section

Name    endp

Section ends

        endm

;++
;
; NESTED_ENTRY <Name>, <Section>, <Handler>, <NoPad>
;
; Macro Description:
;
;   This macro indicates the beginning of a nested function.
;
;   A nested function is one that does any of the following:
;
;   - manipulates non-volatile registers
;   - manipulates the stack pointer
;   - references an exception handler
;   - calls other functions
;
;   A nested function must include a prologue with unwind data.
;
; Arguments:
;
;   Name - Supplies the name of the function.
;
;   Section - Supplies the name of the section within which the function
;             is to appear.
;
;   Handler - Supplies the name of the handler for exceptions raised
;             within the scope of this function.
;
;   NoPad - If present, indicates that the function should not be prefixed
;           with 6 bytes of padding.  This is for internal use only - the
;           calling standard dictates that functions (nested and leaf) must
;           be prefixed with padding.
;
;--

NESTED_ENTRY macro Name, Section, Handler, NoPad

ifdef _CurrentSection_

ifdif <Section>, _CurrentSection_

.err <NESTED_ENTRY invoked for different sections within same module>

endif

endif

_CurrentSection_ EQU <Section>

Section segment para 'CODE'

ifb <NoPad>

        db      6 dup (0cch)

endif

        align   16

        public  Name

ifb <Handler>

Name    proc    frame

else

Name    proc    frame:Handler

endif

        endm

;++
;
; NESTED_END <Name>, <Section>
;
; Macro Description:
;
;   This macro indicates the end of a nested function.  It must be paired
;   with a NESTED_ENTRY macro that includes matching Name and Section
;   parameters.
;
; Arguments:
;
;   Name - Supplies the name of the function.  Must match that supplied to
;          the corresponding NESTED_ENTRY macro.
;
;   Section - Supplies the name of the section within which the function
;             is to appear.  Must match that supplied to the corresponding
;             NESTED_ENTRY macro.
;
;--

NESTED_END macro Name, section

Name    endp

Section ends

        endm

;++
;
; ALTERNATE_ENTRY <Name>
;
; Macro Description:
;
;   This macro indicates an alternate entry point in a function, or
;   a synonymous name for an existing function.
;
; Arguments:
;
;   Name - Supplies the name of the alternate entry point.
;
;--

ALTERNATE_ENTRY macro Name

Name:
        endm

;++
;
; Yield
;
; Macro Description:
;
;   This macro generates a yield instruction, interpreted by SMT processors
;   as an indication of a stall or idle condition.
;
; Arguments:
;
;   None.
;
;--

Yield   macro

        pause                           ; yield execution on SMT processors

        endm

