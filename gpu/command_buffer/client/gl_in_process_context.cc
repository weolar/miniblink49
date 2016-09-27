// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/client/gl_in_process_context.h"

#include <set>
#include <utility>
#include <vector>

#include <GLES2/gl2.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop/message_loop.h"
#include "gpu/command_buffer/client/gles2_implementation.h"
#include "gpu/command_buffer/client/transfer_buffer.h"
#include "gpu/command_buffer/common/command_buffer.h"
#include "gpu/command_buffer/common/constants.h"
#include "ui/gfx/size.h"
#include "ui/gl/gl_image.h"

#if defined(OS_ANDROID)
#include "ui/gl/android/surface_texture.h"
#endif

namespace gpu {

namespace {

const int32 kCommandBufferSize = 1024 * 1024;
// TODO(kbr): make the transfer buffer size configurable via context
// creation attributes.
const size_t kStartTransferBufferSize = 4 * 1024 * 1024;
const size_t kMinTransferBufferSize = 1 * 256 * 1024;
const size_t kMaxTransferBufferSize = 6 * 1024 * 1024;

class GLInProcessContextImpl
    : public GLInProcessContext,
      public base::SupportsWeakPtr<GLInProcessContextImpl> {
 public:
  explicit GLInProcessContextImpl();
  virtual ~GLInProcessContextImpl();

  bool Initialize(
      scoped_refptr<gfx::GLSurface> surface,
      bool is_offscreen,
      bool use_global_share_group,
      GLInProcessContext* share_context,
      gfx::AcceleratedWidget window,
      const gfx::Size& size,
      const GLInProcessContextAttribs& attribs,
      gfx::GpuPreference gpu_preference,
      const scoped_refptr<InProcessCommandBuffer::Service>& service);

  // GLInProcessContext implementation:
  virtual void SetContextLostCallback(const base::Closure& callback) OVERRIDE;
  virtual gles2::GLES2Implementation* GetImplementation() OVERRIDE;

#if defined(OS_ANDROID)
  virtual scoped_refptr<gfx::SurfaceTexture> GetSurfaceTexture(
      uint32 stream_id) OVERRIDE;
#endif

 private:
  void Destroy();
  void OnContextLost();
  void OnSignalSyncPoint(const base::Closure& callback);

  scoped_ptr<gles2::GLES2CmdHelper> gles2_helper_;
  scoped_ptr<TransferBuffer> transfer_buffer_;
  scoped_ptr<gles2::GLES2Implementation> gles2_implementation_;
  scoped_ptr<InProcessCommandBuffer> command_buffer_;

  bool context_lost_;
  base::Closure context_lost_callback_;

  DISALLOW_COPY_AND_ASSIGN(GLInProcessContextImpl);
};

base::LazyInstance<base::Lock> g_all_shared_contexts_lock =
    LAZY_INSTANCE_INITIALIZER;
base::LazyInstance<std::set<GLInProcessContextImpl*> > g_all_shared_contexts =
    LAZY_INSTANCE_INITIALIZER;

GLInProcessContextImpl::GLInProcessContextImpl()
    : context_lost_(false) {}

GLInProcessContextImpl::~GLInProcessContextImpl() {
  {
    base::AutoLock lock(g_all_shared_contexts_lock.Get());
    g_all_shared_contexts.Get().erase(this);
  }
  Destroy();
}

gles2::GLES2Implementation* GLInProcessContextImpl::GetImplementation() {
  return gles2_implementation_.get();
}

void GLInProcessContextImpl::SetContextLostCallback(
    const base::Closure& callback) {
  context_lost_callback_ = callback;
}

void GLInProcessContextImpl::OnContextLost() {
  context_lost_ = true;
  if (!context_lost_callback_.is_null()) {
    context_lost_callback_.Run();
  }
}

bool GLInProcessContextImpl::Initialize(
    scoped_refptr<gfx::GLSurface> surface,
    bool is_offscreen,
    bool use_global_share_group,
    GLInProcessContext* share_context,
    gfx::AcceleratedWidget window,
    const gfx::Size& size,
    const GLInProcessContextAttribs& attribs,
    gfx::GpuPreference gpu_preference,
    const scoped_refptr<InProcessCommandBuffer::Service>& service) {
  DCHECK(!use_global_share_group || !share_context);
  DCHECK(size.width() >= 0 && size.height() >= 0);

  // Changes to these values should also be copied to
  // gpu/command_buffer/client/gl_in_process_context.cc and to
  // content/common/gpu/client/webgraphicscontext3d_command_buffer_impl.h
  const int32 ALPHA_SIZE     = 0x3021;
  const int32 BLUE_SIZE      = 0x3022;
  const int32 GREEN_SIZE     = 0x3023;
  const int32 RED_SIZE       = 0x3024;
  const int32 DEPTH_SIZE     = 0x3025;
  const int32 STENCIL_SIZE   = 0x3026;
  const int32 SAMPLES        = 0x3031;
  const int32 SAMPLE_BUFFERS = 0x3032;
  const int32 NONE           = 0x3038;

  // Chromium-specific attributes
  const int32 FAIL_IF_MAJOR_PERF_CAVEAT = 0x10002;
  const int32 LOSE_CONTEXT_WHEN_OUT_OF_MEMORY = 0x10003;

#ifdef TENCENT_CHANGES
  const int32 SHARE_SYS_CONTEXT = 0x10004;
#endif

  std::vector<int32> attrib_vector;
  if (attribs.alpha_size >= 0) {
    attrib_vector.push_back(ALPHA_SIZE);
    attrib_vector.push_back(attribs.alpha_size);
  }
  if (attribs.blue_size >= 0) {
    attrib_vector.push_back(BLUE_SIZE);
    attrib_vector.push_back(attribs.blue_size);
  }
  if (attribs.green_size >= 0) {
    attrib_vector.push_back(GREEN_SIZE);
    attrib_vector.push_back(attribs.green_size);
  }
  if (attribs.red_size >= 0) {
    attrib_vector.push_back(RED_SIZE);
    attrib_vector.push_back(attribs.red_size);
  }
  if (attribs.depth_size >= 0) {
    attrib_vector.push_back(DEPTH_SIZE);
    attrib_vector.push_back(attribs.depth_size);
  }
  if (attribs.stencil_size >= 0) {
    attrib_vector.push_back(STENCIL_SIZE);
    attrib_vector.push_back(attribs.stencil_size);
  }
  if (attribs.samples >= 0) {
    attrib_vector.push_back(SAMPLES);
    attrib_vector.push_back(attribs.samples);
  }
  if (attribs.sample_buffers >= 0) {
    attrib_vector.push_back(SAMPLE_BUFFERS);
    attrib_vector.push_back(attribs.sample_buffers);
  }
  if (attribs.fail_if_major_perf_caveat > 0) {
    attrib_vector.push_back(FAIL_IF_MAJOR_PERF_CAVEAT);
    attrib_vector.push_back(attribs.fail_if_major_perf_caveat);
  }
  if (attribs.lose_context_when_out_of_memory > 0) {
    attrib_vector.push_back(LOSE_CONTEXT_WHEN_OUT_OF_MEMORY);
    attrib_vector.push_back(attribs.lose_context_when_out_of_memory);
  }
#ifdef TENCENT_CHANGES
  if (attribs.share_sys_context > 0) {
    attrib_vector.push_back(SHARE_SYS_CONTEXT);
    attrib_vector.push_back(attribs.share_sys_context);
  }
#endif
  attrib_vector.push_back(NONE);

  base::Closure wrapped_callback =
      base::Bind(&GLInProcessContextImpl::OnContextLost, AsWeakPtr());
  command_buffer_.reset(new InProcessCommandBuffer(service));

  scoped_ptr<base::AutoLock> scoped_shared_context_lock;
  scoped_refptr<gles2::ShareGroup> share_group;
  InProcessCommandBuffer* share_command_buffer = NULL;
  if (use_global_share_group) {
    scoped_shared_context_lock.reset(
        new base::AutoLock(g_all_shared_contexts_lock.Get()));
    for (std::set<GLInProcessContextImpl*>::const_iterator it =
             g_all_shared_contexts.Get().begin();
         it != g_all_shared_contexts.Get().end();
         it++) {
      const GLInProcessContextImpl* context = *it;
      if (!context->context_lost_) {
        share_group = context->gles2_implementation_->share_group();
        share_command_buffer = context->command_buffer_.get();
        DCHECK(share_group);
        DCHECK(share_command_buffer);
        break;
      }
    }
  } else if (share_context) {
    GLInProcessContextImpl* impl =
        static_cast<GLInProcessContextImpl*>(share_context);
    share_group = impl->gles2_implementation_->share_group();
    share_command_buffer = impl->command_buffer_.get();
    DCHECK(share_group);
    DCHECK(share_command_buffer);
  }

  if (!command_buffer_->Initialize(surface,
                                   is_offscreen,
                                   window,
                                   size,
                                   attrib_vector,
                                   gpu_preference,
                                   wrapped_callback,
                                   share_command_buffer)) {
    LOG(ERROR) << "Failed to initialize InProcessCommmandBuffer";
    return false;
  }

  // Create the GLES2 helper, which writes the command buffer protocol.
  gles2_helper_.reset(new gles2::GLES2CmdHelper(command_buffer_.get()));
  if (!gles2_helper_->Initialize(kCommandBufferSize)) {
    LOG(ERROR) << "Failed to initialize GLES2CmdHelper";
    Destroy();
    return false;
  }

  // Create a transfer buffer.
  transfer_buffer_.reset(new TransferBuffer(gles2_helper_.get()));

  bool bind_generates_resources = false;

  // Create the object exposing the OpenGL API.
  gles2_implementation_.reset(new gles2::GLES2Implementation(
      gles2_helper_.get(),
      share_group,
      transfer_buffer_.get(),
      bind_generates_resources,
      attribs.lose_context_when_out_of_memory > 0,
      command_buffer_.get()));

  if (use_global_share_group) {
    g_all_shared_contexts.Get().insert(this);
    scoped_shared_context_lock.reset();
  }

  if (!gles2_implementation_->Initialize(
      kStartTransferBufferSize,
      kMinTransferBufferSize,
      kMaxTransferBufferSize,
      gles2::GLES2Implementation::kNoLimit)) {
    return false;
  }

  return true;
}

void GLInProcessContextImpl::Destroy() {
  if (gles2_implementation_) {
    // First flush the context to ensure that any pending frees of resources
    // are completed. Otherwise, if this context is part of a share group,
    // those resources might leak. Also, any remaining side effects of commands
    // issued on this context might not be visible to other contexts in the
    // share group.
    gles2_implementation_->Flush();

    gles2_implementation_.reset();
  }

  transfer_buffer_.reset();
  gles2_helper_.reset();
  command_buffer_.reset();
}

#if defined(OS_ANDROID)
scoped_refptr<gfx::SurfaceTexture>
GLInProcessContextImpl::GetSurfaceTexture(uint32 stream_id) {
  return command_buffer_->GetSurfaceTexture(stream_id);
}
#endif

}  // anonymous namespace

GLInProcessContextAttribs::GLInProcessContextAttribs()
    : alpha_size(-1),
      blue_size(-1),
      green_size(-1),
      red_size(-1),
      depth_size(-1),
      stencil_size(-1),
      samples(-1),
      sample_buffers(-1),
      fail_if_major_perf_caveat(-1),
#ifdef TENCENT_CHANGES
      lose_context_when_out_of_memory(-1),
      share_sys_context(-1) {}
#else
      lose_context_when_out_of_memory(-1) {}
#endif

GLInProcessContext* GLInProcessContext::Create(
    scoped_refptr<gpu::InProcessCommandBuffer::Service> service,
    scoped_refptr<gfx::GLSurface> surface,
    bool is_offscreen,
    gfx::AcceleratedWidget window,
    const gfx::Size& size,
    GLInProcessContext* share_context,
    bool use_global_share_group,
    const GLInProcessContextAttribs& attribs,
    gfx::GpuPreference gpu_preference) {
  DCHECK(!use_global_share_group || !share_context);
  if (surface.get()) {
    DCHECK_EQ(surface->IsOffscreen(), is_offscreen);
    DCHECK(surface->GetSize() == size);
    DCHECK_EQ(gfx::kNullAcceleratedWidget, window);
  }

  scoped_ptr<GLInProcessContextImpl> context(new GLInProcessContextImpl());
  if (!context->Initialize(surface,
                           is_offscreen,
                           use_global_share_group,
                           share_context,
                           window,
                           size,
                           attribs,
                           gpu_preference,
                           service))
    return NULL;

  return context.release();
}

}  // namespace gpu
