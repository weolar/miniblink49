// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/thread.h"
#include "base/time/time.h"
#include "gpu/command_buffer/service/gpu_scheduler.h"
#include "gpu/command_buffer/service/mocks.h"

namespace gpu {

AsyncAPIMock::AsyncAPIMock() {
  testing::DefaultValue<error::Error>::Set(
      error::kNoError);
}

AsyncAPIMock::~AsyncAPIMock() {}

void AsyncAPIMock::SetToken(unsigned int command,
                            unsigned int arg_count,
                            const void* _args) {
  DCHECK(engine_);
  DCHECK_EQ(1u, command);
  DCHECK_EQ(1u, arg_count);
  const cmd::SetToken* args =
      static_cast<const cmd::SetToken*>(_args);
  engine_->set_token(args->token);
}

namespace gles2 {

MockShaderTranslator::MockShaderTranslator() {}

MockShaderTranslator::~MockShaderTranslator() {}

MockProgramCache::MockProgramCache() {}
MockProgramCache::~MockProgramCache() {}

MockMemoryTracker::MockMemoryTracker() {}
MockMemoryTracker::~MockMemoryTracker() {}

}  // namespace gles2
}  // namespace gpu


