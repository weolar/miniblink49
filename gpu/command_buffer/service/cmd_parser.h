// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the command parser class.

#ifndef GPU_COMMAND_BUFFER_SERVICE_CMD_PARSER_H_
#define GPU_COMMAND_BUFFER_SERVICE_CMD_PARSER_H_

#include "gpu/command_buffer/common/constants.h"
#include "gpu/command_buffer/common/cmd_buffer_common.h"
#include "gpu/gpu_export.h"

namespace gpu {

class AsyncAPIInterface;

// Command parser class. This class parses commands from a shared memory
// buffer, to implement some asynchronous RPC mechanism.
class GPU_EXPORT CommandParser {
 public:
  explicit CommandParser(AsyncAPIInterface* handler);

  // Sets the buffer to read commands from.
  void SetBuffer(
      void* shm_address,
      size_t shm_size,
      ptrdiff_t offset,
      size_t size);

  // Gets the "get" pointer. The get pointer is an index into the command
  // buffer considered as an array of CommandBufferEntry.
  CommandBufferOffset get() const { return get_; }

  // Sets the "get" pointer. The get pointer is an index into the command buffer
  // considered as an array of CommandBufferEntry.
  bool set_get(CommandBufferOffset get) {
    if (get >= 0 && get < entry_count_) {
      get_ = get;
      return true;
    }
    return false;
  }

  // Sets the "put" pointer. The put pointer is an index into the command
  // buffer considered as an array of CommandBufferEntry.
  void set_put(CommandBufferOffset put) { put_ = put; }

  // Gets the "put" pointer. The put pointer is an index into the command
  // buffer considered as an array of CommandBufferEntry.
  CommandBufferOffset put() const { return put_; }

  // Checks whether there are commands to process.
  bool IsEmpty() const { return put_ == get_; }

  // Processes one command, updating the get pointer. This will return an error
  // if there are no commands in the buffer.
  error::Error ProcessCommand();

  // Processes all commands until get == put.
  error::Error ProcessAllCommands();

  // Reports an error.
  void ReportError(unsigned int command_id, error::Error result);

 private:
  CommandBufferOffset get_;
  CommandBufferOffset put_;
  CommandBufferEntry* buffer_;
  int32 entry_count_;
  AsyncAPIInterface* handler_;
};

// This class defines the interface for an asynchronous API handler, that
// is responsible for de-multiplexing commands and their arguments.
class AsyncAPIInterface {
 public:
  AsyncAPIInterface() {}
  virtual ~AsyncAPIInterface() {}

  // Executes a command.
  // Parameters:
  //    command: the command index.
  //    arg_count: the number of CommandBufferEntry arguments.
  //    cmd_data: the command data.
  // Returns:
  //   error::kNoError if no error was found, one of
  //   error::Error otherwise.
  virtual error::Error DoCommand(
      unsigned int command,
      unsigned int arg_count,
      const void* cmd_data) = 0;

  // Returns a name for a command. Useful for logging / debuging.
  virtual const char* GetCommandName(unsigned int command_id) const = 0;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_CMD_PARSER_H_
