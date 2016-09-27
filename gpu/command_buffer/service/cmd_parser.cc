// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the implementation of the command parser.

#include "gpu/command_buffer/service/cmd_parser.h"

#include "base/logging.h"
#include "base/debug/trace_event.h"

namespace gpu {

CommandParser::CommandParser(AsyncAPIInterface* handler)
    : get_(0),
      put_(0),
      buffer_(NULL),
      entry_count_(0),
      handler_(handler) {
}

void CommandParser::SetBuffer(
    void* shm_address,
    size_t shm_size,
    ptrdiff_t offset,
    size_t size) {
  // check proper alignments.
  DCHECK_EQ(0, (reinterpret_cast<intptr_t>(shm_address)) % 4);
  DCHECK_EQ(0, offset % 4);
  DCHECK_EQ(0u, size % 4);
  // check that the command buffer fits into the memory buffer.
  DCHECK_GE(shm_size, offset + size);
  get_ = 0;
  put_ = 0;
  char* buffer_begin = static_cast<char*>(shm_address) + offset;
  buffer_ = reinterpret_cast<CommandBufferEntry*>(buffer_begin);
  entry_count_ = size / 4;
}

// Process one command, reading the header from the command buffer, and
// forwarding the command index and the arguments to the handler.
// Note that:
// - validation needs to happen on a copy of the data (to avoid race
// conditions). This function only validates the header, leaving the arguments
// validation to the handler, so it can pass a reference to them.
// - get_ is modified *after* the command has been executed.
error::Error CommandParser::ProcessCommand() {
  CommandBufferOffset get = get_;
  if (get == put_)
    return error::kNoError;

  CommandHeader header = buffer_[get].value_header;
  if (header.size == 0) {
    DVLOG(1) << "Error: zero sized command in command buffer";
    return error::kInvalidSize;
  }

  if (static_cast<int>(header.size) + get > entry_count_) {
    DVLOG(1) << "Error: get offset out of bounds";
    return error::kOutOfBounds;
  }

  TRACE_EVENT0(TRACE_DISABLED_BY_DEFAULT("cb_command"),
               handler_->GetCommandName(header.command));

  error::Error result = handler_->DoCommand(
      header.command, header.size - 1, buffer_ + get);

  // TODO(gman): If you want to log errors this is the best place to catch them.
  //     It seems like we need an official way to turn on a debug mode and
  //     get these errors.
  if (error::IsError(result)) {
    ReportError(header.command, result);
  }

  // If get was not set somewhere else advance it.
  if (get == get_ && result != error::kDeferCommandUntilLater)
    get_ = (get + header.size) % entry_count_;

  return result;
}

void CommandParser::ReportError(unsigned int command_id,
                                error::Error result) {
  DVLOG(1) << "Error: " << result << " for Command "
           << handler_->GetCommandName(command_id);
}

// Processes all the commands, while the buffer is not empty. Stop if an error
// is encountered.
error::Error CommandParser::ProcessAllCommands() {
  while (!IsEmpty()) {
    error::Error error = ProcessCommand();
    if (error)
      return error;
  }
  return error::kNoError;
}

}  // namespace gpu
