// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stack>
#include <vector>

#include "gpu/command_buffer/client/share_group.h"

#include "base/logging.h"
#include "base/synchronization/lock.h"
#include "gpu/command_buffer/client/gles2_implementation.h"
#include "gpu/command_buffer/client/program_info_manager.h"
#include "gpu/command_buffer/common/id_allocator.h"

namespace gpu {
namespace gles2 {

ShareGroupContextData::IdHandlerData::IdHandlerData() : flush_generation_(0) {}
ShareGroupContextData::IdHandlerData::~IdHandlerData() {}

COMPILE_ASSERT(gpu::kInvalidResource == 0,
               INVALID_RESOURCE_NOT_0_AS_GL_EXPECTS);

// The standard id handler.
class IdHandler : public IdHandlerInterface {
 public:
  IdHandler() { }
  virtual ~IdHandler() { }

  // Overridden from IdHandlerInterface.
  virtual void MakeIds(
      GLES2Implementation* /* gl_impl */,
      GLuint id_offset, GLsizei n, GLuint* ids) OVERRIDE {
    base::AutoLock auto_lock(lock_);
    if (id_offset == 0) {
      for (GLsizei ii = 0; ii < n; ++ii) {
        ids[ii] = id_allocator_.AllocateID();
      }
    } else {
      for (GLsizei ii = 0; ii < n; ++ii) {
        ids[ii] = id_allocator_.AllocateIDAtOrAbove(id_offset);
        id_offset = ids[ii] + 1;
      }
    }
  }

  // Overridden from IdHandlerInterface.
  virtual bool FreeIds(
      GLES2Implementation* gl_impl,
      GLsizei n, const GLuint* ids, DeleteFn delete_fn) OVERRIDE {
    base::AutoLock auto_lock(lock_);

    for (GLsizei ii = 0; ii < n; ++ii) {
      id_allocator_.FreeID(ids[ii]);
    }

    (gl_impl->*delete_fn)(n, ids);
    // We need to ensure that the delete call is evaluated on the service side
    // before any other contexts issue commands using these client ids.
    // TODO(vmiura): Can remove this by virtualizing internal ids, however
    // this code only affects PPAPI for now.
    gl_impl->helper()->CommandBufferHelper::Flush();
    return true;
  }

  // Overridden from IdHandlerInterface.
  virtual bool MarkAsUsedForBind(GLuint id) OVERRIDE {
    if (id == 0)
      return true;
    base::AutoLock auto_lock(lock_);
    return id_allocator_.MarkAsUsed(id);
  }

  virtual void FreeContext(GLES2Implementation* gl_impl) OVERRIDE {}

 private:
  base::Lock lock_;
  IdAllocator id_allocator_;
};

// An id handler that requires Gen before Bind.
class StrictIdHandler : public IdHandlerInterface {
 public:
  explicit StrictIdHandler(int id_namespace) : id_namespace_(id_namespace) {}
  virtual ~StrictIdHandler() {}

  // Overridden from IdHandler.
  virtual void MakeIds(GLES2Implementation* gl_impl,
                       GLuint /* id_offset */,
                       GLsizei n,
                       GLuint* ids) OVERRIDE {
    base::AutoLock auto_lock(lock_);

    // Collect pending FreeIds from other flush_generation.
    CollectPendingFreeIds(gl_impl);

    for (GLsizei ii = 0; ii < n; ++ii) {
      if (!free_ids_.empty()) {
        // Allocate a previously freed Id.
        ids[ii] = free_ids_.top();
        free_ids_.pop();

        // Record kIdInUse state.
        DCHECK(id_states_[ids[ii] - 1] == kIdFree);
        id_states_[ids[ii] - 1] = kIdInUse;
      } else {
        // Allocate a new Id.
        id_states_.push_back(kIdInUse);
        ids[ii] = id_states_.size();
      }
    }
  }

  // Overridden from IdHandler.
  virtual bool FreeIds(GLES2Implementation* gl_impl,
                       GLsizei n,
                       const GLuint* ids,
                       DeleteFn delete_fn) OVERRIDE {

    // Delete stub must run before CollectPendingFreeIds.
    (gl_impl->*delete_fn)(n, ids);

    {
      base::AutoLock auto_lock(lock_);

      // Collect pending FreeIds from other flush_generation.
      CollectPendingFreeIds(gl_impl);

      // Save Ids to free in a later flush_generation.
      ShareGroupContextData::IdHandlerData* ctxt_data =
          gl_impl->share_group_context_data()->id_handler_data(id_namespace_);

      for (GLsizei ii = 0; ii < n; ++ii) {
        GLuint id = ids[ii];
        if (id != 0) {
          // Save freed Id for later.
          DCHECK(id_states_[id - 1] == kIdInUse);
          id_states_[id - 1] = kIdPendingFree;
          ctxt_data->freed_ids_.push_back(id);
        }
      }
    }

    return true;
  }

  // Overridden from IdHandler.
  virtual bool MarkAsUsedForBind(GLuint id) OVERRIDE {
#ifndef NDEBUG
    if (id != 0) {
      base::AutoLock auto_lock(lock_);
      DCHECK(id_states_[id - 1] == kIdInUse);
    }
#endif
    return true;
  }

  // Overridden from IdHandlerInterface.
  virtual void FreeContext(GLES2Implementation* gl_impl) OVERRIDE {
    base::AutoLock auto_lock(lock_);
    CollectPendingFreeIds(gl_impl);
  }

 private:
  enum IdState { kIdFree, kIdPendingFree, kIdInUse };

  void CollectPendingFreeIds(GLES2Implementation* gl_impl) {
    uint32 flush_generation = gl_impl->helper()->flush_generation();
    ShareGroupContextData::IdHandlerData* ctxt_data =
        gl_impl->share_group_context_data()->id_handler_data(id_namespace_);

    if (ctxt_data->flush_generation_ != flush_generation) {
      ctxt_data->flush_generation_ = flush_generation;
      for (uint32 ii = 0; ii < ctxt_data->freed_ids_.size(); ++ii) {
        const GLuint id = ctxt_data->freed_ids_[ii];
        DCHECK(id_states_[id - 1] == kIdPendingFree);
        id_states_[id - 1] = kIdFree;
        free_ids_.push(id);
      }
      ctxt_data->freed_ids_.clear();
    }
  }

  int id_namespace_;

  base::Lock lock_;
  std::vector<uint8> id_states_;
  std::stack<uint32> free_ids_;
};

// An id handler for ids that are never reused.
class NonReusedIdHandler : public IdHandlerInterface {
 public:
  NonReusedIdHandler() : last_id_(0) {}
  virtual ~NonReusedIdHandler() {}

  // Overridden from IdHandlerInterface.
  virtual void MakeIds(
      GLES2Implementation* /* gl_impl */,
      GLuint id_offset, GLsizei n, GLuint* ids) OVERRIDE {
    base::AutoLock auto_lock(lock_);
    for (GLsizei ii = 0; ii < n; ++ii) {
      ids[ii] = ++last_id_ + id_offset;
    }
  }

  // Overridden from IdHandlerInterface.
  virtual bool FreeIds(
      GLES2Implementation* gl_impl,
      GLsizei n, const GLuint* ids, DeleteFn delete_fn) OVERRIDE {
    // Ids are never freed.
    (gl_impl->*delete_fn)(n, ids);
    return true;
  }

  // Overridden from IdHandlerInterface.
  virtual bool MarkAsUsedForBind(GLuint /* id */) OVERRIDE {
    // This is only used for Shaders and Programs which have no bind.
    return false;
  }

  virtual void FreeContext(GLES2Implementation* gl_impl) OVERRIDE {}

 private:
  base::Lock lock_;
  GLuint last_id_;
};

ShareGroup::ShareGroup(bool bind_generates_resource)
    : bind_generates_resource_(bind_generates_resource) {
  if (bind_generates_resource) {
    for (int i = 0; i < id_namespaces::kNumIdNamespaces; ++i) {
      if (i == id_namespaces::kProgramsAndShaders) {
        id_handlers_[i].reset(new NonReusedIdHandler());
      } else {
        id_handlers_[i].reset(new IdHandler());
      }
    }
  } else {
    for (int i = 0; i < id_namespaces::kNumIdNamespaces; ++i) {
      if (i == id_namespaces::kProgramsAndShaders) {
        id_handlers_[i].reset(new NonReusedIdHandler());
      } else {
        id_handlers_[i].reset(new StrictIdHandler(i));
      }
    }
  }
  program_info_manager_.reset(ProgramInfoManager::Create(false));
}

void ShareGroup::set_program_info_manager(ProgramInfoManager* manager) {
  program_info_manager_.reset(manager);
}

ShareGroup::~ShareGroup() {}

}  // namespace gles2
}  // namespace gpu
