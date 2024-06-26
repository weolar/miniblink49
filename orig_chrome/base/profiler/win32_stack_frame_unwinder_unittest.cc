// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/profiler/win32_stack_frame_unwinder.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {

namespace {

    class TestUnwindFunctions : public Win32StackFrameUnwinder::UnwindFunctions {
    public:
        TestUnwindFunctions();

        PRUNTIME_FUNCTION LookupFunctionEntry(DWORD64 program_counter,
            PDWORD64 image_base) override;
        void VirtualUnwind(DWORD64 image_base,
            DWORD64 program_counter,
            PRUNTIME_FUNCTION runtime_function,
            CONTEXT* context) override;
        ScopedModuleHandle GetModuleForProgramCounter(
            DWORD64 program_counter) override;

        // Instructs GetModuleForProgramCounter to return null on the next call.
        void SetUnloadedModule();

        // These functions set whether the next frame will have a RUNTIME_FUNCTION,
        // and allow specification of a custom image_base.
        void SetHasRuntimeFunction(CONTEXT* context);
        void SetHasRuntimeFunction(DWORD64 image_base, CONTEXT* context);
        void SetHasRuntimeFunction(DWORD64 image_base,
            const RUNTIME_FUNCTION& runtime_function,
            DWORD program_counter_offset,
            CONTEXT* context);
        void SetNoRuntimeFunction(CONTEXT* context);
        void SetNoRuntimeFunction(DWORD64 image_base, CONTEXT* context);

    private:
        enum RuntimeFunctionState { NO_RUNTIME_FUNCTION,
            HAS_RUNTIME_FUNCTION };

        enum { kImageBaseIncrement = 1 << 20 };

        // Sets whether the next frame should have a RUNTIME_FUNCTION, and allows
        // specification of a custom image_base.
        void SetNextFrameState(RuntimeFunctionState runtime_function_state,
            DWORD64 image_base,
            CONTEXT* context);

        static RUNTIME_FUNCTION* const kInvalidRuntimeFunction;

        bool module_is_loaded_;
        DWORD64 expected_program_counter_;
        DWORD64 custom_image_base_;
        DWORD64 next_image_base_;
        DWORD64 expected_image_base_;
        RUNTIME_FUNCTION* next_runtime_function_;
        std::vector<RUNTIME_FUNCTION> runtime_functions_;

        DISALLOW_COPY_AND_ASSIGN(TestUnwindFunctions);
    };

    RUNTIME_FUNCTION* const TestUnwindFunctions::kInvalidRuntimeFunction = reinterpret_cast<RUNTIME_FUNCTION*>(static_cast<uintptr_t>(-1));

    TestUnwindFunctions::TestUnwindFunctions()
        : module_is_loaded_(true)
        , expected_program_counter_(0)
        , custom_image_base_(0)
        , next_image_base_(kImageBaseIncrement)
        , expected_image_base_(0)
        , next_runtime_function_(kInvalidRuntimeFunction)
    {
    }

    PRUNTIME_FUNCTION TestUnwindFunctions::LookupFunctionEntry(
        DWORD64 program_counter,
        PDWORD64 image_base)
    {
        EXPECT_EQ(expected_program_counter_, program_counter);
        if (custom_image_base_) {
            *image_base = expected_image_base_ = custom_image_base_;
            custom_image_base_ = 0;
        } else {
            *image_base = expected_image_base_ = next_image_base_;
            next_image_base_ += kImageBaseIncrement;
        }
        RUNTIME_FUNCTION* return_value = next_runtime_function_;
        next_runtime_function_ = kInvalidRuntimeFunction;
        return return_value;
    }

    void TestUnwindFunctions::VirtualUnwind(DWORD64 image_base,
        DWORD64 program_counter,
        PRUNTIME_FUNCTION runtime_function,
        CONTEXT* context)
    {
        ASSERT_NE(kInvalidRuntimeFunction, runtime_function)
            << "expected call to SetHasRuntimeFunction() or SetNoRuntimeFunction() "
            << "before invoking TryUnwind()";
        EXPECT_EQ(expected_image_base_, image_base);
        expected_image_base_ = 0;
        EXPECT_EQ(expected_program_counter_, program_counter);
        expected_program_counter_ = 0;
        // This function should only be called when LookupFunctionEntry returns
        // a RUNTIME_FUNCTION.
        EXPECT_EQ(&runtime_functions_.back(), runtime_function);
    }

    ScopedModuleHandle TestUnwindFunctions::GetModuleForProgramCounter(
        DWORD64 program_counter)
    {
        bool return_non_null_value = module_is_loaded_;
        module_is_loaded_ = true;
        return ScopedModuleHandle(return_non_null_value ? ModuleHandleTraits::kNonNullModuleForTesting : nullptr);
    }

    void TestUnwindFunctions::SetUnloadedModule()
    {
        module_is_loaded_ = false;
    }

    void TestUnwindFunctions::SetHasRuntimeFunction(CONTEXT* context)
    {
        SetNextFrameState(HAS_RUNTIME_FUNCTION, 0, context);
    }

    void TestUnwindFunctions::SetHasRuntimeFunction(DWORD64 image_base,
        CONTEXT* context)
    {
        SetNextFrameState(HAS_RUNTIME_FUNCTION, image_base, context);
    }

    void TestUnwindFunctions::SetHasRuntimeFunction(
        DWORD64 image_base,
        const RUNTIME_FUNCTION& runtime_function,
        DWORD program_counter_offset,
        CONTEXT* context)
    {
        custom_image_base_ = image_base;
        runtime_functions_.push_back(runtime_function);
        next_runtime_function_ = &runtime_functions_.back();
        expected_program_counter_ = context->Rip = image_base + program_counter_offset;
    }

    void TestUnwindFunctions::SetNoRuntimeFunction(CONTEXT* context)
    {
        SetNextFrameState(NO_RUNTIME_FUNCTION, 0, context);
    }

    void TestUnwindFunctions::SetNoRuntimeFunction(DWORD64 image_base,
        CONTEXT* context)
    {
        SetNextFrameState(NO_RUNTIME_FUNCTION, image_base, context);
    }

    void TestUnwindFunctions::SetNextFrameState(
        RuntimeFunctionState runtime_function_state,
        DWORD64 image_base,
        CONTEXT* context)
    {
        if (image_base)
            custom_image_base_ = image_base;

        if (runtime_function_state == HAS_RUNTIME_FUNCTION) {
            RUNTIME_FUNCTION runtime_function = {};
            runtime_function.BeginAddress = 16;
            runtime_function.EndAddress = runtime_function.BeginAddress + 256;
            runtime_functions_.push_back(runtime_function);
            next_runtime_function_ = &runtime_functions_.back();

            DWORD64 image_base = custom_image_base_ ? custom_image_base_ : next_image_base_;
            expected_program_counter_ = context->Rip = image_base + runtime_function.BeginAddress + 8;
        } else {
            expected_program_counter_ = context->Rip = 100;
            next_runtime_function_ = nullptr;
        }
    }

} // namespace

class Win32StackFrameUnwinderTest : public testing::Test {
protected:
    Win32StackFrameUnwinderTest() { }

    // This exists so that Win32StackFrameUnwinder's constructor can be private
    // with a single friend declaration of this test fixture.
    scoped_ptr<Win32StackFrameUnwinder> CreateUnwinder();

    // Weak pointer to the unwind functions used by last created unwinder.
    TestUnwindFunctions* unwind_functions_;

private:
    DISALLOW_COPY_AND_ASSIGN(Win32StackFrameUnwinderTest);
};

scoped_ptr<Win32StackFrameUnwinder>
Win32StackFrameUnwinderTest::CreateUnwinder()
{
    scoped_ptr<TestUnwindFunctions> unwind_functions(new TestUnwindFunctions);
    unwind_functions_ = unwind_functions.get();
    return make_scoped_ptr(new Win32StackFrameUnwinder(unwind_functions.Pass()));
}

// Checks the case where all frames have unwind information.
TEST_F(Win32StackFrameUnwinderTest, FramesWithUnwindInfo)
{
    scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
    CONTEXT context = { 0 };
    ScopedModuleHandle module;

    unwind_functions_->SetHasRuntimeFunction(&context);
    EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
    EXPECT_TRUE(module.IsValid());

    unwind_functions_->SetHasRuntimeFunction(&context);
    module.Set(nullptr);
    EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
    EXPECT_TRUE(module.IsValid());

    unwind_functions_->SetHasRuntimeFunction(&context);
    module.Set(nullptr);
    EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
    EXPECT_TRUE(module.IsValid());
}

// Checks that an instruction pointer in an unloaded module fails to unwind.
TEST_F(Win32StackFrameUnwinderTest, UnloadedModule)
{
    scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
    CONTEXT context = { 0 };
    ScopedModuleHandle module;

    unwind_functions_->SetUnloadedModule();
    EXPECT_FALSE(unwinder->TryUnwind(&context, &module));
}

// Checks that the CONTEXT's stack pointer gets popped when the top frame has no
// unwind information.
TEST_F(Win32StackFrameUnwinderTest, FrameAtTopWithoutUnwindInfo)
{
    scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
    CONTEXT context = { 0 };
    ScopedModuleHandle module;
    const DWORD64 original_rsp = 128;
    context.Rsp = original_rsp;

    unwind_functions_->SetNoRuntimeFunction(&context);
    EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
    EXPECT_EQ(original_rsp, context.Rip);
    EXPECT_EQ(original_rsp + 8, context.Rsp);
    EXPECT_TRUE(module.IsValid());

    unwind_functions_->SetHasRuntimeFunction(&context);
    module.Set(nullptr);
    EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
    EXPECT_TRUE(module.IsValid());

    unwind_functions_->SetHasRuntimeFunction(&context);
    module.Set(nullptr);
    EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
    EXPECT_TRUE(module.IsValid());
}

// Checks that a frame below the top of the stack with missing unwind info
// results in blacklisting the module.
TEST_F(Win32StackFrameUnwinderTest, BlacklistedModule)
{
    const DWORD64 image_base_for_module_with_bad_function = 1024;
    {
        // First stack, with a bad function below the top of the stack.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        unwind_functions_->SetHasRuntimeFunction(&context);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());

        unwind_functions_->SetNoRuntimeFunction(
            image_base_for_module_with_bad_function,
            &context);
        EXPECT_FALSE(unwinder->TryUnwind(&context, &module));
    }

    {
        // Second stack; check that a function at the top of the stack without
        // unwind info from the previously-seen module is blacklisted.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        unwind_functions_->SetNoRuntimeFunction(
            image_base_for_module_with_bad_function,
            &context);
        EXPECT_FALSE(unwinder->TryUnwind(&context, &module));
    }

    {
        // Third stack; check that a function at the top of the stack *with* unwind
        // info from the previously-seen module is not blacklisted. Then check that
        // functions below the top of the stack with unwind info are not
        // blacklisted, regardless of whether they are in the previously-seen
        // module.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        unwind_functions_->SetHasRuntimeFunction(
            image_base_for_module_with_bad_function,
            &context);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());

        unwind_functions_->SetHasRuntimeFunction(&context);
        module.Set(nullptr);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());

        unwind_functions_->SetHasRuntimeFunction(
            image_base_for_module_with_bad_function,
            &context);
        module.Set(nullptr);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());
    }

    {
        // Fourth stack; check that a function at the top of the stack without
        // unwind info and not from the previously-seen module is not
        // blacklisted. Then check that functions below the top of the stack with
        // unwind info are not blacklisted, regardless of whether they are in the
        // previously-seen module.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        unwind_functions_->SetNoRuntimeFunction(&context);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());

        unwind_functions_->SetHasRuntimeFunction(&context);
        module.Set(nullptr);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());

        unwind_functions_->SetHasRuntimeFunction(
            image_base_for_module_with_bad_function,
            &context);
        module.Set(nullptr);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());
    }
}

// Checks that a frame below the top of the stack with missing unwind info does
// not result in blacklisting the module if the first frame also was missing
// unwind info. This ensures we don't blacklist an innocent module because the
// first frame was bad but we didn't know it at the time.
TEST_F(Win32StackFrameUnwinderTest, ModuleFromQuestionableFrameNotBlacklisted)
{
    const DWORD64 image_base_for_questionable_module = 2048;
    {
        // First stack, with both the first and second frames missing unwind info.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        unwind_functions_->SetNoRuntimeFunction(&context);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());

        unwind_functions_->SetNoRuntimeFunction(image_base_for_questionable_module,
            &context);
        EXPECT_FALSE(unwinder->TryUnwind(&context, &module));
    }

    {
        // Second stack; check that the questionable module was not blacklisted.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        unwind_functions_->SetNoRuntimeFunction(image_base_for_questionable_module,
            &context);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());
    }
}

// Checks that frames with RUNTIME_FUNCTION structures with nonsensical values
// are not unwound.
TEST_F(Win32StackFrameUnwinderTest, RuntimeFunctionSanityCheck)
{
    const DWORD64 image_base_for_sanity_check = 3072;
    {
        // Test the expected case: end address greater than begin address and
        // instruction pointer between the two.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        RUNTIME_FUNCTION runtime_function = { 0 };
        runtime_function.BeginAddress = 128;
        runtime_function.EndAddress = 512;
        unwind_functions_->SetHasRuntimeFunction(image_base_for_sanity_check,
            runtime_function, 256,
            &context);
        EXPECT_TRUE(unwinder->TryUnwind(&context, &module));
        EXPECT_TRUE(module.IsValid());
    }

    {
        // Test begin address greater than end address.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        RUNTIME_FUNCTION runtime_function = { 0 };
        runtime_function.BeginAddress = 512;
        runtime_function.EndAddress = 128;
        unwind_functions_->SetHasRuntimeFunction(image_base_for_sanity_check,
            runtime_function, 256,
            &context);
        EXPECT_FALSE(unwinder->TryUnwind(&context, &module));
    }

    {
        // Test instruction pointer before begin address.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        RUNTIME_FUNCTION runtime_function = { 0 };
        runtime_function.BeginAddress = 128;
        runtime_function.EndAddress = 512;
        unwind_functions_->SetHasRuntimeFunction(image_base_for_sanity_check,
            runtime_function, 50,
            &context);
        EXPECT_FALSE(unwinder->TryUnwind(&context, &module));
    }

    {
        // Test instruction pointer after end address.
        scoped_ptr<Win32StackFrameUnwinder> unwinder = CreateUnwinder();
        CONTEXT context = { 0 };
        ScopedModuleHandle module;
        RUNTIME_FUNCTION runtime_function = { 0 };
        runtime_function.BeginAddress = 128;
        runtime_function.EndAddress = 512;
        unwind_functions_->SetHasRuntimeFunction(image_base_for_sanity_check,
            runtime_function, 600,
            &context);
        EXPECT_FALSE(unwinder->TryUnwind(&context, &module));
    }
}

} // namespace base
