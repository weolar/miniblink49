// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <objbase.h>
#include <windows.h>
//#include <winternl.h>

#include <cstdlib>
#include <map>
#include <utility>
#include <vector>

#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/profiler/native_stack_sampler.h"
#include "base/profiler/win32_stack_frame_unwinder.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "base/win/pe_image.h"
#include "base/win/scoped_handle.h"

typedef LONG NTSTATUS;

namespace base {

// Stack recording functions --------------------------------------------------

namespace {

    // The thread environment block internal type.
    struct TEB {
        NT_TIB Tib;
        // Rest of struct is ignored.
    };

    // Returns the thread environment block pointer for |thread_handle|.
    const TEB* GetThreadEnvironmentBlock(HANDLE thread_handle)
    {
        // Define the internal types we need to invoke NtQueryInformationThread.
        enum THREAD_INFORMATION_CLASS { ThreadBasicInformation };

        struct CLIENT_ID {
            HANDLE UniqueProcess;
            HANDLE UniqueThread;
        };

        struct THREAD_BASIC_INFORMATION {
            NTSTATUS ExitStatus;
            TEB* Teb;
            CLIENT_ID ClientId;
            KAFFINITY AffinityMask;
            LONG Priority;
            LONG BasePriority;
        };

        using NtQueryInformationThreadFunction = NTSTATUS(WINAPI*)(HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG,
            PULONG);

        const NtQueryInformationThreadFunction nt_query_information_thread = reinterpret_cast<NtQueryInformationThreadFunction>(
            ::GetProcAddress(::GetModuleHandle(L"ntdll.dll"),
                "NtQueryInformationThread"));
        if (!nt_query_information_thread)
            return nullptr;

        THREAD_BASIC_INFORMATION basic_info = { 0 };
        NTSTATUS status = nt_query_information_thread(thread_handle, ThreadBasicInformation,
            &basic_info, sizeof(THREAD_BASIC_INFORMATION),
            nullptr);
        if (status != 0)
            return nullptr;

        return basic_info.Teb;
    }

#if defined(_WIN64)
    // If the value at |pointer| points to the original stack, rewrite it to point
    // to the corresponding location in the copied stack.
    void RewritePointerIfInOriginalStack(uintptr_t top, uintptr_t bottom,
        void* stack_copy, const void** pointer)
    {
        const uintptr_t value = reinterpret_cast<uintptr_t>(*pointer);
        if (value >= bottom && value < top) {
            *pointer = reinterpret_cast<const void*>(
                static_cast<unsigned char*>(stack_copy) + (value - bottom));
        }
    }
#endif

    // Rewrites possible pointers to locations within the stack to point to the
    // corresponding locations in the copy, and rewrites the non-volatile registers
    // in |context| likewise. This is necessary to handle stack frames with dynamic
    // stack allocation, where a pointer to the beginning of the dynamic allocation
    // area is stored on the stack and/or in a non-volatile register.
    //
    // Eager rewriting of anything that looks like a pointer to the stack, as done
    // in this function, does not adversely affect the stack unwinding. The only
    // other values on the stack the unwinding depends on are return addresses,
    // which should not point within the stack memory. The rewriting is guaranteed
    // to catch all pointers because the stacks are guaranteed by the ABI to be
    // sizeof(void*) aligned.
    //
    // Note: this function must not access memory in the original stack as it may
    // have been changed or deallocated by this point. This is why |top| and
    // |bottom| are passed as uintptr_t.
    void RewritePointersToStackMemory(uintptr_t top, uintptr_t bottom,
        CONTEXT* context, void* stack_copy)
    {
#if defined(_WIN64)
        DWORD64 CONTEXT::*const nonvolatile_registers[] = {
            &CONTEXT::R12,
            &CONTEXT::R13,
            &CONTEXT::R14,
            &CONTEXT::R15,
            &CONTEXT::Rdi,
            &CONTEXT::Rsi,
            &CONTEXT::Rbx,
            &CONTEXT::Rbp,
            &CONTEXT::Rsp
        };

        // Rewrite pointers in the context.
        for (size_t i = 0; i < arraysize(nonvolatile_registers); ++i) {
            DWORD64* const reg = &(context->*nonvolatile_registers[i]);
            RewritePointerIfInOriginalStack(top, bottom, stack_copy,
                reinterpret_cast<const void**>(reg));
        }

        // Rewrite pointers on the stack.
        const void** start = reinterpret_cast<const void**>(stack_copy);
        const void** end = reinterpret_cast<const void**>(
            reinterpret_cast<char*>(stack_copy) + (top - bottom));
        for (const void** loc = start; loc < end; ++loc)
            RewritePointerIfInOriginalStack(top, bottom, stack_copy, loc);
#endif
    }

    // Walks the stack represented by |context| from the current frame downwards,
    // recording the instruction pointers for each frame in |instruction_pointers|.
    int RecordStack(CONTEXT* context, int max_stack_size,
        const void* instruction_pointers[],
        ScopedModuleHandle modules[])
    {
#ifdef _WIN64
        Win32StackFrameUnwinder frame_unwinder;
        int i = 0;
        for (; (i < max_stack_size) && context->Rip; ++i) {
            instruction_pointers[i] = reinterpret_cast<const void*>(context->Rip);
            if (!frame_unwinder.TryUnwind(context, &modules[i]))
                return i + 1;
        }
        return i;
#else
        return 0;
#endif
    }

    // Gets the unique build ID for a module. Windows build IDs are created by a
    // concatenation of a GUID and AGE fields found in the headers of a module. The
    // GUID is stored in the first 16 bytes and the AGE is stored in the last 4
    // bytes. Returns the empty string if the function fails to get the build ID.
    //
    // Example:
    // dumpbin chrome.exe /headers | find "Format:"
    //   ... Format: RSDS, {16B2A428-1DED-442E-9A36-FCE8CBD29726}, 10, ...
    //
    // The resulting buildID string of this instance of chrome.exe is
    // "16B2A4281DED442E9A36FCE8CBD2972610".
    //
    // Note that the AGE field is encoded in decimal, not hex.
    std::string GetBuildIDForModule(HMODULE module_handle)
    {
        GUID guid;
        DWORD age;
        win::PEImage(module_handle).GetDebugId(&guid, &age);
        const int kGUIDSize = 39;
        std::wstring build_id;
        int result = ::StringFromGUID2(guid, WriteInto(&build_id, kGUIDSize), kGUIDSize);
        if (result != kGUIDSize)
            return std::string();
        RemoveChars(build_id, L"{}-", &build_id);
        build_id += StringPrintf(L"%d", age);
        return WideToUTF8(build_id);
    }

    // ScopedDisablePriorityBoost -------------------------------------------------

    // Disables priority boost on a thread for the lifetime of the object.
    class ScopedDisablePriorityBoost {
    public:
        ScopedDisablePriorityBoost(HANDLE thread_handle);
        ~ScopedDisablePriorityBoost();

    private:
        HANDLE thread_handle_;
        BOOL got_previous_boost_state_;
        BOOL boost_state_was_disabled_;

        DISALLOW_COPY_AND_ASSIGN(ScopedDisablePriorityBoost);
    };

    ScopedDisablePriorityBoost::ScopedDisablePriorityBoost(HANDLE thread_handle)
        : thread_handle_(thread_handle)
        , got_previous_boost_state_(false)
        , boost_state_was_disabled_(false)
    {
        got_previous_boost_state_ = ::GetThreadPriorityBoost(thread_handle_, &boost_state_was_disabled_);
        if (got_previous_boost_state_) {
            // Confusingly, TRUE disables priority boost.
            ::SetThreadPriorityBoost(thread_handle_, TRUE);
        }
    }

    ScopedDisablePriorityBoost::~ScopedDisablePriorityBoost()
    {
        if (got_previous_boost_state_)
            ::SetThreadPriorityBoost(thread_handle_, boost_state_was_disabled_);
    }

    // ScopedSuspendThread --------------------------------------------------------

    // Suspends a thread for the lifetime of the object.
    class ScopedSuspendThread {
    public:
        ScopedSuspendThread(HANDLE thread_handle);
        ~ScopedSuspendThread();

        bool was_successful() const { return was_successful_; }

    private:
        HANDLE thread_handle_;
        bool was_successful_;

        DISALLOW_COPY_AND_ASSIGN(ScopedSuspendThread);
    };

    ScopedSuspendThread::ScopedSuspendThread(HANDLE thread_handle)
        : thread_handle_(thread_handle)
        , was_successful_(::SuspendThread(thread_handle) != -1)
    {
    }

    ScopedSuspendThread::~ScopedSuspendThread()
    {
        if (!was_successful_)
            return;

        // Disable the priority boost that the thread would otherwise receive on
        // resume. We do this to avoid artificially altering the dynamics of the
        // executing application any more than we already are by suspending and
        // resuming the thread.
        //
        // Note that this can racily disable a priority boost that otherwise would
        // have been given to the thread, if the thread is waiting on other wait
        // conditions at the time of SuspendThread and those conditions are satisfied
        // before priority boost is reenabled. The measured length of this window is
        // ~100us, so this should occur fairly rarely.
        ScopedDisablePriorityBoost disable_priority_boost(thread_handle_);
        bool resume_thread_succeeded = ::ResumeThread(thread_handle_) != -1;
        CHECK(resume_thread_succeeded) << "ResumeThread failed: " << GetLastError();
    }

    // Suspends the thread with |thread_handle|, copies its stack and resumes the
    // thread, then records the stack into |instruction_pointers|. Returns the size
    // of the stack.
    //
    // IMPORTANT NOTE: No allocations from the default heap may occur in the
    // ScopedSuspendThread scope, including indirectly via use of DCHECK/CHECK or
    // other logging statements. Otherwise this code can deadlock on heap locks in
    // the default heap acquired by the target thread before it was suspended. This
    // is why we pass instruction pointers as preallocated arrays.
    int SuspendThreadAndRecordStack(HANDLE thread_handle,
        const void* base_address,
        void* stack_copy_buffer,
        size_t stack_copy_buffer_size,
        int max_stack_size,
        const void* instruction_pointers[],
        ScopedModuleHandle modules[],
        NativeStackSamplerTestDelegate* test_delegate)
    {
        CONTEXT thread_context = { 0 };
        thread_context.ContextFlags = CONTEXT_FULL;
        // The stack bounds are saved to uintptr_ts for use outside
        // ScopedSuspendThread, as the thread's memory is not safe to dereference
        // beyond that point.
        const uintptr_t top = reinterpret_cast<uintptr_t>(base_address);
        uintptr_t bottom = 0u;

        {
            ScopedSuspendThread suspend_thread(thread_handle);

            if (!suspend_thread.was_successful())
                return 0;

            if (!::GetThreadContext(thread_handle, &thread_context))
                return 0;
#if defined(_WIN64)
            bottom = thread_context.Rsp;
#else
            bottom = thread_context.Esp;
#endif

            if ((top - bottom) > stack_copy_buffer_size)
                return 0;

            memcpy(stack_copy_buffer, reinterpret_cast<const void*>(bottom),
                top - bottom);
        }

        if (test_delegate)
            test_delegate->OnPreStackWalk();

        RewritePointersToStackMemory(top, bottom, &thread_context, stack_copy_buffer);

        return RecordStack(&thread_context, max_stack_size, instruction_pointers,
            modules);
    }

    // NativeStackSamplerWin ------------------------------------------------------

    class NativeStackSamplerWin : public NativeStackSampler {
    public:
        NativeStackSamplerWin(win::ScopedHandle thread_handle,
            NativeStackSamplerTestDelegate* test_delegate);
        ~NativeStackSamplerWin() override;

        // StackSamplingProfiler::NativeStackSampler:
        void ProfileRecordingStarting(
            std::vector<StackSamplingProfiler::Module>* modules) override;
        void RecordStackSample(StackSamplingProfiler::Sample* sample) override;
        void ProfileRecordingStopped() override;

    private:
        enum {
            // Intended to hold the largest stack used by Chrome. The default Win32
            // reserved stack size is 1 MB and Chrome Windows threads currently always
            // use the default, but this allows for expansion if it occurs. The size
            // beyond the actual stack size consists of unallocated virtual memory pages
            // so carries little cost (just a bit of wated address space).
            kStackCopyBufferSize = 2 * 1024 * 1024
        };

        // Attempts to query the module filename, base address, and id for
        // |module_handle|, and store them in |module|. Returns true if it succeeded.
        static bool GetModuleForHandle(HMODULE module_handle,
            StackSamplingProfiler::Module* module);

        // Gets the index for the Module corresponding to |module_handle| in
        // |modules|, adding it if it's not already present. Returns
        // StackSamplingProfiler::Frame::kUnknownModuleIndex if no Module can be
        // determined for |module|.
        size_t GetModuleIndex(HMODULE module_handle,
            std::vector<StackSamplingProfiler::Module>* modules);

        // Copies the stack information represented by |instruction_pointers| into
        // |sample| and |modules|.
        void CopyToSample(const void* const instruction_pointers[],
            const ScopedModuleHandle module_handles[],
            int stack_depth,
            StackSamplingProfiler::Sample* sample,
            std::vector<StackSamplingProfiler::Module>* modules);

        win::ScopedHandle thread_handle_;

        NativeStackSamplerTestDelegate* const test_delegate_;

        // The stack base address corresponding to |thread_handle_|.
        const void* const thread_stack_base_address_;

        // Buffer to use for copies of the stack. We use the same buffer for all the
        // samples to avoid the overhead of multiple allocations and frees.
        const scoped_ptr<unsigned char[]> stack_copy_buffer_;

        // Weak. Points to the modules associated with the profile being recorded
        // between ProfileRecordingStarting() and ProfileRecordingStopped().
        std::vector<StackSamplingProfiler::Module>* current_modules_;

        // Maps a module handle to the corresponding Module's index within
        // current_modules_.
        std::map<HMODULE, size_t> profile_module_index_;

        DISALLOW_COPY_AND_ASSIGN(NativeStackSamplerWin);
    };

    NativeStackSamplerWin::NativeStackSamplerWin(
        win::ScopedHandle thread_handle,
        NativeStackSamplerTestDelegate* test_delegate)
        : thread_handle_(thread_handle.Take())
        , test_delegate_(test_delegate)
        , thread_stack_base_address_(
              GetThreadEnvironmentBlock(thread_handle_.Get())->Tib.StackBase)
        , stack_copy_buffer_(new unsigned char[kStackCopyBufferSize])
    {
    }

    NativeStackSamplerWin::~NativeStackSamplerWin()
    {
    }

    void NativeStackSamplerWin::ProfileRecordingStarting(
        std::vector<StackSamplingProfiler::Module>* modules)
    {
        current_modules_ = modules;
        profile_module_index_.clear();
    }

    void NativeStackSamplerWin::RecordStackSample(
        StackSamplingProfiler::Sample* sample)
    {
        DCHECK(current_modules_);

        if (!stack_copy_buffer_)
            return;

        const int max_stack_size = 64;
        const void* instruction_pointers[max_stack_size] = { 0 };
        ScopedModuleHandle modules[max_stack_size];

        int stack_depth = SuspendThreadAndRecordStack(thread_handle_.Get(),
            thread_stack_base_address_,
            stack_copy_buffer_.get(),
            kStackCopyBufferSize,
            max_stack_size,
            instruction_pointers,
            modules,
            test_delegate_);
        CopyToSample(instruction_pointers, modules, stack_depth, sample,
            current_modules_);
    }

    void NativeStackSamplerWin::ProfileRecordingStopped()
    {
        current_modules_ = nullptr;
    }

    // static
    bool NativeStackSamplerWin::GetModuleForHandle(
        HMODULE module_handle,
        StackSamplingProfiler::Module* module)
    {
        wchar_t module_name[MAX_PATH];
        DWORD result_length = GetModuleFileName(module_handle, module_name, arraysize(module_name));
        if (result_length == 0)
            return false;

        module->filename = base::FilePath(module_name);

        module->base_address = reinterpret_cast<uintptr_t>(module_handle);

        module->id = GetBuildIDForModule(module_handle);
        if (module->id.empty())
            return false;

        return true;
    }

    size_t NativeStackSamplerWin::GetModuleIndex(
        HMODULE module_handle,
        std::vector<StackSamplingProfiler::Module>* modules)
    {
        if (!module_handle)
            return StackSamplingProfiler::Frame::kUnknownModuleIndex;

        auto loc = profile_module_index_.find(module_handle);
        if (loc == profile_module_index_.end()) {
            StackSamplingProfiler::Module module;
            if (!GetModuleForHandle(module_handle, &module))
                return StackSamplingProfiler::Frame::kUnknownModuleIndex;
            modules->push_back(module);
            loc = profile_module_index_.insert(std::make_pair(
                                                   module_handle, modules->size() - 1))
                      .first;
        }

        return loc->second;
    }

    void NativeStackSamplerWin::CopyToSample(
        const void* const instruction_pointers[],
        const ScopedModuleHandle module_handles[],
        int stack_depth,
        StackSamplingProfiler::Sample* sample,
        std::vector<StackSamplingProfiler::Module>* modules)
    {
        sample->clear();
        sample->reserve(stack_depth);

        for (int i = 0; i < stack_depth; ++i) {
            sample->push_back(StackSamplingProfiler::Frame(
                reinterpret_cast<uintptr_t>(instruction_pointers[i]),
                GetModuleIndex(module_handles[i].Get(), modules)));
        }
    }

} // namespace

scoped_ptr<NativeStackSampler> NativeStackSampler::Create(
    PlatformThreadId thread_id,
    NativeStackSamplerTestDelegate* test_delegate)
{
#if _WIN64
    // Get the thread's handle.
    HANDLE thread_handle = ::OpenThread(
        THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION,
        FALSE,
        thread_id);

    if (thread_handle) {
        return scoped_ptr<NativeStackSampler>(new NativeStackSamplerWin(
            win::ScopedHandle(thread_handle),
            test_delegate));
    }
#endif
    return scoped_ptr<NativeStackSampler>();
}

} // namespace base
