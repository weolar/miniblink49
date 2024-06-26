// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gpu_scheduler.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/message_loop/message_loop.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "gpu/command_buffer/service/logger.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_fence.h"
#include "ui/gl/gl_switches.h"

using ::base::SharedMemory;

namespace gpu {

GpuScheduler::GpuScheduler(CommandBufferServiceBase* command_buffer,
    AsyncAPIInterface* handler,
    gles2::GLES2Decoder* decoder)
    : command_buffer_(command_buffer)
    , handler_(handler)
    , decoder_(decoder)
    , scheduled_(true)
    , was_preempted_(false)
{
}

GpuScheduler::~GpuScheduler() { }

void GpuScheduler::PutChanged()
{
    TRACE_EVENT1(
        "gpu", "GpuScheduler:PutChanged",
        "decoder", decoder_ ? decoder_->GetLogger()->GetLogPrefix() : "None");

    CommandBuffer::State state = command_buffer_->GetLastState();

    // If there is no parser, exit.
    if (!parser_.get()) {
        DCHECK_EQ(state.get_offset, command_buffer_->GetPutOffset());
        return;
    }

    parser_->set_put(command_buffer_->GetPutOffset());
    if (state.error != error::kNoError)
        return;

    base::TimeTicks begin_time(base::TimeTicks::Now());
    error::Error error = error::kNoError;
    if (decoder_)
        decoder_->BeginDecoding();
    while (!parser_->IsEmpty()) {
        if (IsPreempted())
            break;

        DCHECK(scheduled());

        error = parser_->ProcessCommands(CommandParser::kParseCommandsSlice);

        if (error == error::kDeferCommandUntilLater) {
            DCHECK(!scheduled());
            break;
        }

        // TODO(piman): various classes duplicate various pieces of state, leading
        // to needlessly complex update logic. It should be possible to simply
        // share the state across all of them.
        command_buffer_->SetGetOffset(static_cast<int32>(parser_->get()));

        if (error::IsError(error)) {
            command_buffer_->SetContextLostReason(decoder_->GetContextLostReason());
            command_buffer_->SetParseError(error);
            break;
        }

        if (!command_processed_callback_.is_null())
            command_processed_callback_.Run();

        if (!scheduled())
            break;
    }

    if (decoder_) {
        if (!error::IsError(error) && decoder_->WasContextLost()) {
            command_buffer_->SetContextLostReason(decoder_->GetContextLostReason());
            command_buffer_->SetParseError(error::kLostContext);
        }
        decoder_->EndDecoding();
        decoder_->AddProcessingCommandsTime(base::TimeTicks::Now() - begin_time);
    }
}

void GpuScheduler::SetScheduled(bool scheduled)
{
    TRACE_EVENT2("gpu", "GpuScheduler:SetScheduled", "this", this, "scheduled",
        scheduled);
    if (scheduled_ == scheduled)
        return;
    scheduled_ = scheduled;
    if (!scheduling_changed_callback_.is_null())
        scheduling_changed_callback_.Run(scheduled);
}

bool GpuScheduler::HasPendingQueries() const
{
    return (decoder_ && decoder_->HasPendingQueries());
}

void GpuScheduler::ProcessPendingQueries()
{
    if (!decoder_)
        return;
    decoder_->ProcessPendingQueries(false);
}

void GpuScheduler::SetSchedulingChangedCallback(
    const SchedulingChangedCallback& callback)
{
    scheduling_changed_callback_ = callback;
}

scoped_refptr<Buffer> GpuScheduler::GetSharedMemoryBuffer(int32 shm_id)
{
    return command_buffer_->GetTransferBuffer(shm_id);
}

void GpuScheduler::set_token(int32 token)
{
    command_buffer_->SetToken(token);
}

bool GpuScheduler::SetGetBuffer(int32 transfer_buffer_id)
{
    scoped_refptr<Buffer> ring_buffer = command_buffer_->GetTransferBuffer(transfer_buffer_id);
    if (!ring_buffer.get()) {
        return false;
    }

    if (!parser_.get()) {
        parser_.reset(new CommandParser(handler_));
    }

    parser_->SetBuffer(
        ring_buffer->memory(), ring_buffer->size(), 0, ring_buffer->size());

    SetGetOffset(0);
    return true;
}

bool GpuScheduler::SetGetOffset(int32 offset)
{
    if (parser_->set_get(offset)) {
        command_buffer_->SetGetOffset(static_cast<int32>(parser_->get()));
        return true;
    }
    return false;
}

int32 GpuScheduler::GetGetOffset()
{
    return parser_->get();
}

void GpuScheduler::SetCommandProcessedCallback(
    const base::Closure& callback)
{
    command_processed_callback_ = callback;
}

bool GpuScheduler::IsPreempted()
{
    if (!preemption_flag_.get())
        return false;

    if (!was_preempted_ && preemption_flag_->IsSet()) {
        TRACE_COUNTER_ID1("gpu", "GpuScheduler::Preempted", this, 1);
        was_preempted_ = true;
    } else if (was_preempted_ && !preemption_flag_->IsSet()) {
        TRACE_COUNTER_ID1("gpu", "GpuScheduler::Preempted", this, 0);
        was_preempted_ = false;
    }

    return preemption_flag_->IsSet();
}

bool GpuScheduler::HasMoreIdleWork() const
{
    return (decoder_ && decoder_->HasMoreIdleWork());
}

void GpuScheduler::PerformIdleWork()
{
    if (!decoder_)
        return;
    decoder_->PerformIdleWork();
}

} // namespace gpu
