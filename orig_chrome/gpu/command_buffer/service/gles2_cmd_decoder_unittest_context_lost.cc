// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gles2_cmd_decoder.h"

#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"
#include "gpu/command_buffer/common/gles2_cmd_format.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/service/cmd_buffer_engine.h"
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/gl_surface_mock.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder_unittest.h"
#include "gpu/command_buffer/service/gpu_switches.h"
#include "gpu/command_buffer/service/mocks.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_mock.h"

using ::gfx::MockGLInterface;
using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::MatcherCast;
using ::testing::Mock;
using ::testing::Pointee;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::SetArgPointee;
using ::testing::SetArgumentPointee;
using ::testing::SetArrayArgument;
using ::testing::StrEq;
using ::testing::StrictMock;

namespace gpu {
namespace gles2 {

    using namespace cmds;

    class GLES2DecoderDrawOOMTest : public GLES2DecoderManualInitTest {
    protected:
        void Init(bool has_robustness)
        {
            InitState init;
            init.lose_context_when_out_of_memory = true;
            if (has_robustness)
                init.extensions = "GL_ARB_robustness";
            InitDecoder(init);
            SetupDefaultProgram();
        }

        void Draw(GLenum reset_status,
            error::ContextLostReason expected_other_reason)
        {
            const GLsizei kFakeLargeCount = 0x1234;
            SetupTexture();
            if (context_->WasAllocatedUsingRobustnessExtension()) {
                EXPECT_CALL(*gl_, GetGraphicsResetStatusARB())
                    .WillOnce(Return(reset_status));
            }
            AddExpectationsForSimulatedAttrib0WithError(kFakeLargeCount, 0,
                GL_OUT_OF_MEMORY);
            EXPECT_CALL(*gl_, DrawArrays(_, _, _)).Times(0).RetiresOnSaturation();
            // Other contexts in the group should be lost also.
            EXPECT_CALL(*mock_decoder_, MarkContextLost(expected_other_reason))
                .Times(1)
                .RetiresOnSaturation();
            DrawArrays cmd;
            cmd.Init(GL_TRIANGLES, 0, kFakeLargeCount);
            EXPECT_EQ(error::kLostContext, ExecuteCmd(cmd));
        }
    };

    // Test that we lose context.
    TEST_P(GLES2DecoderDrawOOMTest, ContextLostReasonOOM)
    {
        Init(false); // without robustness
        const error::ContextLostReason expected_reason_for_other_contexts = error::kOutOfMemory;
        Draw(GL_NO_ERROR, expected_reason_for_other_contexts);
        EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_EQ(error::kOutOfMemory, decoder_->GetContextLostReason());
    }

    TEST_P(GLES2DecoderDrawOOMTest, ContextLostReasonWhenStatusIsNoError)
    {
        Init(true); // with robustness
        // If the reset status is NO_ERROR, we should be signaling kOutOfMemory.
        const error::ContextLostReason expected_reason_for_other_contexts = error::kOutOfMemory;
        Draw(GL_NO_ERROR, expected_reason_for_other_contexts);
        EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_EQ(error::kOutOfMemory, decoder_->GetContextLostReason());
    }

    TEST_P(GLES2DecoderDrawOOMTest, ContextLostReasonWhenStatusIsGuilty)
    {
        Init(true);
        // If there was a reset, it should override kOutOfMemory.
        const error::ContextLostReason expected_reason_for_other_contexts = error::kUnknown;
        Draw(GL_GUILTY_CONTEXT_RESET_ARB, expected_reason_for_other_contexts);
        EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_EQ(error::kGuilty, decoder_->GetContextLostReason());
    }

    TEST_P(GLES2DecoderDrawOOMTest, ContextLostReasonWhenStatusIsUnknown)
    {
        Init(true);
        // If there was a reset, it should override kOutOfMemory.
        const error::ContextLostReason expected_reason_for_other_contexts = error::kUnknown;
        Draw(GL_UNKNOWN_CONTEXT_RESET_ARB, expected_reason_for_other_contexts);
        EXPECT_EQ(GL_OUT_OF_MEMORY, GetGLError());
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_EQ(error::kUnknown, decoder_->GetContextLostReason());
    }

    INSTANTIATE_TEST_CASE_P(Service, GLES2DecoderDrawOOMTest, ::testing::Bool());

    class GLES2DecoderLostContextTest : public GLES2DecoderManualInitTest {
    protected:
        void Init(bool has_robustness)
        {
            InitState init;
            init.gl_version = "opengl es 2.0";
            if (has_robustness)
                init.extensions = "GL_KHR_robustness";
            InitDecoder(init);
        }

        void InitWithVirtualContextsAndRobustness()
        {
            base::CommandLine command_line(0, NULL);
            command_line.AppendSwitchASCII(
                switches::kGpuDriverBugWorkarounds,
                base::IntToString(USE_VIRTUALIZED_GL_CONTEXTS));
            InitState init;
            init.gl_version = "opengl es 2.0";
            init.extensions = "GL_KHR_robustness";
            InitDecoderWithCommandLine(init, &command_line);
        }

        void DoGetErrorWithContextLost(GLenum reset_status)
        {
            DCHECK(context_->HasExtension("GL_KHR_robustness"));
            EXPECT_CALL(*gl_, GetError())
                .WillOnce(Return(GL_CONTEXT_LOST_KHR))
                .RetiresOnSaturation();
            EXPECT_CALL(*gl_, GetGraphicsResetStatusARB())
                .WillOnce(Return(reset_status));
            cmds::GetError cmd;
            cmd.Init(shared_memory_id_, shared_memory_offset_);
            EXPECT_EQ(error::kLostContext, ExecuteCmd(cmd));
            EXPECT_EQ(static_cast<GLuint>(GL_NO_ERROR), *GetSharedMemoryAs<GLenum*>());
        }

        void ClearCurrentDecoderError()
        {
            DCHECK(decoder_->WasContextLost());
            EXPECT_CALL(*gl_, GetError())
                .WillOnce(Return(GL_CONTEXT_LOST_KHR))
                .RetiresOnSaturation();
            cmds::GetError cmd;
            cmd.Init(shared_memory_id_, shared_memory_offset_);
            EXPECT_EQ(error::kLostContext, ExecuteCmd(cmd));
        }
    };

    TEST_P(GLES2DecoderLostContextTest, LostFromMakeCurrent)
    {
        Init(false); // without robustness
        EXPECT_CALL(*context_, MakeCurrent(surface_.get())).WillOnce(Return(false));
        // Expect the group to be lost.
        EXPECT_CALL(*mock_decoder_, MarkContextLost(error::kUnknown)).Times(1);
        decoder_->MakeCurrent();
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_EQ(error::kMakeCurrentFailed, decoder_->GetContextLostReason());

        // We didn't process commands, so we need to clear the decoder error,
        // so that we can shut down cleanly.
        ClearCurrentDecoderError();
    }

    TEST_P(GLES2DecoderLostContextTest, LostFromMakeCurrentWithRobustness)
    {
        Init(true); // with robustness
        // If we can't make the context current, we cannot query the robustness
        // extension.
        EXPECT_CALL(*gl_, GetGraphicsResetStatusARB()).Times(0);
        EXPECT_CALL(*context_, MakeCurrent(surface_.get())).WillOnce(Return(false));
        // Expect the group to be lost.
        EXPECT_CALL(*mock_decoder_, MarkContextLost(error::kUnknown)).Times(1);
        decoder_->MakeCurrent();
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_FALSE(decoder_->WasContextLostByRobustnessExtension());
        EXPECT_EQ(error::kMakeCurrentFailed, decoder_->GetContextLostReason());

        // We didn't process commands, so we need to clear the decoder error,
        // so that we can shut down cleanly.
        ClearCurrentDecoderError();
    }

    TEST_P(GLES2DecoderLostContextTest, LostFromResetAfterMakeCurrent)
    {
        Init(true); // with robustness
        InSequence seq;
        EXPECT_CALL(*context_, MakeCurrent(surface_.get())).WillOnce(Return(true));
        EXPECT_CALL(*gl_, GetGraphicsResetStatusARB())
            .WillOnce(Return(GL_GUILTY_CONTEXT_RESET_KHR));
        // Expect the group to be lost.
        EXPECT_CALL(*mock_decoder_, MarkContextLost(error::kUnknown)).Times(1);
        decoder_->MakeCurrent();
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_TRUE(decoder_->WasContextLostByRobustnessExtension());
        EXPECT_EQ(error::kGuilty, decoder_->GetContextLostReason());

        // We didn't process commands, so we need to clear the decoder error,
        // so that we can shut down cleanly.
        ClearCurrentDecoderError();
    }

    TEST_P(GLES2DecoderLostContextTest, LoseGuiltyFromGLError)
    {
        Init(true);
        // Always expect other contexts to be signaled as 'kUnknown' since we can't
        // query their status without making them current.
        EXPECT_CALL(*mock_decoder_, MarkContextLost(error::kUnknown))
            .Times(1);
        DoGetErrorWithContextLost(GL_GUILTY_CONTEXT_RESET_KHR);
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_TRUE(decoder_->WasContextLostByRobustnessExtension());
        EXPECT_EQ(error::kGuilty, decoder_->GetContextLostReason());
    }

    TEST_P(GLES2DecoderLostContextTest, LoseInnocentFromGLError)
    {
        Init(true);
        // Always expect other contexts to be signaled as 'kUnknown' since we can't
        // query their status without making them current.
        EXPECT_CALL(*mock_decoder_, MarkContextLost(error::kUnknown))
            .Times(1);
        DoGetErrorWithContextLost(GL_INNOCENT_CONTEXT_RESET_KHR);
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_TRUE(decoder_->WasContextLostByRobustnessExtension());
        EXPECT_EQ(error::kInnocent, decoder_->GetContextLostReason());
    }

    TEST_P(GLES2DecoderLostContextTest, LoseVirtualContextWithRobustness)
    {
        InitWithVirtualContextsAndRobustness();
        EXPECT_CALL(*mock_decoder_, MarkContextLost(error::kUnknown))
            .Times(1);
        // Signal guilty....
        DoGetErrorWithContextLost(GL_GUILTY_CONTEXT_RESET_KHR);
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_TRUE(decoder_->WasContextLostByRobustnessExtension());
        // ...but make sure we don't pretend, since for virtual contexts we don't
        // know if this was really the guilty client.
        EXPECT_EQ(error::kUnknown, decoder_->GetContextLostReason());
    }

    TEST_P(GLES2DecoderLostContextTest, LoseGroupFromRobustness)
    {
        // If one context in a group is lost through robustness,
        // the other ones should also get lost and query the reset status.
        Init(true);
        EXPECT_CALL(*mock_decoder_, MarkContextLost(error::kUnknown))
            .Times(1);
        // There should be no GL calls, since we might not have a current context.
        EXPECT_CALL(*gl_, GetGraphicsResetStatusARB()).Times(0);
        LoseContexts(error::kUnknown);
        EXPECT_TRUE(decoder_->WasContextLost());
        EXPECT_EQ(error::kUnknown, decoder_->GetContextLostReason());

        // We didn't process commands, so we need to clear the decoder error,
        // so that we can shut down cleanly.
        ClearCurrentDecoderError();
    }

    INSTANTIATE_TEST_CASE_P(Service,
        GLES2DecoderLostContextTest,
        ::testing::Bool());

} // namespace gles2
} // namespace gpu
