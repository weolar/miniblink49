// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/profiler/win32_stack_frame_unwinder.h"

#include <windows.h>

#include "base/containers/hash_tables.h"
#include "base/memory/singleton.h"
#include "base/stl_util.h"

namespace base {

// Win32UnwindFunctions -------------------------------------------------------

const HMODULE ModuleHandleTraits::kNonNullModuleForTesting = reinterpret_cast<HMODULE>(static_cast<uintptr_t>(-1));

// static
bool ModuleHandleTraits::CloseHandle(HMODULE handle)
{
    if (handle == kNonNullModuleForTesting)
        return true;

    return ::FreeLibrary(handle) != 0;
}

// static
bool ModuleHandleTraits::IsHandleValid(HMODULE handle)
{
    return handle != nullptr;
}

// static
HMODULE ModuleHandleTraits::NullHandle()
{
    return nullptr;
}

namespace {

    // Implements the UnwindFunctions interface for the corresponding Win32
    // functions.
    class Win32UnwindFunctions : public Win32StackFrameUnwinder::UnwindFunctions {
    public:
        Win32UnwindFunctions();
        ~Win32UnwindFunctions() override;

        PRUNTIME_FUNCTION LookupFunctionEntry(DWORD64 program_counter,
            PDWORD64 image_base) override;

        void VirtualUnwind(DWORD64 image_base,
            DWORD64 program_counter,
            PRUNTIME_FUNCTION runtime_function,
            CONTEXT* context) override;

        ScopedModuleHandle GetModuleForProgramCounter(
            DWORD64 program_counter) override;

    private:
        DISALLOW_COPY_AND_ASSIGN(Win32UnwindFunctions);
    };

    Win32UnwindFunctions::Win32UnwindFunctions() { }
    Win32UnwindFunctions::~Win32UnwindFunctions() { }

    PRUNTIME_FUNCTION Win32UnwindFunctions::LookupFunctionEntry(
        DWORD64 program_counter,
        PDWORD64 image_base)
    {
#ifdef _WIN64
        return RtlLookupFunctionEntry(program_counter, image_base, nullptr);
#else
        NOTREACHED();
        return nullptr;
#endif
    }

    void Win32UnwindFunctions::VirtualUnwind(DWORD64 image_base,
        DWORD64 program_counter,
        PRUNTIME_FUNCTION runtime_function,
        CONTEXT* context)
    {
#ifdef _WIN64
        void* handler_data;
        ULONG64 establisher_frame;
        KNONVOLATILE_CONTEXT_POINTERS nvcontext = {};
        RtlVirtualUnwind(UNW_FLAG_NHANDLER, image_base, program_counter,
            runtime_function, context, &handler_data,
            &establisher_frame, &nvcontext);
#else
        NOTREACHED();
#endif
    }

    ScopedModuleHandle Win32UnwindFunctions::GetModuleForProgramCounter(
        DWORD64 program_counter)
    {
        HMODULE module_handle = nullptr;
        // GetModuleHandleEx() increments the module reference count, which is then
        // managed and ultimately decremented by ScopedModuleHandle.
        //   if (!::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        //                            reinterpret_cast<LPCTSTR>(program_counter),
        //                            &module_handle)) {
        //     const DWORD error = ::GetLastError();
        //     DCHECK_EQ(ERROR_MOD_NOT_FOUND, static_cast<int>(error));
        //   }
        DebugBreak();
        return ScopedModuleHandle(module_handle);
    }

    // LeafUnwindBlacklist --------------------------------------------------------

    // Records modules that are known to have functions that violate the Microsoft
    // x64 calling convention and would be dangerous to manually unwind if
    // encountered as the last frame on the call stack. Functions like these have
    // been observed in injected third party modules that either do not provide
    // function unwind information, or do not provide the required function prologue
    // and epilogue. The former case was observed in several AV products and the
    // latter in a WndProc function associated with Actual Window
    // Manager/aimemb64.dll. See https://crbug.com/476422.
    class LeafUnwindBlacklist {
    public:
        static LeafUnwindBlacklist* GetInstance();

        // Returns true if |module| has been blacklisted.
        bool IsBlacklisted(const void* module) const;

        // Records |module| for blacklisting.
        void BlacklistModule(const void* module);

    private:
        friend struct DefaultSingletonTraits<LeafUnwindBlacklist>;

        LeafUnwindBlacklist();
        ~LeafUnwindBlacklist();

        // The set of modules known to have functions that violate the Microsoft x64
        // calling convention.
        base::hash_set<const void*> blacklisted_modules_;

        DISALLOW_COPY_AND_ASSIGN(LeafUnwindBlacklist);
    };

    // static
    LeafUnwindBlacklist* LeafUnwindBlacklist::GetInstance()
    {
        // Leaky for performance reasons.
        return Singleton<LeafUnwindBlacklist,
            LeakySingletonTraits<LeafUnwindBlacklist>>::get();
    }

    bool LeafUnwindBlacklist::IsBlacklisted(const void* module) const
    {
        return ContainsKey(blacklisted_modules_, module);
    }

    void LeafUnwindBlacklist::BlacklistModule(const void* module)
    {
        CHECK(module);
        blacklisted_modules_.insert(module);
    }

    LeafUnwindBlacklist::LeafUnwindBlacklist() { }
    LeafUnwindBlacklist::~LeafUnwindBlacklist() { }

} // namespace

// Win32StackFrameUnwinder ----------------------------------------------------

// hipis0e011b8.dll from McAfee Host Intrusion Prevention has been observed to
// provide a pointer to a bogus RUNTIME_FUNCTION structure. This function checks
// that the values in the structure look plausible.
bool SanityCheckRuntimeFunction(PRUNTIME_FUNCTION runtime_function,
    ULONG64 image_base,
    DWORD64 program_counter)
{
    const DWORD64 program_counter_offset = program_counter - image_base;
    return (runtime_function->BeginAddress <= runtime_function->EndAddress && program_counter_offset >= runtime_function->BeginAddress && program_counter_offset <= runtime_function->EndAddress);
}

Win32StackFrameUnwinder::UnwindFunctions::~UnwindFunctions() { }
Win32StackFrameUnwinder::UnwindFunctions::UnwindFunctions() { }

Win32StackFrameUnwinder::Win32StackFrameUnwinder()
    : Win32StackFrameUnwinder(make_scoped_ptr(new Win32UnwindFunctions))
{
}

Win32StackFrameUnwinder::~Win32StackFrameUnwinder() { }

bool Win32StackFrameUnwinder::TryUnwind(CONTEXT* context,
    ScopedModuleHandle* module)
{
#ifdef _WIN64
    CHECK(!at_top_frame_ || unwind_info_present_for_all_frames_);

    ScopedModuleHandle frame_module = unwind_functions_->GetModuleForProgramCounter(context->Rip);
    // The module may have been unloaded since we recorded the stack. Note that if
    // this check detects module as valid, it still could be a different module at
    // the same instruction pointer address (i.e. if the module was unloaded and a
    // different module loaded in overlapping memory). This should occur extremely
    // rarely.
    if (!frame_module.IsValid())
        return false;

    ULONG64 image_base;
    // Try to look up unwind metadata for the current function.
    PRUNTIME_FUNCTION runtime_function = unwind_functions_->LookupFunctionEntry(context->Rip, &image_base);

    if (runtime_function) {
        if (!SanityCheckRuntimeFunction(runtime_function, image_base, context->Rip))
            return false;

        unwind_functions_->VirtualUnwind(image_base, context->Rip, runtime_function,
            context);
        at_top_frame_ = false;
    } else {
        // RtlLookupFunctionEntry didn't find unwind information. This could mean
        // the code at the instruction pointer is in:
        //
        // 1. a true leaf function (i.e. a function that neither calls a function,
        //    nor allocates any stack space itself) in which case the return
        //    address is at RSP, or
        //
        // 2. a function that doesn't adhere to the Microsoft x64 calling
        //    convention, either by not providing the required unwind information,
        //    or by not having the prologue or epilogue required for unwinding;
        //    this case has been observed in crash data in injected third party
        //    DLLs.
        //
        // In valid code, case 1 can only occur (by definition) as the last frame
        // on the stack. This happens in about 5% of observed stacks and can
        // easily be unwound by popping RSP and using it as the next frame's
        // instruction pointer.
        //
        // Case 2 can occur anywhere on the stack, and attempting to unwind the
        // stack will result in treating whatever value happens to be on the stack
        // at RSP as the next frame's instruction pointer. This is certainly wrong
        // and very likely to lead to crashing by deferencing invalid pointers in
        // the next RtlVirtualUnwind call.
        //
        // If we see case 2 at a location not the last frame, and all the previous
        // frame had valid unwind information, then this is definitely bad code.
        // We blacklist the module as untrustable for unwinding if we encounter a
        // function in it that doesn't have unwind information.

        if (at_top_frame_) {
            at_top_frame_ = false;

            // We are at the end of the stack. It's very likely that we're in case 1
            // since the vast majority of code adheres to the Microsoft x64 calling
            // convention. But there's a small chance we might be unlucky and be in
            // case 2. If this module is known to have bad code according to the
            // leaf unwind blacklist, stop here, otherwise manually unwind.
            if (LeafUnwindBlacklist::GetInstance()->IsBlacklisted(
                    reinterpret_cast<const void*>(image_base))) {
                return false;
            }

            context->Rip = context->Rsp;
            context->Rsp += 8;
            unwind_info_present_for_all_frames_ = false;
        } else {
            // We're not at the end of the stack. This frame is untrustworthy and we
            // can't safely unwind from here.
            if (!image_base) {
                // A null image_base means that the the last unwind produced an invalid
                // instruction pointer. This has been observed where unwind information
                // was present for a function but was inconsistent with the actual
                // function code, in particular in BoringSSL. See
                // https://crbug.com/542919.
            } else if (unwind_info_present_for_all_frames_) {
                // Unwind information was present for all previous frames, so we can
                // be confident this is case 2. Record the module to be blacklisted.
                LeafUnwindBlacklist::GetInstance()->BlacklistModule(
                    reinterpret_cast<const void*>(image_base));
            } else {
                // We started off on a function without unwind information. It's very
                // likely that all frames up to this point have been good, and this
                // frame is case 2. But it's possible that the initial frame was case
                // 2 but hadn't been blacklisted yet, and we've started to go off into
                // the weeds. Since we can't be sure, just bail out without
                // blacklisting the module; chances are we'll later encounter the same
                // function on a stack with full unwind information.
            }
            return false;
        }
    }

    module->Set(frame_module.Take());
    return true;
#else
    NOTREACHED();
    return false;
#endif
}

Win32StackFrameUnwinder::Win32StackFrameUnwinder(
    scoped_ptr<UnwindFunctions> unwind_functions)
    : at_top_frame_(true)
    , unwind_info_present_for_all_frames_(true)
    , unwind_functions_(unwind_functions.Pass())
{
}

} // namespace base
