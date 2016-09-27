// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A class to emulate GLES2 over command buffers.

#include "gpu/command_buffer/client/gles2_implementation.h"

#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>
#include <algorithm>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include "base/bind.h"
#include "gpu/command_buffer/client/buffer_tracker.h"
#include "gpu/command_buffer/client/gpu_control.h"
#include "gpu/command_buffer/client/gpu_memory_buffer_tracker.h"
#include "gpu/command_buffer/client/program_info_manager.h"
#include "gpu/command_buffer/client/query_tracker.h"
#include "gpu/command_buffer/client/transfer_buffer.h"
#include "gpu/command_buffer/client/vertex_array_object_manager.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "gpu/command_buffer/common/trace_event.h"
#include "ui/gfx/gpu_memory_buffer.h"

#if defined(__native_client__) && !defined(GLES2_SUPPORT_CLIENT_SIDE_ARRAYS)
#define GLES2_SUPPORT_CLIENT_SIDE_ARRAYS
#endif

#if defined(GPU_CLIENT_DEBUG)
#include "base/command_line.h"
#include "ui/gl/gl_switches.h"
#endif

namespace gpu {
namespace gles2 {

// A 32-bit and 64-bit compatible way of converting a pointer to a GLuint.
static GLuint ToGLuint(const void* ptr) {
  return static_cast<GLuint>(reinterpret_cast<size_t>(ptr));
}

#if !defined(_MSC_VER)
const size_t GLES2Implementation::kMaxSizeOfSimpleResult;
const unsigned int GLES2Implementation::kStartingOffset;
#endif

GLES2Implementation::GLStaticState::GLStaticState() {
}

GLES2Implementation::GLStaticState::~GLStaticState() {
}

GLES2Implementation::GLStaticState::IntState::IntState()
    : max_combined_texture_image_units(0),
      max_cube_map_texture_size(0),
      max_fragment_uniform_vectors(0),
      max_renderbuffer_size(0),
      max_texture_image_units(0),
      max_texture_size(0),
      max_varying_vectors(0),
      max_vertex_attribs(0),
      max_vertex_texture_image_units(0),
      max_vertex_uniform_vectors(0),
      num_compressed_texture_formats(0),
      num_shader_binary_formats(0),
      bind_generates_resource_chromium(0) {}

GLES2Implementation::SingleThreadChecker::SingleThreadChecker(
    GLES2Implementation* gles2_implementation)
    : gles2_implementation_(gles2_implementation) {
  CHECK_EQ(0, gles2_implementation_->use_count_);
  ++gles2_implementation_->use_count_;
}

GLES2Implementation::SingleThreadChecker::~SingleThreadChecker() {
  --gles2_implementation_->use_count_;
  CHECK_EQ(0, gles2_implementation_->use_count_);
}

GLES2Implementation::GLES2Implementation(
    GLES2CmdHelper* helper,
    ShareGroup* share_group,
    TransferBufferInterface* transfer_buffer,
    bool bind_generates_resource,
    bool lose_context_when_out_of_memory,
    GpuControl* gpu_control)
    : helper_(helper),
      transfer_buffer_(transfer_buffer),
      angle_pack_reverse_row_order_status_(kUnknownExtensionStatus),
      chromium_framebuffer_multisample_(kUnknownExtensionStatus),
      pack_alignment_(4),
      unpack_alignment_(4),
      unpack_flip_y_(false),
      unpack_row_length_(0),
      unpack_skip_rows_(0),
      unpack_skip_pixels_(0),
      pack_reverse_row_order_(false),
      active_texture_unit_(0),
      bound_framebuffer_(0),
      bound_read_framebuffer_(0),
      bound_renderbuffer_(0),
      current_program_(0),
      bound_array_buffer_id_(0),
      bound_pixel_pack_transfer_buffer_id_(0),
      bound_pixel_unpack_transfer_buffer_id_(0),
      async_upload_token_(0),
      async_upload_sync_(NULL),
      async_upload_sync_shm_id_(0),
      async_upload_sync_shm_offset_(0),
      error_bits_(0),
      debug_(false),
      lose_context_when_out_of_memory_(lose_context_when_out_of_memory),
      use_count_(0),
      error_message_callback_(NULL),
      gpu_control_(gpu_control),
      capabilities_(gpu_control->GetCapabilities()),
      weak_ptr_factory_(this) {
  DCHECK(helper);
  DCHECK(transfer_buffer);
  DCHECK(gpu_control);

  std::stringstream ss;
  ss << std::hex << this;
  this_in_hex_ = ss.str();

  GPU_CLIENT_LOG_CODE_BLOCK({
    debug_ = CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kEnableGPUClientLogging);
  });

  share_group_ =
      (share_group ? share_group : new ShareGroup(bind_generates_resource));

  memset(&reserved_ids_, 0, sizeof(reserved_ids_));
}

bool GLES2Implementation::Initialize(
    unsigned int starting_transfer_buffer_size,
    unsigned int min_transfer_buffer_size,
    unsigned int max_transfer_buffer_size,
    unsigned int mapped_memory_limit) {
  TRACE_EVENT0("gpu", "GLES2Implementation::Initialize");
  DCHECK_GE(starting_transfer_buffer_size, min_transfer_buffer_size);
  DCHECK_LE(starting_transfer_buffer_size, max_transfer_buffer_size);
  DCHECK_GE(min_transfer_buffer_size, kStartingOffset);

  if (!transfer_buffer_->Initialize(
      starting_transfer_buffer_size,
      kStartingOffset,
      min_transfer_buffer_size,
      max_transfer_buffer_size,
      kAlignment,
      kSizeToFlush)) {
    return false;
  }

  mapped_memory_.reset(
      new MappedMemoryManager(
          helper_,
          base::Bind(&GLES2Implementation::PollAsyncUploads,
                     // The mapped memory manager is owned by |this| here, and
                     // since its destroyed before before we destroy ourselves
                     // we don't need extra safety measures for this closure.
                     base::Unretained(this)),
          mapped_memory_limit));

  unsigned chunk_size = 2 * 1024 * 1024;
  if (mapped_memory_limit != kNoLimit) {
    // Use smaller chunks if the client is very memory conscientious.
    chunk_size = std::min(mapped_memory_limit / 4, chunk_size);
  }
  mapped_memory_->set_chunk_size_multiple(chunk_size);

  if (!QueryAndCacheStaticState())
    return false;

  util_.set_num_compressed_texture_formats(
      static_state_.int_state.num_compressed_texture_formats);
  util_.set_num_shader_binary_formats(
      static_state_.int_state.num_shader_binary_formats);

  texture_units_.reset(
      new TextureUnit[
          static_state_.int_state.max_combined_texture_image_units]);

  query_tracker_.reset(new QueryTracker(mapped_memory_.get()));
  buffer_tracker_.reset(new BufferTracker(mapped_memory_.get()));
  gpu_memory_buffer_tracker_.reset(new GpuMemoryBufferTracker(gpu_control_));

#if defined(GLES2_SUPPORT_CLIENT_SIDE_ARRAYS)
  GetIdHandler(id_namespaces::kBuffers)->MakeIds(
      this, kClientSideArrayId, arraysize(reserved_ids_), &reserved_ids_[0]);
#endif

  vertex_array_object_manager_.reset(new VertexArrayObjectManager(
      static_state_.int_state.max_vertex_attribs,
      reserved_ids_[0],
      reserved_ids_[1]));

  // GL_BIND_GENERATES_RESOURCE_CHROMIUM state must be the same
  // on Client & Service.
  if (static_state_.int_state.bind_generates_resource_chromium !=
      (share_group_->bind_generates_resource() ? 1 : 0)) {
    SetGLError(GL_INVALID_OPERATION,
               "Initialize",
               "Service bind_generates_resource mismatch.");
    return false;
  }

  return true;
}

bool GLES2Implementation::QueryAndCacheStaticState() {
  TRACE_EVENT0("gpu", "GLES2Implementation::QueryAndCacheStaticState");
  // Setup query for multiple GetIntegerv's
  static const GLenum pnames[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_FRAGMENT_UNIFORM_VECTORS,
    GL_MAX_RENDERBUFFER_SIZE,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_VECTORS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_VECTORS,
    GL_NUM_COMPRESSED_TEXTURE_FORMATS,
    GL_NUM_SHADER_BINARY_FORMATS,
    GL_BIND_GENERATES_RESOURCE_CHROMIUM,
  };

  GetMultipleIntegervState integerv_state(
      pnames, arraysize(pnames),
      &static_state_.int_state.max_combined_texture_image_units,
      sizeof(static_state_.int_state));
  if (!GetMultipleIntegervSetup(&integerv_state)) {
    return false;
  }

  // Setup query for multiple GetShaderPrecisionFormat's
  static const GLenum precision_params[][2] = {
    { GL_VERTEX_SHADER, GL_LOW_INT },
    { GL_VERTEX_SHADER, GL_MEDIUM_INT },
    { GL_VERTEX_SHADER, GL_HIGH_INT },
    { GL_VERTEX_SHADER, GL_LOW_FLOAT },
    { GL_VERTEX_SHADER, GL_MEDIUM_FLOAT },
    { GL_VERTEX_SHADER, GL_HIGH_FLOAT },
    { GL_FRAGMENT_SHADER, GL_LOW_INT },
    { GL_FRAGMENT_SHADER, GL_MEDIUM_INT },
    { GL_FRAGMENT_SHADER, GL_HIGH_INT },
    { GL_FRAGMENT_SHADER, GL_LOW_FLOAT },
    { GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT },
    { GL_FRAGMENT_SHADER, GL_HIGH_FLOAT },
  };

  GetAllShaderPrecisionFormatsState  precision_state(
      precision_params, arraysize(precision_params));
  GetAllShaderPrecisionFormatsSetup(&precision_state);

  // Allocate and partition transfer buffer for all requests
  void* buffer = transfer_buffer_->Alloc(
      integerv_state.transfer_buffer_size_needed +
      precision_state.transfer_buffer_size_needed);
  if (!buffer) {
    SetGLError(GL_OUT_OF_MEMORY, "QueryAndCacheStaticState",
               "Transfer buffer allocation failed.");
    return false;
  }
  integerv_state.buffer = buffer;
  precision_state.results_buffer =
      static_cast<char*>(buffer) + integerv_state.transfer_buffer_size_needed;

  // Make all the requests and wait once for all the results.
  GetMultipleIntegervRequest(&integerv_state);
  GetAllShaderPrecisionFormatsRequest(&precision_state);
  WaitForCmd();
  GetMultipleIntegervOnCompleted(&integerv_state);
  GetAllShaderPrecisionFormatsOnCompleted(&precision_state);

  // TODO(gman): We should be able to free without a token.
  transfer_buffer_->FreePendingToken(buffer, helper_->InsertToken());
  CheckGLError();

  return true;
}

GLES2Implementation::~GLES2Implementation() {
  // Make sure the queries are finished otherwise we'll delete the
  // shared memory (mapped_memory_) which will free the memory used
  // by the queries. The GPU process when validating that memory is still
  // shared will fail and abort (ie, it will stop running).
  WaitForCmd();
  query_tracker_.reset();

#if defined(GLES2_SUPPORT_CLIENT_SIDE_ARRAYS)
  DeleteBuffers(arraysize(reserved_ids_), &reserved_ids_[0]);
#endif

  // Release any per-context data in share group.
  share_group_->FreeContext(this);

  buffer_tracker_.reset();

  FreeAllAsyncUploadBuffers();

  if (async_upload_sync_) {
    mapped_memory_->Free(async_upload_sync_);
    async_upload_sync_ = NULL;
  }

  // Make sure the commands make it the service.
  WaitForCmd();
}

GLES2CmdHelper* GLES2Implementation::helper() const {
  return helper_;
}

IdHandlerInterface* GLES2Implementation::GetIdHandler(int namespace_id) const {
  return share_group_->GetIdHandler(namespace_id);
}

void* GLES2Implementation::GetResultBuffer() {
  return transfer_buffer_->GetResultBuffer();
}

int32 GLES2Implementation::GetResultShmId() {
  return transfer_buffer_->GetShmId();
}

uint32 GLES2Implementation::GetResultShmOffset() {
  return transfer_buffer_->GetResultOffset();
}

void GLES2Implementation::FreeUnusedSharedMemory() {
  mapped_memory_->FreeUnused();
}

void GLES2Implementation::FreeEverything() {
  FreeAllAsyncUploadBuffers();
  WaitForCmd();
  query_tracker_->Shrink();
  FreeUnusedSharedMemory();
  transfer_buffer_->Free();
  helper_->FreeRingBuffer();
}

void GLES2Implementation::RunIfContextNotLost(const base::Closure& callback) {
  if (!helper_->IsContextLost())
    callback.Run();
}

void GLES2Implementation::SignalSyncPoint(uint32 sync_point,
                                          const base::Closure& callback) {
  gpu_control_->SignalSyncPoint(
      sync_point,
      base::Bind(&GLES2Implementation::RunIfContextNotLost,
                 weak_ptr_factory_.GetWeakPtr(),
                 callback));
}

void GLES2Implementation::SignalQuery(uint32 query,
                                      const base::Closure& callback) {
  // Flush previously entered commands to ensure ordering with any
  // glBeginQueryEXT() calls that may have been put into the context.
  ShallowFlushCHROMIUM();
  gpu_control_->SignalQuery(
      query,
      base::Bind(&GLES2Implementation::RunIfContextNotLost,
                 weak_ptr_factory_.GetWeakPtr(),
                 callback));
}

void GLES2Implementation::SetSurfaceVisible(bool visible) {
  TRACE_EVENT1(
      "gpu", "GLES2Implementation::SetSurfaceVisible", "visible", visible);
  // TODO(piman): This probably should be ShallowFlushCHROMIUM().
  Flush();
  gpu_control_->SetSurfaceVisible(visible);
  if (!visible)
    FreeEverything();
}

void GLES2Implementation::WaitForCmd() {
  TRACE_EVENT0("gpu", "GLES2::WaitForCmd");
  helper_->CommandBufferHelper::Finish();
}

bool GLES2Implementation::IsExtensionAvailable(const char* ext) {
  const char* extensions =
      reinterpret_cast<const char*>(GetStringHelper(GL_EXTENSIONS));
  if (!extensions)
    return false;

  int length = strlen(ext);
  while (true) {
    int n = strcspn(extensions, " ");
    if (n == length && 0 == strncmp(ext, extensions, length)) {
      return true;
    }
    if ('\0' == extensions[n]) {
      return false;
    }
    extensions += n + 1;
  }
}

bool GLES2Implementation::IsExtensionAvailableHelper(
    const char* extension, ExtensionStatus* status) {
  switch (*status) {
    case kAvailableExtensionStatus:
      return true;
    case kUnavailableExtensionStatus:
      return false;
    default: {
      bool available = IsExtensionAvailable(extension);
      *status = available ? kAvailableExtensionStatus :
                            kUnavailableExtensionStatus;
      return available;
    }
  }
}

bool GLES2Implementation::IsAnglePackReverseRowOrderAvailable() {
  return IsExtensionAvailableHelper(
      "GL_ANGLE_pack_reverse_row_order",
      &angle_pack_reverse_row_order_status_);
}

bool GLES2Implementation::IsChromiumFramebufferMultisampleAvailable() {
  return IsExtensionAvailableHelper(
      "GL_CHROMIUM_framebuffer_multisample",
      &chromium_framebuffer_multisample_);
}

const std::string& GLES2Implementation::GetLogPrefix() const {
  const std::string& prefix(debug_marker_manager_.GetMarker());
  return prefix.empty() ? this_in_hex_ : prefix;
}

GLenum GLES2Implementation::GetError() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetError()");
  GLenum err = GetGLError();
  GPU_CLIENT_LOG("returned " << GLES2Util::GetStringError(err));
  return err;
}

GLenum GLES2Implementation::GetClientSideGLError() {
  if (error_bits_ == 0) {
    return GL_NO_ERROR;
  }

  GLenum error = GL_NO_ERROR;
  for (uint32 mask = 1; mask != 0; mask = mask << 1) {
    if ((error_bits_ & mask) != 0) {
      error = GLES2Util::GLErrorBitToGLError(mask);
      break;
    }
  }
  error_bits_ &= ~GLES2Util::GLErrorToErrorBit(error);
  return error;
}

GLenum GLES2Implementation::GetGLError() {
  TRACE_EVENT0("gpu", "GLES2::GetGLError");
  // Check the GL error first, then our wrapped error.
  typedef cmds::GetError::Result Result;
  Result* result = GetResultAs<Result*>();
  // If we couldn't allocate a result the context is lost.
  if (!result) {
    return GL_NO_ERROR;
  }
  *result = GL_NO_ERROR;
  helper_->GetError(GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GLenum error = *result;
  if (error == GL_NO_ERROR) {
    error = GetClientSideGLError();
  } else {
    // There was an error, clear the corresponding wrapped error.
    error_bits_ &= ~GLES2Util::GLErrorToErrorBit(error);
  }
  return error;
}

#if defined(GL_CLIENT_FAIL_GL_ERRORS)
void GLES2Implementation::FailGLError(GLenum error) {
  if (error != GL_NO_ERROR) {
    NOTREACHED() << "Error";
  }
}
// NOTE: Calling GetGLError overwrites data in the result buffer.
void GLES2Implementation::CheckGLError() {
  FailGLError(GetGLError());
}
#endif  // defined(GPU_CLIENT_FAIL_GL_ERRORS)

void GLES2Implementation::SetGLError(
    GLenum error, const char* function_name, const char* msg) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] Client Synthesized Error: "
                 << GLES2Util::GetStringError(error) << ": "
                 << function_name << ": " << msg);
  FailGLError(error);
  if (msg) {
    last_error_ = msg;
  }
  if (error_message_callback_) {
    std::string temp(GLES2Util::GetStringError(error)  + " : " +
                     function_name + ": " + (msg ? msg : ""));
    error_message_callback_->OnErrorMessage(temp.c_str(), 0);
  }
  error_bits_ |= GLES2Util::GLErrorToErrorBit(error);

  if (error == GL_OUT_OF_MEMORY && lose_context_when_out_of_memory_) {
    helper_->LoseContextCHROMIUM(GL_GUILTY_CONTEXT_RESET_ARB,
                                 GL_UNKNOWN_CONTEXT_RESET_ARB);
  }
}

void GLES2Implementation::SetGLErrorInvalidEnum(
    const char* function_name, GLenum value, const char* label) {
  SetGLError(GL_INVALID_ENUM, function_name,
             (std::string(label) + " was " +
              GLES2Util::GetStringEnum(value)).c_str());
}

bool GLES2Implementation::GetBucketContents(uint32 bucket_id,
                                            std::vector<int8>* data) {
  TRACE_EVENT0("gpu", "GLES2::GetBucketContents");
  DCHECK(data);
  const uint32 kStartSize = 32 * 1024;
  ScopedTransferBufferPtr buffer(kStartSize, helper_, transfer_buffer_);
  if (!buffer.valid()) {
    return false;
  }
  typedef cmd::GetBucketStart::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return false;
  }
  *result = 0;
  helper_->GetBucketStart(
      bucket_id, GetResultShmId(), GetResultShmOffset(),
      buffer.size(), buffer.shm_id(), buffer.offset());
  WaitForCmd();
  uint32 size = *result;
  data->resize(size);
  if (size > 0u) {
    uint32 offset = 0;
    while (size) {
      if (!buffer.valid()) {
        buffer.Reset(size);
        if (!buffer.valid()) {
          return false;
        }
        helper_->GetBucketData(
            bucket_id, offset, buffer.size(), buffer.shm_id(), buffer.offset());
        WaitForCmd();
      }
      uint32 size_to_copy = std::min(size, buffer.size());
      memcpy(&(*data)[offset], buffer.address(), size_to_copy);
      offset += size_to_copy;
      size -= size_to_copy;
      buffer.Release();
    };
    // Free the bucket. This is not required but it does free up the memory.
    // and we don't have to wait for the result so from the client's perspective
    // it's cheap.
    helper_->SetBucketSize(bucket_id, 0);
  }
  return true;
}

void GLES2Implementation::SetBucketContents(
    uint32 bucket_id, const void* data, size_t size) {
  DCHECK(data);
  helper_->SetBucketSize(bucket_id, size);
  if (size > 0u) {
    uint32 offset = 0;
    while (size) {
      ScopedTransferBufferPtr buffer(size, helper_, transfer_buffer_);
      if (!buffer.valid()) {
        return;
      }
      memcpy(buffer.address(), static_cast<const int8*>(data) + offset,
             buffer.size());
      helper_->SetBucketData(
          bucket_id, offset, buffer.size(), buffer.shm_id(), buffer.offset());
      offset += buffer.size();
      size -= buffer.size();
    }
  }
}

void GLES2Implementation::SetBucketAsCString(
    uint32 bucket_id, const char* str) {
  // NOTE: strings are passed NULL terminated. That means the empty
  // string will have a size of 1 and no-string will have a size of 0
  if (str) {
    SetBucketContents(bucket_id, str, strlen(str) + 1);
  } else {
    helper_->SetBucketSize(bucket_id, 0);
  }
}

bool GLES2Implementation::GetBucketAsString(
    uint32 bucket_id, std::string* str) {
  DCHECK(str);
  std::vector<int8> data;
  // NOTE: strings are passed NULL terminated. That means the empty
  // string will have a size of 1 and no-string will have a size of 0
  if (!GetBucketContents(bucket_id, &data)) {
    return false;
  }
  if (data.empty()) {
    return false;
  }
  str->assign(&data[0], &data[0] + data.size() - 1);
  return true;
}

void GLES2Implementation::SetBucketAsString(
    uint32 bucket_id, const std::string& str) {
  // NOTE: strings are passed NULL terminated. That means the empty
  // string will have a size of 1 and no-string will have a size of 0
  SetBucketContents(bucket_id, str.c_str(), str.size() + 1);
}

void GLES2Implementation::Disable(GLenum cap) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDisable("
                 << GLES2Util::GetStringCapability(cap) << ")");
  bool changed = false;
  if (!state_.SetCapabilityState(cap, false, &changed) || changed) {
    helper_->Disable(cap);
  }
  CheckGLError();
}

void GLES2Implementation::Enable(GLenum cap) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glEnable("
                 << GLES2Util::GetStringCapability(cap) << ")");
  bool changed = false;
  if (!state_.SetCapabilityState(cap, true, &changed) || changed) {
    helper_->Enable(cap);
  }
  CheckGLError();
}

GLboolean GLES2Implementation::IsEnabled(GLenum cap) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glIsEnabled("
                 << GLES2Util::GetStringCapability(cap) << ")");
  bool state = false;
  if (!state_.GetEnabled(cap, &state)) {
    typedef cmds::IsEnabled::Result Result;
    Result* result = GetResultAs<Result*>();
    if (!result) {
      return GL_FALSE;
    }
    *result = 0;
    helper_->IsEnabled(cap, GetResultShmId(), GetResultShmOffset());
    WaitForCmd();
    state = (*result) != 0;
  }

  GPU_CLIENT_LOG("returned " << state);
  CheckGLError();
  return state;
}

bool GLES2Implementation::GetHelper(GLenum pname, GLint* params) {
  switch (pname) {
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
      *params = static_state_.int_state.max_combined_texture_image_units;
      return true;
    case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
      *params = static_state_.int_state.max_cube_map_texture_size;
      return true;
    case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
      *params = static_state_.int_state.max_fragment_uniform_vectors;
      return true;
    case GL_MAX_RENDERBUFFER_SIZE:
      *params = static_state_.int_state.max_renderbuffer_size;
      return true;
    case GL_MAX_TEXTURE_IMAGE_UNITS:
      *params = static_state_.int_state.max_texture_image_units;
      return true;
    case GL_MAX_TEXTURE_SIZE:
      *params = static_state_.int_state.max_texture_size;
      return true;
    case GL_MAX_VARYING_VECTORS:
      *params = static_state_.int_state.max_varying_vectors;
      return true;
    case GL_MAX_VERTEX_ATTRIBS:
      *params = static_state_.int_state.max_vertex_attribs;
      return true;
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
      *params = static_state_.int_state.max_vertex_texture_image_units;
      return true;
    case GL_MAX_VERTEX_UNIFORM_VECTORS:
      *params = static_state_.int_state.max_vertex_uniform_vectors;
      return true;
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
      *params = static_state_.int_state.num_compressed_texture_formats;
      return true;
    case GL_NUM_SHADER_BINARY_FORMATS:
      *params = static_state_.int_state.num_shader_binary_formats;
      return true;
    case GL_ARRAY_BUFFER_BINDING:
      if (share_group_->bind_generates_resource()) {
        *params = bound_array_buffer_id_;
        return true;
      }
      return false;
    case GL_ELEMENT_ARRAY_BUFFER_BINDING:
      if (share_group_->bind_generates_resource()) {
        *params =
            vertex_array_object_manager_->bound_element_array_buffer();
        return true;
      }
      return false;
    case GL_PIXEL_PACK_TRANSFER_BUFFER_BINDING_CHROMIUM:
      *params = bound_pixel_pack_transfer_buffer_id_;
      return true;
    case GL_PIXEL_UNPACK_TRANSFER_BUFFER_BINDING_CHROMIUM:
      *params = bound_pixel_unpack_transfer_buffer_id_;
      return true;
    case GL_ACTIVE_TEXTURE:
      *params = active_texture_unit_ + GL_TEXTURE0;
      return true;
    case GL_TEXTURE_BINDING_2D:
      if (share_group_->bind_generates_resource()) {
        *params = texture_units_[active_texture_unit_].bound_texture_2d;
        return true;
      }
      return false;
    case GL_TEXTURE_BINDING_CUBE_MAP:
      if (share_group_->bind_generates_resource()) {
        *params = texture_units_[active_texture_unit_].bound_texture_cube_map;
        return true;
      }
      return false;
    case GL_TEXTURE_BINDING_EXTERNAL_OES:
      if (share_group_->bind_generates_resource()) {
        *params =
            texture_units_[active_texture_unit_].bound_texture_external_oes;
        return true;
      }
      return false;
    case GL_FRAMEBUFFER_BINDING:
      if (share_group_->bind_generates_resource()) {
        *params = bound_framebuffer_;
        return true;
      }
      return false;
    case GL_READ_FRAMEBUFFER_BINDING:
      if (IsChromiumFramebufferMultisampleAvailable() &&
          share_group_->bind_generates_resource()) {
        *params = bound_read_framebuffer_;
        return true;
      }
      return false;
    case GL_RENDERBUFFER_BINDING:
      if (share_group_->bind_generates_resource()) {
        *params = bound_renderbuffer_;
        return true;
      }
      return false;
    default:
      return false;
  }
}

bool GLES2Implementation::GetBooleanvHelper(GLenum pname, GLboolean* params) {
  // TODO(gman): Make this handle pnames that return more than 1 value.
  GLint value;
  if (!GetHelper(pname, &value)) {
    return false;
  }
  *params = static_cast<GLboolean>(value);
  return true;
}

bool GLES2Implementation::GetFloatvHelper(GLenum pname, GLfloat* params) {
  // TODO(gman): Make this handle pnames that return more than 1 value.
  GLint value;
  if (!GetHelper(pname, &value)) {
    return false;
  }
  *params = static_cast<GLfloat>(value);
  return true;
}

bool GLES2Implementation::GetIntegervHelper(GLenum pname, GLint* params) {
  return GetHelper(pname, params);
}

GLuint GLES2Implementation::GetMaxValueInBufferCHROMIUMHelper(
    GLuint buffer_id, GLsizei count, GLenum type, GLuint offset) {
  typedef cmds::GetMaxValueInBufferCHROMIUM::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return 0;
  }
  *result = 0;
  helper_->GetMaxValueInBufferCHROMIUM(
      buffer_id, count, type, offset, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  return *result;
}

GLuint GLES2Implementation::GetMaxValueInBufferCHROMIUM(
    GLuint buffer_id, GLsizei count, GLenum type, GLuint offset) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetMaxValueInBufferCHROMIUM("
                 << buffer_id << ", " << count << ", "
                 << GLES2Util::GetStringGetMaxIndexType(type)
                 << ", " << offset << ")");
  GLuint result = GetMaxValueInBufferCHROMIUMHelper(
      buffer_id, count, type, offset);
  GPU_CLIENT_LOG("returned " << result);
  CheckGLError();
  return result;
}

void GLES2Implementation::RestoreElementAndArrayBuffers(bool restore) {
  if (restore) {
    RestoreArrayBuffer(restore);
    // Restore the element array binding.
    // We only need to restore it if it wasn't a client side array.
    if (vertex_array_object_manager_->bound_element_array_buffer() == 0) {
      helper_->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
  }
}

void GLES2Implementation::RestoreArrayBuffer(bool restore) {
  if (restore) {
    // Restore the user's current binding.
    helper_->BindBuffer(GL_ARRAY_BUFFER, bound_array_buffer_id_);
  }
}

void GLES2Implementation::DrawElements(
    GLenum mode, GLsizei count, GLenum type, const void* indices) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDrawElements("
      << GLES2Util::GetStringDrawMode(mode) << ", "
      << count << ", "
      << GLES2Util::GetStringIndexType(type) << ", "
      << static_cast<const void*>(indices) << ")");
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawElements", "count less than 0.");
    return;
  }
  if (count == 0) {
    return;
  }
  if (vertex_array_object_manager_->bound_element_array_buffer() != 0 &&
      !ValidateOffset("glDrawElements", reinterpret_cast<GLintptr>(indices))) {
    return;
  }
  GLuint offset = 0;
  bool simulated = false;
  if (!vertex_array_object_manager_->SetupSimulatedIndexAndClientSideBuffers(
      "glDrawElements", this, helper_, count, type, 0, indices,
      &offset, &simulated)) {
    return;
  }
  helper_->DrawElements(mode, count, type, offset);
  RestoreElementAndArrayBuffers(simulated);
  CheckGLError();
}

void GLES2Implementation::Flush() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glFlush()");
  // Insert the cmd to call glFlush
  helper_->Flush();
  // Flush our command buffer
  // (tell the service to execute up to the flush cmd.)
  helper_->CommandBufferHelper::Flush();
}

void GLES2Implementation::ShallowFlushCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glShallowFlushCHROMIUM()");
  // Flush our command buffer
  // (tell the service to execute up to the flush cmd.)
  helper_->CommandBufferHelper::Flush();
  // TODO(piman): Add the FreeEverything() logic here.
}

void GLES2Implementation::Finish() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  FinishHelper();
}

void GLES2Implementation::ShallowFinishCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  TRACE_EVENT0("gpu", "GLES2::ShallowFinishCHROMIUM");
  // Flush our command buffer (tell the service to execute up to the flush cmd
  // and don't return until it completes).
  helper_->CommandBufferHelper::Finish();
}

void GLES2Implementation::FinishHelper() {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glFinish()");
  TRACE_EVENT0("gpu", "GLES2::Finish");
  // Insert the cmd to call glFinish
  helper_->Finish();
  // Finish our command buffer
  // (tell the service to execute up to the Finish cmd and wait for it to
  // execute.)
  helper_->CommandBufferHelper::Finish();
}

void GLES2Implementation::SwapBuffers() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glSwapBuffers()");
  // TODO(piman): Strictly speaking we'd want to insert the token after the
  // swap, but the state update with the updated token might not have happened
  // by the time the SwapBuffer callback gets called, forcing us to synchronize
  // with the GPU process more than needed. So instead, make it happen before.
  // All it means is that we could be slightly looser on the kMaxSwapBuffers
  // semantics if the client doesn't use the callback mechanism, and by chance
  // the scheduler yields between the InsertToken and the SwapBuffers.
  swap_buffers_tokens_.push(helper_->InsertToken());
  helper_->SwapBuffers();
  helper_->CommandBufferHelper::Flush();
  // Wait if we added too many swap buffers. Add 1 to kMaxSwapBuffers to
  // compensate for TODO above.
  if (swap_buffers_tokens_.size() > kMaxSwapBuffers + 1) {
    helper_->WaitForToken(swap_buffers_tokens_.front());
    swap_buffers_tokens_.pop();
  }
}

void GLES2Implementation::GenSharedIdsCHROMIUM(
  GLuint namespace_id, GLuint id_offset, GLsizei n, GLuint* ids) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenSharedIdsCHROMIUM("
      << namespace_id << ", " << id_offset << ", " << n << ", " <<
      static_cast<void*>(ids) << ")");
  TRACE_EVENT0("gpu", "GLES2::GenSharedIdsCHROMIUM");
  GLsizei num = n;
  GLuint* dst = ids;
  while (num) {
    ScopedTransferBufferArray<GLint> id_buffer(num, helper_, transfer_buffer_);
    if (!id_buffer.valid()) {
      return;
    }
    helper_->GenSharedIdsCHROMIUM(
        namespace_id, id_offset, id_buffer.num_elements(),
        id_buffer.shm_id(), id_buffer.offset());
    WaitForCmd();
    memcpy(dst, id_buffer.address(), sizeof(*dst) * id_buffer.num_elements());
    num -= id_buffer.num_elements();
    dst += id_buffer.num_elements();
  }
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << namespace_id << ", " << ids[i]);
    }
  });
}

void GLES2Implementation::DeleteSharedIdsCHROMIUM(
    GLuint namespace_id, GLsizei n, const GLuint* ids) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDeleteSharedIdsCHROMIUM("
      << namespace_id << ", " << n << ", "
      << static_cast<const void*>(ids) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << namespace_id << ", "  << ids[i]);
    }
  });
  TRACE_EVENT0("gpu", "GLES2::DeleteSharedIdsCHROMIUM");
  while (n) {
    ScopedTransferBufferArray<GLint> id_buffer(n, helper_, transfer_buffer_);
    if (!id_buffer.valid()) {
      return;
    }
    memcpy(id_buffer.address(), ids, sizeof(*ids) * id_buffer.num_elements());
    helper_->DeleteSharedIdsCHROMIUM(
        namespace_id, id_buffer.num_elements(),
        id_buffer.shm_id(), id_buffer.offset());
    WaitForCmd();
    n -= id_buffer.num_elements();
    ids += id_buffer.num_elements();
  }
}

void GLES2Implementation::RegisterSharedIdsCHROMIUM(
    GLuint namespace_id, GLsizei n, const GLuint* ids) {
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glRegisterSharedIdsCHROMIUM("
     << namespace_id << ", " << n << ", "
     << static_cast<const void*>(ids) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei i = 0; i < n; ++i) {
      GPU_CLIENT_LOG("  " << i << ": "  << namespace_id << ", " << ids[i]);
    }
  });
  TRACE_EVENT0("gpu", "GLES2::RegisterSharedIdsCHROMIUM");
  while (n) {
    ScopedTransferBufferArray<GLint> id_buffer(n, helper_, transfer_buffer_);
    if (!id_buffer.valid()) {
      return;
    }
    memcpy(id_buffer.address(), ids, sizeof(*ids) * id_buffer.num_elements());
    helper_->RegisterSharedIdsCHROMIUM(
        namespace_id, id_buffer.num_elements(),
        id_buffer.shm_id(), id_buffer.offset());
    WaitForCmd();
    n -= id_buffer.num_elements();
    ids += id_buffer.num_elements();
  }
}

void GLES2Implementation::BindAttribLocation(
  GLuint program, GLuint index, const char* name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindAttribLocation("
      << program << ", " << index << ", " << name << ")");
  SetBucketAsString(kResultBucketId, name);
  helper_->BindAttribLocationBucket(program, index, kResultBucketId);
  helper_->SetBucketSize(kResultBucketId, 0);
  CheckGLError();
}

void GLES2Implementation::BindUniformLocationCHROMIUM(
  GLuint program, GLint location, const char* name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBindUniformLocationCHROMIUM("
      << program << ", " << location << ", " << name << ")");
  SetBucketAsString(kResultBucketId, name);
  helper_->BindUniformLocationCHROMIUMBucket(
      program, location, kResultBucketId);
  helper_->SetBucketSize(kResultBucketId, 0);
  CheckGLError();
}

void GLES2Implementation::GetVertexAttribPointerv(
    GLuint index, GLenum pname, void** ptr) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetVertexAttribPointer("
      << index << ", " << GLES2Util::GetStringVertexPointer(pname) << ", "
      << static_cast<void*>(ptr) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK(int32 num_results = 1);
  if (!vertex_array_object_manager_->GetAttribPointer(index, pname, ptr)) {
    TRACE_EVENT0("gpu", "GLES2::GetVertexAttribPointerv");
    typedef cmds::GetVertexAttribPointerv::Result Result;
    Result* result = GetResultAs<Result*>();
    if (!result) {
      return;
    }
    result->SetNumResults(0);
    helper_->GetVertexAttribPointerv(
      index, pname, GetResultShmId(), GetResultShmOffset());
    WaitForCmd();
    result->CopyResult(ptr);
    GPU_CLIENT_LOG_CODE_BLOCK(num_results = result->GetNumResults());
  }
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32 i = 0; i < num_results; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << ptr[i]);
    }
  });
  CheckGLError();
}

bool GLES2Implementation::DeleteProgramHelper(GLuint program) {
  if (!GetIdHandler(id_namespaces::kProgramsAndShaders)->FreeIds(
      this, 1, &program, &GLES2Implementation::DeleteProgramStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteProgram", "id not created by this context.");
    return false;
  }
  if (program == current_program_) {
    current_program_ = 0;
  }
  return true;
}

void GLES2Implementation::DeleteProgramStub(
    GLsizei n, const GLuint* programs) {
  DCHECK_EQ(1, n);
  share_group_->program_info_manager()->DeleteInfo(programs[0]);
  helper_->DeleteProgram(programs[0]);
}

bool GLES2Implementation::DeleteShaderHelper(GLuint shader) {
  if (!GetIdHandler(id_namespaces::kProgramsAndShaders)->FreeIds(
      this, 1, &shader, &GLES2Implementation::DeleteShaderStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteShader", "id not created by this context.");
    return false;
  }
  return true;
}

void GLES2Implementation::DeleteShaderStub(
    GLsizei n, const GLuint* shaders) {
  DCHECK_EQ(1, n);
  share_group_->program_info_manager()->DeleteInfo(shaders[0]);
  helper_->DeleteShader(shaders[0]);
}


GLint GLES2Implementation::GetAttribLocationHelper(
    GLuint program, const char* name) {
  typedef cmds::GetAttribLocation::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return -1;
  }
  *result = -1;
  SetBucketAsCString(kResultBucketId, name);
  helper_->GetAttribLocation(
      program, kResultBucketId, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  helper_->SetBucketSize(kResultBucketId, 0);
  return *result;
}

GLint GLES2Implementation::GetAttribLocation(
    GLuint program, const char* name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetAttribLocation(" << program
      << ", " << name << ")");
  TRACE_EVENT0("gpu", "GLES2::GetAttribLocation");
  GLint loc = share_group_->program_info_manager()->GetAttribLocation(
      this, program, name);
  GPU_CLIENT_LOG("returned " << loc);
  CheckGLError();
  return loc;
}

GLint GLES2Implementation::GetUniformLocationHelper(
    GLuint program, const char* name) {
  typedef cmds::GetUniformLocation::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return -1;
  }
  *result = -1;
  SetBucketAsCString(kResultBucketId, name);
  helper_->GetUniformLocation(program, kResultBucketId,
                                    GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  helper_->SetBucketSize(kResultBucketId, 0);
  return *result;
}

GLint GLES2Implementation::GetUniformLocation(
    GLuint program, const char* name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetUniformLocation(" << program
      << ", " << name << ")");
  TRACE_EVENT0("gpu", "GLES2::GetUniformLocation");
  GLint loc = share_group_->program_info_manager()->GetUniformLocation(
      this, program, name);
  GPU_CLIENT_LOG("returned " << loc);
  CheckGLError();
  return loc;
}

bool GLES2Implementation::GetProgramivHelper(
    GLuint program, GLenum pname, GLint* params) {
  bool got_value = share_group_->program_info_manager()->GetProgramiv(
      this, program, pname, params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    if (got_value) {
      GPU_CLIENT_LOG("  0: " << *params);
    }
  });
  return got_value;
}

void GLES2Implementation::LinkProgram(GLuint program) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glLinkProgram(" << program << ")");
  helper_->LinkProgram(program);
  share_group_->program_info_manager()->CreateInfo(program);
  CheckGLError();
}

void GLES2Implementation::ShaderBinary(
    GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary,
    GLsizei length) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glShaderBinary(" << n << ", "
      << static_cast<const void*>(shaders) << ", "
      << GLES2Util::GetStringEnum(binaryformat) << ", "
      << static_cast<const void*>(binary) << ", "
      << length << ")");
  if (n < 0) {
    SetGLError(GL_INVALID_VALUE, "glShaderBinary", "n < 0.");
    return;
  }
  if (length < 0) {
    SetGLError(GL_INVALID_VALUE, "glShaderBinary", "length < 0.");
    return;
  }
  // TODO(gman): ShaderBinary should use buckets.
  unsigned int shader_id_size = n * sizeof(*shaders);
  ScopedTransferBufferArray<GLint> buffer(
      shader_id_size + length, helper_, transfer_buffer_);
  if (!buffer.valid() || buffer.num_elements() != shader_id_size + length) {
    SetGLError(GL_OUT_OF_MEMORY, "glShaderBinary", "out of memory.");
    return;
  }
  void* shader_ids = buffer.elements();
  void* shader_data = buffer.elements() + shader_id_size;
  memcpy(shader_ids, shaders, shader_id_size);
  memcpy(shader_data, binary, length);
  helper_->ShaderBinary(
      n,
      buffer.shm_id(),
      buffer.offset(),
      binaryformat,
      buffer.shm_id(),
      buffer.offset() + shader_id_size,
      length);
  CheckGLError();
}

void GLES2Implementation::PixelStorei(GLenum pname, GLint param) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glPixelStorei("
      << GLES2Util::GetStringPixelStore(pname) << ", "
      << param << ")");
  switch (pname) {
    case GL_PACK_ALIGNMENT:
        pack_alignment_ = param;
        break;
    case GL_UNPACK_ALIGNMENT:
        unpack_alignment_ = param;
        break;
    case GL_UNPACK_ROW_LENGTH_EXT:
        unpack_row_length_ = param;
        return;
    case GL_UNPACK_SKIP_ROWS_EXT:
        unpack_skip_rows_ = param;
        return;
    case GL_UNPACK_SKIP_PIXELS_EXT:
        unpack_skip_pixels_ = param;
        return;
    case GL_UNPACK_FLIP_Y_CHROMIUM:
        unpack_flip_y_ = (param != 0);
        break;
    case GL_PACK_REVERSE_ROW_ORDER_ANGLE:
        pack_reverse_row_order_ =
            IsAnglePackReverseRowOrderAvailable() ? (param != 0) : false;
        break;
    default:
        break;
  }
  helper_->PixelStorei(pname, param);
  CheckGLError();
}

void GLES2Implementation::VertexAttribPointer(
    GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
    const void* ptr) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttribPointer("
      << index << ", "
      << size << ", "
      << GLES2Util::GetStringVertexAttribType(type) << ", "
      << GLES2Util::GetStringBool(normalized) << ", "
      << stride << ", "
      << static_cast<const void*>(ptr) << ")");
  // Record the info on the client side.
  if (!vertex_array_object_manager_->SetAttribPointer(
      bound_array_buffer_id_, index, size, type, normalized, stride, ptr)) {
    SetGLError(GL_INVALID_OPERATION, "glVertexAttribPointer",
               "client side arrays are not allowed in vertex array objects.");
    return;
  }
#if defined(GLES2_SUPPORT_CLIENT_SIDE_ARRAYS)
  if (bound_array_buffer_id_ != 0) {
    // Only report NON client side buffers to the service.
    if (!ValidateOffset("glVertexAttribPointer",
                        reinterpret_cast<GLintptr>(ptr))) {
      return;
    }
    helper_->VertexAttribPointer(index, size, type, normalized, stride,
                                 ToGLuint(ptr));
  }
#else  // !defined(GLES2_SUPPORT_CLIENT_SIDE_ARRAYS)
  if (!ValidateOffset("glVertexAttribPointer",
                      reinterpret_cast<GLintptr>(ptr))) {
    return;
  }
  helper_->VertexAttribPointer(index, size, type, normalized, stride,
                               ToGLuint(ptr));
#endif  // !defined(GLES2_SUPPORT_CLIENT_SIDE_ARRAYS)
  CheckGLError();
}

void GLES2Implementation::VertexAttribDivisorANGLE(
    GLuint index, GLuint divisor) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glVertexAttribDivisorANGLE("
      << index << ", "
      << divisor << ") ");
  // Record the info on the client side.
  vertex_array_object_manager_->SetAttribDivisor(index, divisor);
  helper_->VertexAttribDivisorANGLE(index, divisor);
  CheckGLError();
}

void GLES2Implementation::ShaderSource(
    GLuint shader, GLsizei count, const GLchar* const* source, const GLint* length) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glShaderSource("
      << shader << ", " << count << ", "
      << static_cast<const void*>(source) << ", "
      << static_cast<const void*>(length) << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLsizei ii = 0; ii < count; ++ii) {
      if (source[ii]) {
        if (length && length[ii] >= 0) {
          std::string str(source[ii], length[ii]);
          GPU_CLIENT_LOG("  " << ii << ": ---\n" << str << "\n---");
        } else {
          GPU_CLIENT_LOG("  " << ii << ": ---\n" << source[ii] << "\n---");
        }
      } else {
        GPU_CLIENT_LOG("  " << ii << ": NULL");
      }
    }
  });
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glShaderSource", "count < 0");
    return;
  }
  if (shader == 0) {
    SetGLError(GL_INVALID_VALUE, "glShaderSource", "shader == 0");
    return;
  }

  // Compute the total size.
  uint32 total_size = 1;
  for (GLsizei ii = 0; ii < count; ++ii) {
    if (source[ii]) {
      total_size += (length && length[ii] >= 0) ?
          static_cast<size_t>(length[ii]) : strlen(source[ii]);
    }
  }

  // Concatenate all the strings in to a bucket on the service.
  helper_->SetBucketSize(kResultBucketId, total_size);
  uint32 offset = 0;
  for (GLsizei ii = 0; ii <= count; ++ii) {
    const char* src = ii < count ? source[ii] : "";
    if (src) {
      uint32 size = ii < count ?
          (length ? static_cast<size_t>(length[ii]) : strlen(src)) : 1;
      while (size) {
        ScopedTransferBufferPtr buffer(size, helper_, transfer_buffer_);
        if (!buffer.valid()) {
          return;
        }
        memcpy(buffer.address(), src, buffer.size());
        helper_->SetBucketData(kResultBucketId, offset, buffer.size(),
                               buffer.shm_id(), buffer.offset());
        offset += buffer.size();
        src += buffer.size();
        size -= buffer.size();
      }
    }
  }

  DCHECK_EQ(total_size, offset);

  helper_->ShaderSourceBucket(shader, kResultBucketId);
  helper_->SetBucketSize(kResultBucketId, 0);
  CheckGLError();
}

void GLES2Implementation::BufferDataHelper(
    GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
  if (!ValidateSize("glBufferData", size))
    return;

  GLuint buffer_id;
  if (GetBoundPixelTransferBuffer(target, "glBufferData", &buffer_id)) {
    if (!buffer_id) {
      return;
    }

    BufferTracker::Buffer* buffer = buffer_tracker_->GetBuffer(buffer_id);
    if (buffer)
      RemoveTransferBuffer(buffer);

    // Create new buffer.
    buffer = buffer_tracker_->CreateBuffer(buffer_id, size);
    DCHECK(buffer);
    if (buffer->address() && data)
      memcpy(buffer->address(), data, size);
    return;
  }

  if (size == 0) {
    return;
  }

  // If there is no data just send BufferData
  if (!data) {
    helper_->BufferData(target, size, 0, 0, usage);
    return;
  }

  // See if we can send all at once.
  ScopedTransferBufferPtr buffer(size, helper_, transfer_buffer_);
  if (!buffer.valid()) {
    return;
  }

  if (buffer.size() >= static_cast<unsigned int>(size)) {
    memcpy(buffer.address(), data, size);
    helper_->BufferData(
        target,
        size,
        buffer.shm_id(),
        buffer.offset(),
        usage);
    return;
  }

  // Make the buffer with BufferData then send via BufferSubData
  helper_->BufferData(target, size, 0, 0, usage);
  BufferSubDataHelperImpl(target, 0, size, data, &buffer);
  CheckGLError();
}

void GLES2Implementation::BufferData(
    GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBufferData("
      << GLES2Util::GetStringBufferTarget(target) << ", "
      << size << ", "
      << static_cast<const void*>(data) << ", "
      << GLES2Util::GetStringBufferUsage(usage) << ")");
  BufferDataHelper(target, size, data, usage);
  CheckGLError();
}

void GLES2Implementation::BufferSubDataHelper(
    GLenum target, GLintptr offset, GLsizeiptr size, const void* data) {
  if (size == 0) {
    return;
  }

  if (!ValidateSize("glBufferSubData", size) ||
      !ValidateOffset("glBufferSubData", offset)) {
    return;
  }

  GLuint buffer_id;
  if (GetBoundPixelTransferBuffer(target, "glBufferSubData", &buffer_id)) {
    if (!buffer_id) {
      return;
    }
    BufferTracker::Buffer* buffer = buffer_tracker_->GetBuffer(buffer_id);
    if (!buffer) {
      SetGLError(GL_INVALID_VALUE, "glBufferSubData", "unknown buffer");
      return;
    }

    int32 end = 0;
    int32 buffer_size = buffer->size();
    if (!SafeAddInt32(offset, size, &end) || end > buffer_size) {
      SetGLError(GL_INVALID_VALUE, "glBufferSubData", "out of range");
      return;
    }

    if (buffer->address() && data)
      memcpy(static_cast<uint8*>(buffer->address()) + offset, data, size);
    return;
  }

  ScopedTransferBufferPtr buffer(size, helper_, transfer_buffer_);
  BufferSubDataHelperImpl(target, offset, size, data, &buffer);
}

void GLES2Implementation::BufferSubDataHelperImpl(
    GLenum target, GLintptr offset, GLsizeiptr size, const void* data,
    ScopedTransferBufferPtr* buffer) {
  DCHECK(buffer);
  DCHECK_GT(size, 0);

  const int8* source = static_cast<const int8*>(data);
  while (size) {
    if (!buffer->valid() || buffer->size() == 0) {
      buffer->Reset(size);
      if (!buffer->valid()) {
        return;
      }
    }
    memcpy(buffer->address(), source, buffer->size());
    helper_->BufferSubData(
        target, offset, buffer->size(), buffer->shm_id(), buffer->offset());
    offset += buffer->size();
    source += buffer->size();
    size -= buffer->size();
    buffer->Release();
  }
}

void GLES2Implementation::BufferSubData(
    GLenum target, GLintptr offset, GLsizeiptr size, const void* data) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glBufferSubData("
      << GLES2Util::GetStringBufferTarget(target) << ", "
      << offset << ", " << size << ", "
      << static_cast<const void*>(data) << ")");
  BufferSubDataHelper(target, offset, size, data);
  CheckGLError();
}

void GLES2Implementation::RemoveTransferBuffer(BufferTracker::Buffer* buffer) {
  int32 token = buffer->last_usage_token();
  uint32 async_token = buffer->last_async_upload_token();

  if (async_token) {
    if (HasAsyncUploadTokenPassed(async_token)) {
      buffer_tracker_->Free(buffer);
    } else {
      detached_async_upload_memory_.push_back(
          std::make_pair(buffer->address(), async_token));
      buffer_tracker_->Unmanage(buffer);
    }
  } else if (token) {
    if (helper_->HasTokenPassed(token))
      buffer_tracker_->Free(buffer);
    else
      buffer_tracker_->FreePendingToken(buffer, token);
  } else {
      buffer_tracker_->Free(buffer);
  }

  buffer_tracker_->RemoveBuffer(buffer->id());
}

bool GLES2Implementation::GetBoundPixelTransferBuffer(
    GLenum target,
    const char* function_name,
    GLuint* buffer_id) {
  *buffer_id = 0;

  switch (target) {
    case GL_PIXEL_PACK_TRANSFER_BUFFER_CHROMIUM:
      *buffer_id = bound_pixel_pack_transfer_buffer_id_;
      break;
    case GL_PIXEL_UNPACK_TRANSFER_BUFFER_CHROMIUM:
      *buffer_id = bound_pixel_unpack_transfer_buffer_id_;
      break;
    default:
      // Unknown target
      return false;
  }
  if (!*buffer_id) {
    SetGLError(GL_INVALID_OPERATION, function_name, "no buffer bound");
  }
  return true;
}

BufferTracker::Buffer*
GLES2Implementation::GetBoundPixelUnpackTransferBufferIfValid(
    GLuint buffer_id,
    const char* function_name,
    GLuint offset, GLsizei size)
{
  DCHECK(buffer_id);
  BufferTracker::Buffer* buffer = buffer_tracker_->GetBuffer(buffer_id);
  if (!buffer) {
    SetGLError(GL_INVALID_OPERATION, function_name, "invalid buffer");
    return NULL;
  }
  if (buffer->mapped()) {
    SetGLError(GL_INVALID_OPERATION, function_name, "buffer mapped");
    return NULL;
  }
  if ((buffer->size() - offset) < static_cast<GLuint>(size)) {
    SetGLError(GL_INVALID_VALUE, function_name, "unpack size to large");
    return NULL;
  }
  return buffer;
}

void GLES2Implementation::CompressedTexImage2D(
    GLenum target, GLint level, GLenum internalformat, GLsizei width,
    GLsizei height, GLint border, GLsizei image_size, const void* data) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCompressedTexImage2D("
      << GLES2Util::GetStringTextureTarget(target) << ", "
      << level << ", "
      << GLES2Util::GetStringCompressedTextureFormat(internalformat) << ", "
      << width << ", " << height << ", " << border << ", "
      << image_size << ", "
      << static_cast<const void*>(data) << ")");
  if (width < 0 || height < 0 || level < 0) {
    SetGLError(GL_INVALID_VALUE, "glCompressedTexImage2D", "dimension < 0");
    return;
  }
  if (border != 0) {
    SetGLError(GL_INVALID_VALUE, "glCompressedTexImage2D", "border != 0");
    return;
  }
  if (height == 0 || width == 0) {
    return;
  }
  // If there's a pixel unpack buffer bound use it when issuing
  // CompressedTexImage2D.
  if (bound_pixel_unpack_transfer_buffer_id_) {
    GLuint offset = ToGLuint(data);
    BufferTracker::Buffer* buffer = GetBoundPixelUnpackTransferBufferIfValid(
        bound_pixel_unpack_transfer_buffer_id_,
        "glCompressedTexImage2D", offset, image_size);
    if (buffer && buffer->shm_id() != -1) {
      helper_->CompressedTexImage2D(
          target, level, internalformat, width, height, image_size,
          buffer->shm_id(), buffer->shm_offset() + offset);
      buffer->set_last_usage_token(helper_->InsertToken());
    }
    return;
  }
  SetBucketContents(kResultBucketId, data, image_size);
  helper_->CompressedTexImage2DBucket(
      target, level, internalformat, width, height, kResultBucketId);
  // Free the bucket. This is not required but it does free up the memory.
  // and we don't have to wait for the result so from the client's perspective
  // it's cheap.
  helper_->SetBucketSize(kResultBucketId, 0);
  CheckGLError();
}

void GLES2Implementation::CompressedTexSubImage2D(
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLsizei image_size, const void* data) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCompressedTexSubImage2D("
      << GLES2Util::GetStringTextureTarget(target) << ", "
      << level << ", "
      << xoffset << ", " << yoffset << ", "
      << width << ", " << height << ", "
      << GLES2Util::GetStringCompressedTextureFormat(format) << ", "
      << image_size << ", "
      << static_cast<const void*>(data) << ")");
  if (width < 0 || height < 0 || level < 0) {
    SetGLError(GL_INVALID_VALUE, "glCompressedTexSubImage2D", "dimension < 0");
    return;
  }
  // If there's a pixel unpack buffer bound use it when issuing
  // CompressedTexSubImage2D.
  if (bound_pixel_unpack_transfer_buffer_id_) {
    GLuint offset = ToGLuint(data);
    BufferTracker::Buffer* buffer = GetBoundPixelUnpackTransferBufferIfValid(
        bound_pixel_unpack_transfer_buffer_id_,
        "glCompressedTexSubImage2D", offset, image_size);
    if (buffer && buffer->shm_id() != -1) {
      helper_->CompressedTexSubImage2D(
          target, level, xoffset, yoffset, width, height, format, image_size,
          buffer->shm_id(), buffer->shm_offset() + offset);
      buffer->set_last_usage_token(helper_->InsertToken());
      CheckGLError();
    }
    return;
  }
  SetBucketContents(kResultBucketId, data, image_size);
  helper_->CompressedTexSubImage2DBucket(
      target, level, xoffset, yoffset, width, height, format, kResultBucketId);
  // Free the bucket. This is not required but it does free up the memory.
  // and we don't have to wait for the result so from the client's perspective
  // it's cheap.
  helper_->SetBucketSize(kResultBucketId, 0);
  CheckGLError();
}

namespace {

void CopyRectToBuffer(
    const void* pixels,
    uint32 height,
    uint32 unpadded_row_size,
    uint32 pixels_padded_row_size,
    bool flip_y,
    void* buffer,
    uint32 buffer_padded_row_size) {
  const int8* source = static_cast<const int8*>(pixels);
  int8* dest = static_cast<int8*>(buffer);
  if (flip_y || pixels_padded_row_size != buffer_padded_row_size) {
    if (flip_y) {
      dest += buffer_padded_row_size * (height - 1);
    }
    // the last row is copied unpadded at the end
    for (; height > 1; --height) {
      memcpy(dest, source, buffer_padded_row_size);
      if (flip_y) {
        dest -= buffer_padded_row_size;
      } else {
        dest += buffer_padded_row_size;
      }
      source += pixels_padded_row_size;
    }
    memcpy(dest, source, unpadded_row_size);
  } else {
    uint32 size = (height - 1) * pixels_padded_row_size + unpadded_row_size;
    memcpy(dest, source, size);
  }
}

}  // anonymous namespace

void GLES2Implementation::TexImage2D(
    GLenum target, GLint level, GLint internalformat, GLsizei width,
    GLsizei height, GLint border, GLenum format, GLenum type,
    const void* pixels) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexImage2D("
      << GLES2Util::GetStringTextureTarget(target) << ", "
      << level << ", "
      << GLES2Util::GetStringTextureInternalFormat(internalformat) << ", "
      << width << ", " << height << ", " << border << ", "
      << GLES2Util::GetStringTextureFormat(format) << ", "
      << GLES2Util::GetStringPixelType(type) << ", "
      << static_cast<const void*>(pixels) << ")");
  if (level < 0 || height < 0 || width < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexImage2D", "dimension < 0");
    return;
  }
  if (border != 0) {
    SetGLError(GL_INVALID_VALUE, "glTexImage2D", "border != 0");
    return;
  }
  uint32 size;
  uint32 unpadded_row_size;
  uint32 padded_row_size;
  if (!GLES2Util::ComputeImageDataSizes(
          width, height, format, type, unpack_alignment_, &size,
          &unpadded_row_size, &padded_row_size)) {
    SetGLError(GL_INVALID_VALUE, "glTexImage2D", "image size too large");
    return;
  }

  // If there's a pixel unpack buffer bound use it when issuing TexImage2D.
  if (bound_pixel_unpack_transfer_buffer_id_) {
    GLuint offset = ToGLuint(pixels);
    BufferTracker::Buffer* buffer = GetBoundPixelUnpackTransferBufferIfValid(
        bound_pixel_unpack_transfer_buffer_id_,
        "glTexImage2D", offset, size);
    if (buffer && buffer->shm_id() != -1) {
      helper_->TexImage2D(
          target, level, internalformat, width, height, format, type,
          buffer->shm_id(), buffer->shm_offset() + offset);
      buffer->set_last_usage_token(helper_->InsertToken());
      CheckGLError();
    }
    return;
  }

  // If there's no data just issue TexImage2D
  if (!pixels) {
    helper_->TexImage2D(
       target, level, internalformat, width, height, format, type,
       0, 0);
    CheckGLError();
    return;
  }

  // compute the advance bytes per row for the src pixels
  uint32 src_padded_row_size;
  if (unpack_row_length_ > 0) {
    if (!GLES2Util::ComputeImagePaddedRowSize(
        unpack_row_length_, format, type, unpack_alignment_,
        &src_padded_row_size)) {
      SetGLError(
          GL_INVALID_VALUE, "glTexImage2D", "unpack row length too large");
      return;
    }
  } else {
    src_padded_row_size = padded_row_size;
  }

  // advance pixels pointer past the skip rows and skip pixels
  pixels = reinterpret_cast<const int8*>(pixels) +
      unpack_skip_rows_ * src_padded_row_size;
  if (unpack_skip_pixels_) {
    uint32 group_size = GLES2Util::ComputeImageGroupSize(format, type);
    pixels = reinterpret_cast<const int8*>(pixels) +
        unpack_skip_pixels_ * group_size;
  }

  // Check if we can send it all at once.
  ScopedTransferBufferPtr buffer(size, helper_, transfer_buffer_);
  if (!buffer.valid()) {
    return;
  }

  if (buffer.size() >= size) {
    CopyRectToBuffer(
        pixels, height, unpadded_row_size, src_padded_row_size, unpack_flip_y_,
        buffer.address(), padded_row_size);
    helper_->TexImage2D(
        target, level, internalformat, width, height, format, type,
        buffer.shm_id(), buffer.offset());
    CheckGLError();
    return;
  }

  // No, so send it using TexSubImage2D.
  helper_->TexImage2D(
     target, level, internalformat, width, height, format, type,
     0, 0);
  TexSubImage2DImpl(
      target, level, 0, 0, width, height, format, type, unpadded_row_size,
      pixels, src_padded_row_size, GL_TRUE, &buffer, padded_row_size);
  CheckGLError();
}

void GLES2Implementation::TexSubImage2D(
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLenum type, const void* pixels) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexSubImage2D("
      << GLES2Util::GetStringTextureTarget(target) << ", "
      << level << ", "
      << xoffset << ", " << yoffset << ", "
      << width << ", " << height << ", "
      << GLES2Util::GetStringTextureFormat(format) << ", "
      << GLES2Util::GetStringPixelType(type) << ", "
      << static_cast<const void*>(pixels) << ")");

  if (level < 0 || height < 0 || width < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexSubImage2D", "dimension < 0");
    return;
  }
  if (height == 0 || width == 0) {
    return;
  }

  uint32 temp_size;
  uint32 unpadded_row_size;
  uint32 padded_row_size;
  if (!GLES2Util::ComputeImageDataSizes(
        width, height, format, type, unpack_alignment_, &temp_size,
        &unpadded_row_size, &padded_row_size)) {
    SetGLError(GL_INVALID_VALUE, "glTexSubImage2D", "size to large");
    return;
  }

  // If there's a pixel unpack buffer bound use it when issuing TexSubImage2D.
  if (bound_pixel_unpack_transfer_buffer_id_) {
    GLuint offset = ToGLuint(pixels);
    BufferTracker::Buffer* buffer = GetBoundPixelUnpackTransferBufferIfValid(
        bound_pixel_unpack_transfer_buffer_id_,
        "glTexSubImage2D", offset, temp_size);
    if (buffer && buffer->shm_id() != -1) {
      helper_->TexSubImage2D(
          target, level, xoffset, yoffset, width, height, format, type,
          buffer->shm_id(), buffer->shm_offset() + offset, false);
      buffer->set_last_usage_token(helper_->InsertToken());
      CheckGLError();
    }
    return;
  }

  // compute the advance bytes per row for the src pixels
  uint32 src_padded_row_size;
  if (unpack_row_length_ > 0) {
    if (!GLES2Util::ComputeImagePaddedRowSize(
        unpack_row_length_, format, type, unpack_alignment_,
        &src_padded_row_size)) {
      SetGLError(
          GL_INVALID_VALUE, "glTexImage2D", "unpack row length too large");
      return;
    }
  } else {
    src_padded_row_size = padded_row_size;
  }

  // advance pixels pointer past the skip rows and skip pixels
  pixels = reinterpret_cast<const int8*>(pixels) +
      unpack_skip_rows_ * src_padded_row_size;
  if (unpack_skip_pixels_) {
    uint32 group_size = GLES2Util::ComputeImageGroupSize(format, type);
    pixels = reinterpret_cast<const int8*>(pixels) +
        unpack_skip_pixels_ * group_size;
  }

  ScopedTransferBufferPtr buffer(temp_size, helper_, transfer_buffer_);
  TexSubImage2DImpl(
      target, level, xoffset, yoffset, width, height, format, type,
      unpadded_row_size, pixels, src_padded_row_size, GL_FALSE, &buffer,
      padded_row_size);
  CheckGLError();
}

static GLint ComputeNumRowsThatFitInBuffer(
    uint32 padded_row_size, uint32 unpadded_row_size,
    unsigned int size) {
  DCHECK_GE(unpadded_row_size, 0u);
  if (padded_row_size == 0) {
    return 1;
  }
  GLint num_rows = size / padded_row_size;
  return num_rows + (size - num_rows * padded_row_size) / unpadded_row_size;
}

void GLES2Implementation::TexSubImage2DImpl(
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLenum type, uint32 unpadded_row_size,
    const void* pixels, uint32 pixels_padded_row_size, GLboolean internal,
    ScopedTransferBufferPtr* buffer, uint32 buffer_padded_row_size) {
  DCHECK(buffer);
  DCHECK_GE(level, 0);
  DCHECK_GT(height, 0);
  DCHECK_GT(width, 0);

  const int8* source = reinterpret_cast<const int8*>(pixels);
  GLint original_yoffset = yoffset;
  // Transfer by rows.
  while (height) {
    unsigned int desired_size =
        buffer_padded_row_size * (height - 1) + unpadded_row_size;
    if (!buffer->valid() || buffer->size() == 0) {
      buffer->Reset(desired_size);
      if (!buffer->valid()) {
        return;
      }
    }

    GLint num_rows = ComputeNumRowsThatFitInBuffer(
        buffer_padded_row_size, unpadded_row_size, buffer->size());
    num_rows = std::min(num_rows, height);
    CopyRectToBuffer(
        source, num_rows, unpadded_row_size, pixels_padded_row_size,
        unpack_flip_y_, buffer->address(), buffer_padded_row_size);
    GLint y = unpack_flip_y_ ? original_yoffset + height - num_rows : yoffset;
    helper_->TexSubImage2D(
        target, level, xoffset, y, width, num_rows, format, type,
        buffer->shm_id(), buffer->offset(), internal);
    buffer->Release();
    yoffset += num_rows;
    source += num_rows * pixels_padded_row_size;
    height -= num_rows;
  }
}

bool GLES2Implementation::GetActiveAttribHelper(
    GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size,
    GLenum* type, char* name) {
  // Clear the bucket so if the command fails nothing will be in it.
  helper_->SetBucketSize(kResultBucketId, 0);
  typedef cmds::GetActiveAttrib::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return false;
  }
  // Set as failed so if the command fails we'll recover.
  result->success = false;
  helper_->GetActiveAttrib(program, index, kResultBucketId,
                           GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  if (result->success) {
    if (size) {
      *size = result->size;
    }
    if (type) {
      *type = result->type;
    }
    if (length || name) {
      std::vector<int8> str;
      GetBucketContents(kResultBucketId, &str);
      GLsizei max_size = std::min(static_cast<size_t>(bufsize) - 1,
                                  std::max(static_cast<size_t>(0),
                                           str.size() - 1));
      if (length) {
        *length = max_size;
      }
      if (name && bufsize > 0) {
        memcpy(name, &str[0], max_size);
        name[max_size] = '\0';
      }
    }
  }
  return result->success != 0;
}

void GLES2Implementation::GetActiveAttrib(
    GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size,
    GLenum* type, char* name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetActiveAttrib("
      << program << ", " << index << ", " << bufsize << ", "
      << static_cast<const void*>(length) << ", "
      << static_cast<const void*>(size) << ", "
      << static_cast<const void*>(type) << ", "
      << static_cast<const void*>(name) << ", ");
  if (bufsize < 0) {
    SetGLError(GL_INVALID_VALUE, "glGetActiveAttrib", "bufsize < 0");
    return;
  }
  TRACE_EVENT0("gpu", "GLES2::GetActiveAttrib");
  bool success = share_group_->program_info_manager()->GetActiveAttrib(
        this, program, index, bufsize, length, size, type, name);
  if (success) {
    if (size) {
      GPU_CLIENT_LOG("  size: " << *size);
    }
    if (type) {
      GPU_CLIENT_LOG("  type: " << GLES2Util::GetStringEnum(*type));
    }
    if (name) {
      GPU_CLIENT_LOG("  name: " << name);
    }
  }
  CheckGLError();
}

bool GLES2Implementation::GetActiveUniformHelper(
    GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size,
    GLenum* type, char* name) {
  // Clear the bucket so if the command fails nothing will be in it.
  helper_->SetBucketSize(kResultBucketId, 0);
  typedef cmds::GetActiveUniform::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return false;
  }
  // Set as failed so if the command fails we'll recover.
  result->success = false;
  helper_->GetActiveUniform(program, index, kResultBucketId,
                            GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  if (result->success) {
    if (size) {
      *size = result->size;
    }
    if (type) {
      *type = result->type;
    }
    if (length || name) {
      std::vector<int8> str;
      GetBucketContents(kResultBucketId, &str);
      GLsizei max_size = std::min(static_cast<size_t>(bufsize) - 1,
                                  std::max(static_cast<size_t>(0),
                                           str.size() - 1));
      if (length) {
        *length = max_size;
      }
      if (name && bufsize > 0) {
        memcpy(name, &str[0], max_size);
        name[max_size] = '\0';
      }
    }
  }
  return result->success != 0;
}

void GLES2Implementation::GetActiveUniform(
    GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size,
    GLenum* type, char* name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetActiveUniform("
      << program << ", " << index << ", " << bufsize << ", "
      << static_cast<const void*>(length) << ", "
      << static_cast<const void*>(size) << ", "
      << static_cast<const void*>(type) << ", "
      << static_cast<const void*>(name) << ", ");
  if (bufsize < 0) {
    SetGLError(GL_INVALID_VALUE, "glGetActiveUniform", "bufsize < 0");
    return;
  }
  TRACE_EVENT0("gpu", "GLES2::GetActiveUniform");
  bool success = share_group_->program_info_manager()->GetActiveUniform(
      this, program, index, bufsize, length, size, type, name);
  if (success) {
    if (size) {
      GPU_CLIENT_LOG("  size: " << *size);
    }
    if (type) {
      GPU_CLIENT_LOG("  type: " << GLES2Util::GetStringEnum(*type));
    }
    if (name) {
      GPU_CLIENT_LOG("  name: " << name);
    }
  }
  CheckGLError();
}

void GLES2Implementation::GetAttachedShaders(
    GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetAttachedShaders("
      << program << ", " << maxcount << ", "
      << static_cast<const void*>(count) << ", "
      << static_cast<const void*>(shaders) << ", ");
  if (maxcount < 0) {
    SetGLError(GL_INVALID_VALUE, "glGetAttachedShaders", "maxcount < 0");
    return;
  }
  TRACE_EVENT0("gpu", "GLES2::GetAttachedShaders");
  typedef cmds::GetAttachedShaders::Result Result;
  uint32 size = Result::ComputeSize(maxcount);
  Result* result = static_cast<Result*>(transfer_buffer_->Alloc(size));
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetAttachedShaders(
    program,
    transfer_buffer_->GetShmId(),
    transfer_buffer_->GetOffset(result),
    size);
  int32 token = helper_->InsertToken();
  WaitForCmd();
  if (count) {
    *count = result->GetNumResults();
  }
  result->CopyResult(shaders);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32 i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  transfer_buffer_->FreePendingToken(result, token);
  CheckGLError();
}

void GLES2Implementation::GetShaderPrecisionFormat(
    GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetShaderPrecisionFormat("
      << GLES2Util::GetStringShaderType(shadertype) << ", "
      << GLES2Util::GetStringShaderPrecision(precisiontype) << ", "
      << static_cast<const void*>(range) << ", "
      << static_cast<const void*>(precision) << ", ");
  TRACE_EVENT0("gpu", "GLES2::GetShaderPrecisionFormat");
  typedef cmds::GetShaderPrecisionFormat::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }

  GLStaticState::ShaderPrecisionKey key(shadertype, precisiontype);
  GLStaticState::ShaderPrecisionMap::iterator i =
      static_state_.shader_precisions.find(key);
  if (i != static_state_.shader_precisions.end()) {
    *result = i->second;
  } else {
    result->success = false;
    helper_->GetShaderPrecisionFormat(
        shadertype, precisiontype, GetResultShmId(), GetResultShmOffset());
    WaitForCmd();
    if (result->success)
      static_state_.shader_precisions[key] = *result;
  }

  if (result->success) {
    if (range) {
      range[0] = result->min_range;
      range[1] = result->max_range;
      GPU_CLIENT_LOG("  min_range: " << range[0]);
      GPU_CLIENT_LOG("  min_range: " << range[1]);
    }
    if (precision) {
      precision[0] = result->precision;
      GPU_CLIENT_LOG("  min_range: " << precision[0]);
    }
  }
  CheckGLError();
}

const GLubyte* GLES2Implementation::GetStringHelper(GLenum name) {
  const char* result = NULL;
  // Clears the bucket so if the command fails nothing will be in it.
  helper_->SetBucketSize(kResultBucketId, 0);
  helper_->GetString(name, kResultBucketId);
  std::string str;
  if (GetBucketAsString(kResultBucketId, &str)) {
    // Adds extensions implemented on client side only.
    switch (name) {
      case GL_EXTENSIONS:
        str += std::string(str.empty() ? "" : " ") +
            "GL_CHROMIUM_flipy "
            "GL_EXT_unpack_subimage "
            "GL_CHROMIUM_map_sub";
        if (capabilities_.map_image) {
          // The first space character is intentional.
          str += " GL_CHROMIUM_map_image";
        }
        break;
      default:
        break;
    }

    // Because of WebGL the extensions can change. We have to cache each unique
    // result since we don't know when the client will stop referring to a
    // previous one it queries.
    GLStringMap::iterator it = gl_strings_.find(name);
    if (it == gl_strings_.end()) {
      std::set<std::string> strings;
      std::pair<GLStringMap::iterator, bool> insert_result =
          gl_strings_.insert(std::make_pair(name, strings));
      DCHECK(insert_result.second);
      it = insert_result.first;
    }
    std::set<std::string>& string_set = it->second;
    std::set<std::string>::const_iterator sit = string_set.find(str);
    if (sit != string_set.end()) {
      result = sit->c_str();
    } else {
      std::pair<std::set<std::string>::const_iterator, bool> insert_result =
          string_set.insert(str);
      DCHECK(insert_result.second);
      result = insert_result.first->c_str();
    }
  }
  return reinterpret_cast<const GLubyte*>(result);
}

const GLubyte* GLES2Implementation::GetString(GLenum name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetString("
      << GLES2Util::GetStringStringType(name) << ")");
  TRACE_EVENT0("gpu", "GLES2::GetString");
  const GLubyte* result = GetStringHelper(name);
  GPU_CLIENT_LOG("  returned " << reinterpret_cast<const char*>(result));
  CheckGLError();
  return result;
}

void GLES2Implementation::GetUniformfv(
    GLuint program, GLint location, GLfloat* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetUniformfv("
      << program << ", " << location << ", "
      << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2::GetUniformfv");
  typedef cmds::GetUniformfv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetUniformfv(
      program, location, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32 i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::GetUniformiv(
    GLuint program, GLint location, GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetUniformiv("
      << program << ", " << location << ", "
      << static_cast<const void*>(params) << ")");
  TRACE_EVENT0("gpu", "GLES2::GetUniformiv");
  typedef cmds::GetUniformiv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetUniformiv(
      program, location, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  GetResultAs<cmds::GetUniformfv::Result*>()->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32 i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::ReadPixels(
    GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format,
    GLenum type, void* pixels) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glReadPixels("
      << xoffset << ", " << yoffset << ", "
      << width << ", " << height << ", "
      << GLES2Util::GetStringReadPixelFormat(format) << ", "
      << GLES2Util::GetStringPixelType(type) << ", "
      << static_cast<const void*>(pixels) << ")");
  if (width < 0 || height < 0) {
    SetGLError(GL_INVALID_VALUE, "glReadPixels", "dimensions < 0");
    return;
  }
  if (width == 0 || height == 0) {
    return;
  }

  // glReadPixel pads the size of each row of pixels by an amount specified by
  // glPixelStorei. So, we have to take that into account both in the fact that
  // the pixels returned from the ReadPixel command will include that padding
  // and that when we copy the results to the user's buffer we need to not
  // write those padding bytes but leave them as they are.

  TRACE_EVENT0("gpu", "GLES2::ReadPixels");
  typedef cmds::ReadPixels::Result Result;

  int8* dest = reinterpret_cast<int8*>(pixels);
  uint32 temp_size;
  uint32 unpadded_row_size;
  uint32 padded_row_size;
  if (!GLES2Util::ComputeImageDataSizes(
      width, 2, format, type, pack_alignment_, &temp_size, &unpadded_row_size,
      &padded_row_size)) {
    SetGLError(GL_INVALID_VALUE, "glReadPixels", "size too large.");
    return;
  }

  if (bound_pixel_pack_transfer_buffer_id_) {
    GLuint offset = ToGLuint(pixels);
    BufferTracker::Buffer* buffer = GetBoundPixelUnpackTransferBufferIfValid(
        bound_pixel_pack_transfer_buffer_id_,
        "glReadPixels", offset, padded_row_size * height);
    if (buffer && buffer->shm_id() != -1) {
      helper_->ReadPixels(xoffset, yoffset, width, height, format, type,
                          buffer->shm_id(), buffer->shm_offset(),
                          0, 0, true);
      CheckGLError();
    }
    return;
  }

  if (!pixels) {
    SetGLError(GL_INVALID_OPERATION, "glReadPixels", "pixels = NULL");
    return;
  }

  // Transfer by rows.
  // The max rows we can transfer.
  while (height) {
    GLsizei desired_size = padded_row_size * height - 1 + unpadded_row_size;
    ScopedTransferBufferPtr buffer(desired_size, helper_, transfer_buffer_);
    if (!buffer.valid()) {
      return;
    }
    GLint num_rows = ComputeNumRowsThatFitInBuffer(
        padded_row_size, unpadded_row_size, buffer.size());
    num_rows = std::min(num_rows, height);
    // NOTE: We must look up the address of the result area AFTER allocation
    // of the transfer buffer since the transfer buffer may be reallocated.
    Result* result = GetResultAs<Result*>();
    if (!result) {
      return;
    }
    *result = 0;  // mark as failed.
    helper_->ReadPixels(
        xoffset, yoffset, width, num_rows, format, type,
        buffer.shm_id(), buffer.offset(),
        GetResultShmId(), GetResultShmOffset(),
        false);
    WaitForCmd();
    if (*result != 0) {
      // when doing a y-flip we have to iterate through top-to-bottom chunks
      // of the dst. The service side handles reversing the rows within a
      // chunk.
      int8* rows_dst;
      if (pack_reverse_row_order_) {
          rows_dst = dest + (height - num_rows) * padded_row_size;
      } else {
          rows_dst = dest;
      }
      // We have to copy 1 row at a time to avoid writing pad bytes.
      const int8* src = static_cast<const int8*>(buffer.address());
      for (GLint yy = 0; yy < num_rows; ++yy) {
        memcpy(rows_dst, src, unpadded_row_size);
        rows_dst += padded_row_size;
        src += padded_row_size;
      }
      if (!pack_reverse_row_order_) {
        dest = rows_dst;
      }
    }
    // If it was not marked as successful exit.
    if (*result == 0) {
      return;
    }
    yoffset += num_rows;
    height -= num_rows;
  }
  CheckGLError();
}

void GLES2Implementation::ActiveTexture(GLenum texture) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glActiveTexture("
      << GLES2Util::GetStringEnum(texture) << ")");
  GLuint texture_index = texture - GL_TEXTURE0;
  if (texture_index >= static_cast<GLuint>(
      static_state_.int_state.max_combined_texture_image_units)) {
    SetGLErrorInvalidEnum(
        "glActiveTexture", texture, "texture");
    return;
  }

  active_texture_unit_ = texture_index;
  helper_->ActiveTexture(texture);
  CheckGLError();
}

void GLES2Implementation::GenBuffersHelper(
    GLsizei /* n */, const GLuint* /* buffers */) {
}

void GLES2Implementation::GenFramebuffersHelper(
    GLsizei /* n */, const GLuint* /* framebuffers */) {
}

void GLES2Implementation::GenRenderbuffersHelper(
    GLsizei /* n */, const GLuint* /* renderbuffers */) {
}

void GLES2Implementation::GenTexturesHelper(
    GLsizei /* n */, const GLuint* /* textures */) {
}

void GLES2Implementation::GenVertexArraysOESHelper(
    GLsizei n, const GLuint* arrays) {
  vertex_array_object_manager_->GenVertexArrays(n, arrays);
}

void GLES2Implementation::GenQueriesEXTHelper(
    GLsizei /* n */, const GLuint* /* queries */) {
}

// NOTE #1: On old versions of OpenGL, calling glBindXXX with an unused id
// generates a new resource. On newer versions of OpenGL they don't. The code
// related to binding below will need to change if we switch to the new OpenGL
// model. Specifically it assumes a bind will succeed which is always true in
// the old model but possibly not true in the new model if another context has
// deleted the resource.

bool GLES2Implementation::BindBufferHelper(
    GLenum target, GLuint buffer_id) {
  // TODO(gman): See note #1 above.
  bool changed = false;
  switch (target) {
    case GL_ARRAY_BUFFER:
      if (bound_array_buffer_id_ != buffer_id) {
        bound_array_buffer_id_ = buffer_id;
        changed = true;
      }
      break;
    case GL_ELEMENT_ARRAY_BUFFER:
      changed = vertex_array_object_manager_->BindElementArray(buffer_id);
      break;
    case GL_PIXEL_PACK_TRANSFER_BUFFER_CHROMIUM:
      bound_pixel_pack_transfer_buffer_id_ = buffer_id;
      break;
    case GL_PIXEL_UNPACK_TRANSFER_BUFFER_CHROMIUM:
      bound_pixel_unpack_transfer_buffer_id_ = buffer_id;
      break;
    default:
      changed = true;
      break;
  }
  // TODO(gman): There's a bug here. If the target is invalid the ID will not be
  // used even though it's marked it as used here.
  GetIdHandler(id_namespaces::kBuffers)->MarkAsUsedForBind(buffer_id);
  return changed;
}

bool GLES2Implementation::BindFramebufferHelper(
    GLenum target, GLuint framebuffer) {
  // TODO(gman): See note #1 above.
  bool changed = false;
  switch (target) {
    case GL_FRAMEBUFFER:
      if (bound_framebuffer_ != framebuffer ||
          bound_read_framebuffer_ != framebuffer) {
        bound_framebuffer_ = framebuffer;
        bound_read_framebuffer_ = framebuffer;
        changed = true;
      }
      break;
    case GL_READ_FRAMEBUFFER:
      if (!IsChromiumFramebufferMultisampleAvailable()) {
        SetGLErrorInvalidEnum("glBindFramebuffer", target, "target");
        return false;
      }
      if (bound_read_framebuffer_ != framebuffer) {
        bound_read_framebuffer_ = framebuffer;
        changed = true;
      }
      break;
    case GL_DRAW_FRAMEBUFFER:
      if (!IsChromiumFramebufferMultisampleAvailable()) {
        SetGLErrorInvalidEnum("glBindFramebuffer", target, "target");
        return false;
      }
      if (bound_framebuffer_ != framebuffer) {
        bound_framebuffer_ = framebuffer;
        changed = true;
      }
      break;
    default:
      SetGLErrorInvalidEnum("glBindFramebuffer", target, "target");
      return false;
  }
  GetIdHandler(id_namespaces::kFramebuffers)->MarkAsUsedForBind(framebuffer);
  return changed;
}

bool GLES2Implementation::BindRenderbufferHelper(
    GLenum target, GLuint renderbuffer) {
  // TODO(gman): See note #1 above.
  bool changed = false;
  switch (target) {
    case GL_RENDERBUFFER:
      if (bound_renderbuffer_ != renderbuffer) {
        bound_renderbuffer_ = renderbuffer;
        changed = true;
      }
      break;
    default:
      changed = true;
      break;
  }
  // TODO(gman): There's a bug here. If the target is invalid the ID will not be
  // used even though it's marked it as used here.
  GetIdHandler(id_namespaces::kRenderbuffers)->MarkAsUsedForBind(renderbuffer);
  return changed;
}

bool GLES2Implementation::BindTextureHelper(GLenum target, GLuint texture) {
  // TODO(gman): See note #1 above.
  // TODO(gman): Change this to false once we figure out why it's failing
  //     on daisy.
  bool changed = true;
  TextureUnit& unit = texture_units_[active_texture_unit_];
  switch (target) {
    case GL_TEXTURE_2D:
      if (unit.bound_texture_2d != texture) {
        unit.bound_texture_2d = texture;
        changed = true;
      }
      break;
    case GL_TEXTURE_CUBE_MAP:
      if (unit.bound_texture_cube_map != texture) {
        unit.bound_texture_cube_map = texture;
        changed = true;
      }
      break;
    case GL_TEXTURE_EXTERNAL_OES:
      if (unit.bound_texture_external_oes != texture) {
        unit.bound_texture_external_oes = texture;
        changed = true;
      }
      break;
    default:
      changed = true;
      break;
  }
  // TODO(gman): There's a bug here. If the target is invalid the ID will not be
  // used. even though it's marked it as used here.
  GetIdHandler(id_namespaces::kTextures)->MarkAsUsedForBind(texture);
  return changed;
}

bool GLES2Implementation::BindVertexArrayOESHelper(GLuint array) {
  // TODO(gman): See note #1 above.
  bool changed = false;
  if (!vertex_array_object_manager_->BindVertexArray(array, &changed)) {
    SetGLError(
        GL_INVALID_OPERATION, "glBindVertexArrayOES",
        "id was not generated with glGenVertexArrayOES");
  }
  // Unlike other BindXXXHelpers we don't call MarkAsUsedForBind
  // because unlike other resources VertexArrayObject ids must
  // be generated by GenVertexArrays. A random id to Bind will not
  // generate a new object.
  return changed;
}

bool GLES2Implementation::UseProgramHelper(GLuint program) {
  bool changed = false;
  if (current_program_ != program) {
    current_program_ = program;
    changed = true;
  }
  return changed;
}

bool GLES2Implementation::IsBufferReservedId(GLuint id) {
  return vertex_array_object_manager_->IsReservedId(id);
}

void GLES2Implementation::DeleteBuffersHelper(
    GLsizei n, const GLuint* buffers) {
  if (!GetIdHandler(id_namespaces::kBuffers)->FreeIds(
      this, n, buffers, &GLES2Implementation::DeleteBuffersStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteBuffers", "id not created by this context.");
    return;
  }
  for (GLsizei ii = 0; ii < n; ++ii) {
    if (buffers[ii] == bound_array_buffer_id_) {
      bound_array_buffer_id_ = 0;
    }
    vertex_array_object_manager_->UnbindBuffer(buffers[ii]);

    BufferTracker::Buffer* buffer = buffer_tracker_->GetBuffer(buffers[ii]);
    if (buffer)
      RemoveTransferBuffer(buffer);

    if (buffers[ii] == bound_pixel_unpack_transfer_buffer_id_) {
      bound_pixel_unpack_transfer_buffer_id_ = 0;
    }
  }
}

void GLES2Implementation::DeleteBuffersStub(
    GLsizei n, const GLuint* buffers) {
  helper_->DeleteBuffersImmediate(n, buffers);
}


void GLES2Implementation::DeleteFramebuffersHelper(
    GLsizei n, const GLuint* framebuffers) {
  if (!GetIdHandler(id_namespaces::kFramebuffers)->FreeIds(
      this, n, framebuffers, &GLES2Implementation::DeleteFramebuffersStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteFramebuffers", "id not created by this context.");
    return;
  }
  for (GLsizei ii = 0; ii < n; ++ii) {
    if (framebuffers[ii] == bound_framebuffer_) {
      bound_framebuffer_ = 0;
    }
    if (framebuffers[ii] == bound_read_framebuffer_) {
      bound_read_framebuffer_ = 0;
    }
  }
}

void GLES2Implementation::DeleteFramebuffersStub(
    GLsizei n, const GLuint* framebuffers) {
  helper_->DeleteFramebuffersImmediate(n, framebuffers);
}

void GLES2Implementation::DeleteRenderbuffersHelper(
    GLsizei n, const GLuint* renderbuffers) {
  if (!GetIdHandler(id_namespaces::kRenderbuffers)->FreeIds(
      this, n, renderbuffers, &GLES2Implementation::DeleteRenderbuffersStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteRenderbuffers", "id not created by this context.");
    return;
  }
  for (GLsizei ii = 0; ii < n; ++ii) {
    if (renderbuffers[ii] == bound_renderbuffer_) {
      bound_renderbuffer_ = 0;
    }
  }
}

void GLES2Implementation::DeleteRenderbuffersStub(
    GLsizei n, const GLuint* renderbuffers) {
  helper_->DeleteRenderbuffersImmediate(n, renderbuffers);
}

void GLES2Implementation::DeleteTexturesHelper(
    GLsizei n, const GLuint* textures) {
  if (!GetIdHandler(id_namespaces::kTextures)->FreeIds(
      this, n, textures, &GLES2Implementation::DeleteTexturesStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteTextures", "id not created by this context.");
    return;
  }
  for (GLsizei ii = 0; ii < n; ++ii) {
    for (GLint tt = 0;
         tt < static_state_.int_state.max_combined_texture_image_units;
         ++tt) {
      TextureUnit& unit = texture_units_[tt];
      if (textures[ii] == unit.bound_texture_2d) {
        unit.bound_texture_2d = 0;
      }
      if (textures[ii] == unit.bound_texture_cube_map) {
        unit.bound_texture_cube_map = 0;
      }
      if (textures[ii] == unit.bound_texture_external_oes) {
        unit.bound_texture_external_oes = 0;
      }
    }
  }
}

void GLES2Implementation::DeleteVertexArraysOESHelper(
    GLsizei n, const GLuint* arrays) {
  vertex_array_object_manager_->DeleteVertexArrays(n, arrays);
  if (!GetIdHandler(id_namespaces::kVertexArrays)->FreeIds(
      this, n, arrays, &GLES2Implementation::DeleteVertexArraysOESStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteVertexArraysOES", "id not created by this context.");
    return;
  }
}

void GLES2Implementation::DeleteVertexArraysOESStub(
    GLsizei n, const GLuint* arrays) {
  helper_->DeleteVertexArraysOESImmediate(n, arrays);
}

void GLES2Implementation::DeleteTexturesStub(
    GLsizei n, const GLuint* textures) {
  helper_->DeleteTexturesImmediate(n, textures);
}

void GLES2Implementation::DisableVertexAttribArray(GLuint index) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glDisableVertexAttribArray(" << index << ")");
  vertex_array_object_manager_->SetAttribEnable(index, false);
  helper_->DisableVertexAttribArray(index);
  CheckGLError();
}

void GLES2Implementation::EnableVertexAttribArray(GLuint index) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glEnableVertexAttribArray("
      << index << ")");
  vertex_array_object_manager_->SetAttribEnable(index, true);
  helper_->EnableVertexAttribArray(index);
  CheckGLError();
}

void GLES2Implementation::DrawArrays(GLenum mode, GLint first, GLsizei count) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDrawArrays("
      << GLES2Util::GetStringDrawMode(mode) << ", "
      << first << ", " << count << ")");
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawArrays", "count < 0");
    return;
  }
  bool simulated = false;
  if (!vertex_array_object_manager_->SetupSimulatedClientSideBuffers(
      "glDrawArrays", this, helper_, first + count, 0, &simulated)) {
    return;
  }
  helper_->DrawArrays(mode, first, count);
  RestoreArrayBuffer(simulated);
  CheckGLError();
}

void GLES2Implementation::GetVertexAttribfv(
    GLuint index, GLenum pname, GLfloat* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetVertexAttribfv("
      << index << ", "
      << GLES2Util::GetStringVertexAttribute(pname) << ", "
      << static_cast<const void*>(params) << ")");
  uint32 value = 0;
  if (vertex_array_object_manager_->GetVertexAttrib(index, pname, &value)) {
    *params = static_cast<float>(value);
    return;
  }
  TRACE_EVENT0("gpu", "GLES2::GetVertexAttribfv");
  typedef cmds::GetVertexAttribfv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetVertexAttribfv(
      index, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32 i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::GetVertexAttribiv(
    GLuint index, GLenum pname, GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetVertexAttribiv("
      << index << ", "
      << GLES2Util::GetStringVertexAttribute(pname) << ", "
      << static_cast<const void*>(params) << ")");
  uint32 value = 0;
  if (vertex_array_object_manager_->GetVertexAttrib(index, pname, &value)) {
    *params = value;
    return;
  }
  TRACE_EVENT0("gpu", "GLES2::GetVertexAttribiv");
  typedef cmds::GetVertexAttribiv::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return;
  }
  result->SetNumResults(0);
  helper_->GetVertexAttribiv(
      index, pname, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  result->CopyResult(params);
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int32 i = 0; i < result->GetNumResults(); ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << result->GetData()[i]);
    }
  });
  CheckGLError();
}

void GLES2Implementation::Swap() {
  SwapBuffers();
  gpu_control_->Echo(
      base::Bind(&GLES2Implementation::OnSwapBuffersComplete,
                 weak_ptr_factory_.GetWeakPtr()));
}

void GLES2Implementation::PartialSwapBuffers(const gfx::Rect& sub_buffer) {
  PostSubBufferCHROMIUM(sub_buffer.x(),
                        sub_buffer.y(),
                        sub_buffer.width(),
                        sub_buffer.height());
  gpu_control_->Echo(base::Bind(&GLES2Implementation::OnSwapBuffersComplete,
                                weak_ptr_factory_.GetWeakPtr()));
}

void GLES2Implementation::SetSwapBuffersCompleteCallback(
      const base::Closure& swap_buffers_complete_callback) {
  swap_buffers_complete_callback_ = swap_buffers_complete_callback;
}

static GLenum GetGLESOverlayTransform(gfx::OverlayTransform plane_transform) {
  switch (plane_transform) {
    case gfx::OVERLAY_TRANSFORM_INVALID:
      break;
    case gfx::OVERLAY_TRANSFORM_NONE:
      return GL_OVERLAY_TRANSFORM_NONE_CHROMIUM;
    case gfx::OVERLAY_TRANSFORM_FLIP_HORIZONTAL:
      return GL_OVERLAY_TRANSFORM_FLIP_HORIZONTAL_CHROMIUM;
    case gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL:
      return GL_OVERLAY_TRANSFORM_FLIP_VERTICAL_CHROMIUM;
    case gfx::OVERLAY_TRANSFORM_ROTATE_90:
      return GL_OVERLAY_TRANSFORM_ROTATE_90_CHROMIUM;
    case gfx::OVERLAY_TRANSFORM_ROTATE_180:
      return GL_OVERLAY_TRANSFORM_ROTATE_180_CHROMIUM;
    case gfx::OVERLAY_TRANSFORM_ROTATE_270:
      return GL_OVERLAY_TRANSFORM_ROTATE_270_CHROMIUM;
  }
  NOTREACHED();
  return GL_OVERLAY_TRANSFORM_NONE_CHROMIUM;
}

void GLES2Implementation::ScheduleOverlayPlane(
    int plane_z_order,
    gfx::OverlayTransform plane_transform,
    unsigned overlay_texture_id,
    const gfx::Rect& display_bounds,
    const gfx::RectF& uv_rect) {
  ScheduleOverlayPlaneCHROMIUM(plane_z_order,
                               GetGLESOverlayTransform(plane_transform),
                               overlay_texture_id,
                               display_bounds.x(),
                               display_bounds.y(),
                               display_bounds.width(),
                               display_bounds.height(),
                               uv_rect.x(),
                               uv_rect.y(),
                               uv_rect.width(),
                               uv_rect.height());
}

void GLES2Implementation::OnSwapBuffersComplete() {
  if (!swap_buffers_complete_callback_.is_null())
    swap_buffers_complete_callback_.Run();
}

GLboolean GLES2Implementation::EnableFeatureCHROMIUM(
    const char* feature) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glEnableFeatureCHROMIUM("
                 << feature << ")");
  TRACE_EVENT0("gpu", "GLES2::EnableFeatureCHROMIUM");
  typedef cmds::EnableFeatureCHROMIUM::Result Result;
  Result* result = GetResultAs<Result*>();
  if (!result) {
    return false;
  }
  *result = 0;
  SetBucketAsCString(kResultBucketId, feature);
  helper_->EnableFeatureCHROMIUM(
      kResultBucketId, GetResultShmId(), GetResultShmOffset());
  WaitForCmd();
  helper_->SetBucketSize(kResultBucketId, 0);
  GPU_CLIENT_LOG("   returned " << GLES2Util::GetStringBool(*result));
  return *result;
}

void* GLES2Implementation::MapBufferSubDataCHROMIUM(
    GLuint target, GLintptr offset, GLsizeiptr size, GLenum access) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glMapBufferSubDataCHROMIUM("
      << target << ", " << offset << ", " << size << ", "
      << GLES2Util::GetStringEnum(access) << ")");
  // NOTE: target is NOT checked because the service will check it
  // and we don't know what targets are valid.
  if (access != GL_WRITE_ONLY) {
    SetGLErrorInvalidEnum(
        "glMapBufferSubDataCHROMIUM", access, "access");
    return NULL;
  }
  if (!ValidateSize("glMapBufferSubDataCHROMIUM", size) ||
      !ValidateOffset("glMapBufferSubDataCHROMIUM", offset)) {
    return NULL;
  }

  int32 shm_id;
  unsigned int shm_offset;
  void* mem = mapped_memory_->Alloc(size, &shm_id, &shm_offset);
  if (!mem) {
    SetGLError(GL_OUT_OF_MEMORY, "glMapBufferSubDataCHROMIUM", "out of memory");
    return NULL;
  }

  std::pair<MappedBufferMap::iterator, bool> result =
     mapped_buffers_.insert(std::make_pair(
         mem,
         MappedBuffer(
             access, shm_id, mem, shm_offset, target, offset, size)));
  DCHECK(result.second);
  GPU_CLIENT_LOG("  returned " << mem);
  return mem;
}

void GLES2Implementation::UnmapBufferSubDataCHROMIUM(const void* mem) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glUnmapBufferSubDataCHROMIUM(" << mem << ")");
  MappedBufferMap::iterator it = mapped_buffers_.find(mem);
  if (it == mapped_buffers_.end()) {
    SetGLError(
        GL_INVALID_VALUE, "UnmapBufferSubDataCHROMIUM", "buffer not mapped");
    return;
  }
  const MappedBuffer& mb = it->second;
  helper_->BufferSubData(
      mb.target, mb.offset, mb.size, mb.shm_id, mb.shm_offset);
  mapped_memory_->FreePendingToken(mb.shm_memory, helper_->InsertToken());
  mapped_buffers_.erase(it);
  CheckGLError();
}

void* GLES2Implementation::MapTexSubImage2DCHROMIUM(
     GLenum target,
     GLint level,
     GLint xoffset,
     GLint yoffset,
     GLsizei width,
     GLsizei height,
     GLenum format,
     GLenum type,
     GLenum access) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glMapTexSubImage2DCHROMIUM("
      << target << ", " << level << ", "
      << xoffset << ", " << yoffset << ", "
      << width << ", " << height << ", "
      << GLES2Util::GetStringTextureFormat(format) << ", "
      << GLES2Util::GetStringPixelType(type) << ", "
      << GLES2Util::GetStringEnum(access) << ")");
  if (access != GL_WRITE_ONLY) {
    SetGLErrorInvalidEnum(
        "glMapTexSubImage2DCHROMIUM", access, "access");
    return NULL;
  }
  // NOTE: target is NOT checked because the service will check it
  // and we don't know what targets are valid.
  if (level < 0 || xoffset < 0 || yoffset < 0 || width < 0 || height < 0) {
    SetGLError(
        GL_INVALID_VALUE, "glMapTexSubImage2DCHROMIUM", "bad dimensions");
    return NULL;
  }
  uint32 size;
  if (!GLES2Util::ComputeImageDataSizes(
      width, height, format, type, unpack_alignment_, &size, NULL, NULL)) {
    SetGLError(
        GL_INVALID_VALUE, "glMapTexSubImage2DCHROMIUM", "image size too large");
    return NULL;
  }
  int32 shm_id;
  unsigned int shm_offset;
  void* mem = mapped_memory_->Alloc(size, &shm_id, &shm_offset);
  if (!mem) {
    SetGLError(GL_OUT_OF_MEMORY, "glMapTexSubImage2DCHROMIUM", "out of memory");
    return NULL;
  }

  std::pair<MappedTextureMap::iterator, bool> result =
     mapped_textures_.insert(std::make_pair(
         mem,
         MappedTexture(
             access, shm_id, mem, shm_offset,
             target, level, xoffset, yoffset, width, height, format, type)));
  DCHECK(result.second);
  GPU_CLIENT_LOG("  returned " << mem);
  return mem;
}

void GLES2Implementation::UnmapTexSubImage2DCHROMIUM(const void* mem) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glUnmapTexSubImage2DCHROMIUM(" << mem << ")");
  MappedTextureMap::iterator it = mapped_textures_.find(mem);
  if (it == mapped_textures_.end()) {
    SetGLError(
        GL_INVALID_VALUE, "UnmapTexSubImage2DCHROMIUM", "texture not mapped");
    return;
  }
  const MappedTexture& mt = it->second;
  helper_->TexSubImage2D(
      mt.target, mt.level, mt.xoffset, mt.yoffset, mt.width, mt.height,
      mt.format, mt.type, mt.shm_id, mt.shm_offset, GL_FALSE);
  mapped_memory_->FreePendingToken(mt.shm_memory, helper_->InsertToken());
  mapped_textures_.erase(it);
  CheckGLError();
}

void GLES2Implementation::ResizeCHROMIUM(GLuint width, GLuint height,
                                         float scale_factor) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glResizeCHROMIUM("
                 << width << ", " << height << ", " << scale_factor << ")");
  helper_->ResizeCHROMIUM(width, height, scale_factor);
  CheckGLError();
}

const GLchar* GLES2Implementation::GetRequestableExtensionsCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix()
      << "] glGetRequestableExtensionsCHROMIUM()");
  TRACE_EVENT0("gpu",
               "GLES2Implementation::GetRequestableExtensionsCHROMIUM()");
  const char* result = NULL;
  // Clear the bucket so if the command fails nothing will be in it.
  helper_->SetBucketSize(kResultBucketId, 0);
  helper_->GetRequestableExtensionsCHROMIUM(kResultBucketId);
  std::string str;
  if (GetBucketAsString(kResultBucketId, &str)) {
    // The set of requestable extensions shrinks as we enable
    // them. Because we don't know when the client will stop referring
    // to a previous one it queries (see GetString) we need to cache
    // the unique results.
    std::set<std::string>::const_iterator sit =
        requestable_extensions_set_.find(str);
    if (sit != requestable_extensions_set_.end()) {
      result = sit->c_str();
    } else {
      std::pair<std::set<std::string>::const_iterator, bool> insert_result =
          requestable_extensions_set_.insert(str);
      DCHECK(insert_result.second);
      result = insert_result.first->c_str();
    }
  }
  GPU_CLIENT_LOG("  returned " << result);
  return reinterpret_cast<const GLchar*>(result);
}

// TODO(gman): Remove this command. It's here for WebGL but is incompatible
// with VirtualGL contexts.
void GLES2Implementation::RequestExtensionCHROMIUM(const char* extension) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glRequestExtensionCHROMIUM("
                 << extension << ")");
  SetBucketAsCString(kResultBucketId, extension);
  helper_->RequestExtensionCHROMIUM(kResultBucketId);
  helper_->SetBucketSize(kResultBucketId, 0);

  struct ExtensionCheck {
    const char* extension;
    ExtensionStatus* status;
  };
  const ExtensionCheck checks[] = {
    {
      "GL_ANGLE_pack_reverse_row_order",
      &angle_pack_reverse_row_order_status_,
    },
    {
      "GL_CHROMIUM_framebuffer_multisample",
       &chromium_framebuffer_multisample_,
    },
  };
  const size_t kNumChecks = sizeof(checks)/sizeof(checks[0]);
  for (size_t ii = 0; ii < kNumChecks; ++ii) {
    const ExtensionCheck& check = checks[ii];
    if (*check.status == kUnavailableExtensionStatus &&
        !strcmp(extension, check.extension)) {
      *check.status = kUnknownExtensionStatus;
    }
  }
}

void GLES2Implementation::RateLimitOffscreenContextCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glRateLimitOffscreenCHROMIUM()");
  // Wait if this would add too many rate limit tokens.
  if (rate_limit_tokens_.size() == kMaxSwapBuffers) {
    helper_->WaitForToken(rate_limit_tokens_.front());
    rate_limit_tokens_.pop();
  }
  rate_limit_tokens_.push(helper_->InsertToken());
}

void GLES2Implementation::GetMultipleIntegervCHROMIUM(
    const GLenum* pnames, GLuint count, GLint* results, GLsizeiptr size) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGetMultipleIntegervCHROMIUM("
                 << static_cast<const void*>(pnames) << ", "
                 << count << ", " << results << ", "
                 << size << ")");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (GLuint i = 0; i < count; ++i) {
      GPU_CLIENT_LOG(
          "  " << i << ": " << GLES2Util::GetStringGLState(pnames[i]));
    }
  });
  DCHECK(size >= 0 && FitInt32NonNegative<GLsizeiptr>(size));

  GetMultipleIntegervState state(pnames, count, results, size);
  if (!GetMultipleIntegervSetup(&state)) {
    return;
  }
  state.buffer = transfer_buffer_->Alloc(state.transfer_buffer_size_needed);
  if (!state.buffer) {
    SetGLError(GL_OUT_OF_MEMORY, "glGetMultipleIntegervCHROMIUM",
               "Transfer buffer allocation failed.");
    return;
  }
  GetMultipleIntegervRequest(&state);
  WaitForCmd();
  GetMultipleIntegervOnCompleted(&state);

  GPU_CLIENT_LOG("  returned");
  GPU_CLIENT_LOG_CODE_BLOCK({
    for (int i = 0; i < state.num_results; ++i) {
      GPU_CLIENT_LOG("  " << i << ": " << (results[i]));
    }
  });

  // TODO(gman): We should be able to free without a token.
  transfer_buffer_->FreePendingToken(state.buffer, helper_->InsertToken());
  CheckGLError();
}

bool GLES2Implementation::GetMultipleIntegervSetup(
    GetMultipleIntegervState* state) {
  state->num_results = 0;
  for (GLuint ii = 0; ii < state->pnames_count; ++ii) {
    int num = util_.GLGetNumValuesReturned(state->pnames[ii]);
    if (!num) {
      SetGLErrorInvalidEnum(
          "glGetMultipleIntegervCHROMIUM", state->pnames[ii], "pname");
      return false;
    }
    state->num_results += num;
  }
  if (static_cast<size_t>(state->results_size) !=
      state->num_results * sizeof(GLint)) {
    SetGLError(GL_INVALID_VALUE, "glGetMultipleIntegervCHROMIUM", "bad size");
    return false;
  }
  for (int ii = 0; ii < state->num_results; ++ii) {
    if (state->results[ii] != 0) {
      SetGLError(GL_INVALID_VALUE,
                 "glGetMultipleIntegervCHROMIUM", "results not set to zero.");
      return false;
    }
  }
  state->transfer_buffer_size_needed =
      state->pnames_count * sizeof(state->pnames[0]) +
      state->num_results * sizeof(state->results[0]);
  return true;
}

void GLES2Implementation::GetMultipleIntegervRequest(
    GetMultipleIntegervState* state) {
  GLenum* pnames_buffer = static_cast<GLenum*>(state->buffer);
  state->results_buffer = pnames_buffer + state->pnames_count;
  memcpy(pnames_buffer, state->pnames, state->pnames_count * sizeof(GLenum));
  memset(state->results_buffer, 0, state->num_results * sizeof(GLint));
  helper_->GetMultipleIntegervCHROMIUM(
      transfer_buffer_->GetShmId(),
      transfer_buffer_->GetOffset(pnames_buffer),
      state->pnames_count,
      transfer_buffer_->GetShmId(),
      transfer_buffer_->GetOffset(state->results_buffer),
      state->results_size);
}

void GLES2Implementation::GetMultipleIntegervOnCompleted(
    GetMultipleIntegervState* state) {
  memcpy(state->results, state->results_buffer, state->results_size);;
}

void GLES2Implementation::GetAllShaderPrecisionFormatsSetup(
    GetAllShaderPrecisionFormatsState* state) {
  state->transfer_buffer_size_needed =
      state->precision_params_count *
      sizeof(cmds::GetShaderPrecisionFormat::Result);
}

void GLES2Implementation::GetAllShaderPrecisionFormatsRequest(
    GetAllShaderPrecisionFormatsState* state) {
  typedef cmds::GetShaderPrecisionFormat::Result Result;
  Result* result = static_cast<Result*>(state->results_buffer);

  for (int i = 0; i < state->precision_params_count; i++) {
    result->success = false;
    helper_->GetShaderPrecisionFormat(state->precision_params[i][0],
                                      state->precision_params[i][1],
                                      transfer_buffer_->GetShmId(),
                                      transfer_buffer_->GetOffset(result));
    result++;
  }
}

void GLES2Implementation::GetAllShaderPrecisionFormatsOnCompleted(
    GetAllShaderPrecisionFormatsState* state) {
  typedef cmds::GetShaderPrecisionFormat::Result Result;
  Result* result = static_cast<Result*>(state->results_buffer);

  for (int i = 0; i < state->precision_params_count; i++) {
    if (result->success) {
      const GLStaticState::ShaderPrecisionKey key(
        state->precision_params[i][0], state->precision_params[i][1]);
      static_state_.shader_precisions[key] = *result;
    }
    result++;
  }
}

void GLES2Implementation::GetProgramInfoCHROMIUMHelper(
    GLuint program, std::vector<int8>* result) {
  DCHECK(result);
  // Clear the bucket so if the command fails nothing will be in it.
  helper_->SetBucketSize(kResultBucketId, 0);
  helper_->GetProgramInfoCHROMIUM(program, kResultBucketId);
  GetBucketContents(kResultBucketId, result);
}

void GLES2Implementation::GetProgramInfoCHROMIUM(
    GLuint program, GLsizei bufsize, GLsizei* size, void* info) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  if (bufsize < 0) {
    SetGLError(
        GL_INVALID_VALUE, "glProgramInfoCHROMIUM", "bufsize less than 0.");
    return;
  }
  if (size == NULL) {
    SetGLError(GL_INVALID_VALUE, "glProgramInfoCHROMIUM", "size is null.");
    return;
  }
  // Make sure they've set size to 0 else the value will be undefined on
  // lost context.
  DCHECK(*size == 0);
  std::vector<int8> result;
  GetProgramInfoCHROMIUMHelper(program, &result);
  if (result.empty()) {
    return;
  }
  *size = result.size();
  if (!info) {
    return;
  }
  if (static_cast<size_t>(bufsize) < result.size()) {
    SetGLError(GL_INVALID_OPERATION,
               "glProgramInfoCHROMIUM", "bufsize is too small for result.");
    return;
  }
  memcpy(info, &result[0], result.size());
}

GLuint GLES2Implementation::CreateStreamTextureCHROMIUM(GLuint texture) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] CreateStreamTextureCHROMIUM("
      << texture << ")");
  TRACE_EVENT0("gpu", "GLES2::CreateStreamTextureCHROMIUM");
  helper_->CommandBufferHelper::Flush();
  return gpu_control_->CreateStreamTexture(texture);
}

void GLES2Implementation::PostSubBufferCHROMIUM(
    GLint x, GLint y, GLint width, GLint height) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] PostSubBufferCHROMIUM("
      << x << ", " << y << ", " << width << ", " << height << ")");
  TRACE_EVENT2("gpu", "GLES2::PostSubBufferCHROMIUM",
               "width", width, "height", height);

  // Same flow control as GLES2Implementation::SwapBuffers (see comments there).
  swap_buffers_tokens_.push(helper_->InsertToken());
  helper_->PostSubBufferCHROMIUM(x, y, width, height);
  helper_->CommandBufferHelper::Flush();
  if (swap_buffers_tokens_.size() > kMaxSwapBuffers + 1) {
    helper_->WaitForToken(swap_buffers_tokens_.front());
    swap_buffers_tokens_.pop();
  }
}

void GLES2Implementation::DeleteQueriesEXTHelper(
    GLsizei n, const GLuint* queries) {
  // TODO(gman): Remove this as queries are not shared resources.
  if (!GetIdHandler(id_namespaces::kQueries)->FreeIds(
      this, n, queries, &GLES2Implementation::DeleteQueriesStub)) {
    SetGLError(
        GL_INVALID_VALUE,
        "glDeleteTextures", "id not created by this context.");
    return;
  }

  for (GLsizei ii = 0; ii < n; ++ii)
    query_tracker_->RemoveQuery(queries[ii]);

  helper_->DeleteQueriesEXTImmediate(n, queries);
}

// TODO(gman): Remove this. Queries are not shared resources.
void GLES2Implementation::DeleteQueriesStub(
    GLsizei /* n */, const GLuint* /* queries */) {
}

GLboolean GLES2Implementation::IsQueryEXT(GLuint id) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] IsQueryEXT(" << id << ")");

  // TODO(gman): To be spec compliant IDs from other contexts sharing
  // resources need to return true here even though you can't share
  // queries across contexts?
  return query_tracker_->GetQuery(id) != NULL;
}

void GLES2Implementation::BeginQueryEXT(GLenum target, GLuint id) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] BeginQueryEXT("
                 << GLES2Util::GetStringQueryTarget(target)
                 << ", " << id << ")");

  // if any outstanding queries INV_OP
  QueryMap::iterator it = current_queries_.find(target);
  if (it != current_queries_.end()) {
    SetGLError(
        GL_INVALID_OPERATION, "glBeginQueryEXT", "query already in progress");
    return;
  }

  // id = 0 INV_OP
  if (id == 0) {
    SetGLError(GL_INVALID_OPERATION, "glBeginQueryEXT", "id is 0");
    return;
  }

  // TODO(gman) if id not GENned INV_OPERATION

  // if id does not have an object
  QueryTracker::Query* query = query_tracker_->GetQuery(id);
  if (!query) {
    query = query_tracker_->CreateQuery(id, target);
    if (!query) {
      SetGLError(GL_OUT_OF_MEMORY,
                 "glBeginQueryEXT",
                 "transfer buffer allocation failed");
      return;
    }
  } else if (query->target() != target) {
    SetGLError(
        GL_INVALID_OPERATION, "glBeginQueryEXT", "target does not match");
    return;
  }

  current_queries_[target] = query;

  query->Begin(this);
  CheckGLError();
}

void GLES2Implementation::EndQueryEXT(GLenum target) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] EndQueryEXT("
                 << GLES2Util::GetStringQueryTarget(target) << ")");
  // Don't do anything if the context is lost.
  if (helper_->IsContextLost()) {
    return;
  }

  QueryMap::iterator it = current_queries_.find(target);
  if (it == current_queries_.end()) {
    SetGLError(GL_INVALID_OPERATION, "glEndQueryEXT", "no active query");
    return;
  }

  QueryTracker::Query* query = it->second;
  query->End(this);
  current_queries_.erase(it);
  CheckGLError();
}

void GLES2Implementation::GetQueryivEXT(
    GLenum target, GLenum pname, GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] GetQueryivEXT("
                 << GLES2Util::GetStringQueryTarget(target) << ", "
                 << GLES2Util::GetStringQueryParameter(pname) << ", "
                 << static_cast<const void*>(params) << ")");

  if (pname != GL_CURRENT_QUERY_EXT) {
    SetGLErrorInvalidEnum("glGetQueryivEXT", pname, "pname");
    return;
  }
  QueryMap::iterator it = current_queries_.find(target);
  if (it != current_queries_.end()) {
    QueryTracker::Query* query = it->second;
    *params = query->id();
  } else {
    *params = 0;
  }
  GPU_CLIENT_LOG("  " << *params);
  CheckGLError();
}

void GLES2Implementation::GetQueryObjectuivEXT(
    GLuint id, GLenum pname, GLuint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] GetQueryivEXT(" << id << ", "
                 << GLES2Util::GetStringQueryObjectParameter(pname) << ", "
                 << static_cast<const void*>(params) << ")");

  QueryTracker::Query* query = query_tracker_->GetQuery(id);
  if (!query) {
    SetGLError(GL_INVALID_OPERATION, "glQueryObjectuivEXT", "unknown query id");
    return;
  }

  QueryMap::iterator it = current_queries_.find(query->target());
  if (it != current_queries_.end()) {
    SetGLError(
        GL_INVALID_OPERATION,
        "glQueryObjectuivEXT", "query active. Did you to call glEndQueryEXT?");
    return;
  }

  if (query->NeverUsed()) {
    SetGLError(
        GL_INVALID_OPERATION,
        "glQueryObjectuivEXT", "Never used. Did you call glBeginQueryEXT?");
    return;
  }

  switch (pname) {
    case GL_QUERY_RESULT_EXT:
      if (!query->CheckResultsAvailable(helper_)) {
        helper_->WaitForToken(query->token());
        if (!query->CheckResultsAvailable(helper_)) {
          FinishHelper();
          CHECK(query->CheckResultsAvailable(helper_));
        }
      }
      *params = query->GetResult();
      break;
    case GL_QUERY_RESULT_AVAILABLE_EXT:
      *params = query->CheckResultsAvailable(helper_);
      break;
    default:
      SetGLErrorInvalidEnum("glQueryObjectuivEXT", pname, "pname");
      break;
  }
  GPU_CLIENT_LOG("  " << *params);
  CheckGLError();
}

void GLES2Implementation::DrawArraysInstancedANGLE(
    GLenum mode, GLint first, GLsizei count, GLsizei primcount) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDrawArraysInstancedANGLE("
      << GLES2Util::GetStringDrawMode(mode) << ", "
      << first << ", " << count << ", " << primcount << ")");
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawArraysInstancedANGLE", "count < 0");
    return;
  }
  if (primcount < 0) {
    SetGLError(GL_INVALID_VALUE, "glDrawArraysInstancedANGLE", "primcount < 0");
    return;
  }
  if (primcount == 0) {
    return;
  }
  bool simulated = false;
  if (!vertex_array_object_manager_->SetupSimulatedClientSideBuffers(
      "glDrawArraysInstancedANGLE", this, helper_, first + count, primcount,
      &simulated)) {
    return;
  }
  helper_->DrawArraysInstancedANGLE(mode, first, count, primcount);
  RestoreArrayBuffer(simulated);
  CheckGLError();
}

void GLES2Implementation::DrawElementsInstancedANGLE(
    GLenum mode, GLsizei count, GLenum type, const void* indices,
    GLsizei primcount) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDrawElementsInstancedANGLE("
      << GLES2Util::GetStringDrawMode(mode) << ", "
      << count << ", "
      << GLES2Util::GetStringIndexType(type) << ", "
      << static_cast<const void*>(indices) << ", "
      << primcount << ")");
  if (count < 0) {
    SetGLError(GL_INVALID_VALUE,
               "glDrawElementsInstancedANGLE", "count less than 0.");
    return;
  }
  if (count == 0) {
    return;
  }
  if (primcount < 0) {
    SetGLError(GL_INVALID_VALUE,
               "glDrawElementsInstancedANGLE", "primcount < 0");
    return;
  }
  if (primcount == 0) {
    return;
  }
  if (vertex_array_object_manager_->bound_element_array_buffer() != 0 &&
      !ValidateOffset("glDrawElementsInstancedANGLE",
                      reinterpret_cast<GLintptr>(indices))) {
    return;
  }
  GLuint offset = 0;
  bool simulated = false;
  if (!vertex_array_object_manager_->SetupSimulatedIndexAndClientSideBuffers(
      "glDrawElementsInstancedANGLE", this, helper_, count, type, primcount,
      indices, &offset, &simulated)) {
    return;
  }
  helper_->DrawElementsInstancedANGLE(mode, count, type, offset, primcount);
  RestoreElementAndArrayBuffers(simulated);
  CheckGLError();
}

void GLES2Implementation::GenMailboxCHROMIUM(
    GLbyte* mailbox) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glGenMailboxCHROMIUM("
      << static_cast<const void*>(mailbox) << ")");
  TRACE_EVENT0("gpu", "GLES2::GenMailboxCHROMIUM");

  gpu::Mailbox result = gpu::Mailbox::Generate();
  memcpy(mailbox, result.name, sizeof(result.name));
}

void GLES2Implementation::ProduceTextureCHROMIUM(GLenum target,
                                                 const GLbyte* data) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glProduceTextureCHROMIUM("
                     << static_cast<const void*>(data) << ")");
  const Mailbox& mailbox = *reinterpret_cast<const Mailbox*>(data);
  DCHECK(mailbox.Verify()) << "ProduceTextureCHROMIUM was passed a "
                              "mailbox that was not generated by "
                              "GenMailboxCHROMIUM.";
  helper_->ProduceTextureCHROMIUMImmediate(target, data);
  CheckGLError();
}

void GLES2Implementation::ProduceTextureDirectCHROMIUM(
    GLuint texture, GLenum target, const GLbyte* data) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glProduceTextureDirectCHROMIUM("
                     << static_cast<const void*>(data) << ")");
  const Mailbox& mailbox = *reinterpret_cast<const Mailbox*>(data);
  DCHECK(mailbox.Verify()) << "ProduceTextureDirectCHROMIUM was passed a "
                              "mailbox that was not generated by "
                              "GenMailboxCHROMIUM.";
  helper_->ProduceTextureDirectCHROMIUMImmediate(texture, target, data);
  CheckGLError();
}

void GLES2Implementation::ConsumeTextureCHROMIUM(GLenum target,
                                                 const GLbyte* data) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glConsumeTextureCHROMIUM("
                     << static_cast<const void*>(data) << ")");
  const Mailbox& mailbox = *reinterpret_cast<const Mailbox*>(data);
  DCHECK(mailbox.Verify()) << "ConsumeTextureCHROMIUM was passed a "
                              "mailbox that was not generated by "
                              "GenMailboxCHROMIUM.";
  helper_->ConsumeTextureCHROMIUMImmediate(target, data);
  CheckGLError();
}

GLuint GLES2Implementation::CreateAndConsumeTextureCHROMIUM(
    GLenum target, const GLbyte* data) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glCreateAndConsumeTextureCHROMIUM("
                     << static_cast<const void*>(data) << ")");
  const Mailbox& mailbox = *reinterpret_cast<const Mailbox*>(data);
  DCHECK(mailbox.Verify()) << "CreateAndConsumeTextureCHROMIUM was passed a "
                              "mailbox that was not generated by "
                              "GenMailboxCHROMIUM.";
  GLuint client_id;
  GetIdHandler(id_namespaces::kTextures)->MakeIds(this, 0, 1, &client_id);
  helper_->CreateAndConsumeTextureCHROMIUMImmediate(target,
      client_id, data);
  if (share_group_->bind_generates_resource())
    helper_->CommandBufferHelper::Flush();
  CheckGLError();
  return client_id;
}

void GLES2Implementation::PushGroupMarkerEXT(
    GLsizei length, const GLchar* marker) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glPushGroupMarkerEXT("
      << length << ", " << marker << ")");
  if (!marker) {
    marker = "";
  }
  SetBucketAsString(
      kResultBucketId,
      (length ? std::string(marker, length) : std::string(marker)));
  helper_->PushGroupMarkerEXT(kResultBucketId);
  helper_->SetBucketSize(kResultBucketId, 0);
  debug_marker_manager_.PushGroup(
      length ? std::string(marker, length) : std::string(marker));
}

void GLES2Implementation::InsertEventMarkerEXT(
    GLsizei length, const GLchar* marker) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glInsertEventMarkerEXT("
      << length << ", " << marker << ")");
  if (!marker) {
    marker = "";
  }
  SetBucketAsString(
      kResultBucketId,
      (length ? std::string(marker, length) : std::string(marker)));
  helper_->InsertEventMarkerEXT(kResultBucketId);
  helper_->SetBucketSize(kResultBucketId, 0);
  debug_marker_manager_.SetMarker(
      length ? std::string(marker, length) : std::string(marker));
}

void GLES2Implementation::PopGroupMarkerEXT() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glPopGroupMarkerEXT()");
  helper_->PopGroupMarkerEXT();
  debug_marker_manager_.PopGroup();
}

void GLES2Implementation::TraceBeginCHROMIUM(const char* name) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTraceBeginCHROMIUM("
                 << name << ")");
  if (current_trace_name_.get()) {
    SetGLError(GL_INVALID_OPERATION, "glTraceBeginCHROMIUM",
               "trace already running");
    return;
  }
  TRACE_EVENT_COPY_ASYNC_BEGIN0("gpu", name, this);
  SetBucketAsCString(kResultBucketId, name);
  helper_->TraceBeginCHROMIUM(kResultBucketId);
  helper_->SetBucketSize(kResultBucketId, 0);
  current_trace_name_.reset(new std::string(name));
}

void GLES2Implementation::TraceEndCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTraceEndCHROMIUM(" << ")");
  if (!current_trace_name_.get()) {
    SetGLError(GL_INVALID_OPERATION, "glTraceEndCHROMIUM",
               "missing begin trace");
    return;
  }
  helper_->TraceEndCHROMIUM();
  TRACE_EVENT_COPY_ASYNC_END0("gpu", current_trace_name_->c_str(), this);
  current_trace_name_.reset();
}

void* GLES2Implementation::MapBufferCHROMIUM(GLuint target, GLenum access) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glMapBufferCHROMIUM("
      << target << ", " << GLES2Util::GetStringEnum(access) << ")");
  switch (target)  {
    case GL_PIXEL_PACK_TRANSFER_BUFFER_CHROMIUM:
      if (access != GL_READ_ONLY) {
        SetGLError(GL_INVALID_ENUM, "glMapBufferCHROMIUM", "bad access mode");
        return NULL;
      }
      break;
    case GL_PIXEL_UNPACK_TRANSFER_BUFFER_CHROMIUM:
      if (access != GL_WRITE_ONLY) {
        SetGLError(GL_INVALID_ENUM, "glMapBufferCHROMIUM", "bad access mode");
        return NULL;
      }
      break;
    default:
      SetGLError(
          GL_INVALID_ENUM, "glMapBufferCHROMIUM", "invalid target");
      return NULL;
  }
  GLuint buffer_id;
  GetBoundPixelTransferBuffer(target, "glMapBufferCHROMIUM", &buffer_id);
  if (!buffer_id) {
    return NULL;
  }
  BufferTracker::Buffer* buffer = buffer_tracker_->GetBuffer(buffer_id);
  if (!buffer) {
    SetGLError(GL_INVALID_OPERATION, "glMapBufferCHROMIUM", "invalid buffer");
    return NULL;
  }
  if (buffer->mapped()) {
    SetGLError(GL_INVALID_OPERATION, "glMapBufferCHROMIUM", "already mapped");
    return NULL;
  }
  // Here we wait for previous transfer operations to be finished.
  // TODO(hubbe): AsyncTex(Sub)Image2dCHROMIUM does not currently work
  // with this method of synchronization. Until this is fixed,
  // MapBufferCHROMIUM will not block even if the transfer is not ready
  // for these calls.
  if (buffer->last_usage_token()) {
    helper_->WaitForToken(buffer->last_usage_token());
    buffer->set_last_usage_token(0);
  }
  buffer->set_mapped(true);

  GPU_CLIENT_LOG("  returned " << buffer->address());
  CheckGLError();
  return buffer->address();
}

GLboolean GLES2Implementation::UnmapBufferCHROMIUM(GLuint target) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glUnmapBufferCHROMIUM(" << target << ")");
  GLuint buffer_id;
  if (!GetBoundPixelTransferBuffer(target, "glMapBufferCHROMIUM", &buffer_id)) {
    SetGLError(GL_INVALID_ENUM, "glUnmapBufferCHROMIUM", "invalid target");
  }
  if (!buffer_id) {
    return false;
  }
  BufferTracker::Buffer* buffer = buffer_tracker_->GetBuffer(buffer_id);
  if (!buffer) {
    SetGLError(GL_INVALID_OPERATION, "glUnmapBufferCHROMIUM", "invalid buffer");
    return false;
  }
  if (!buffer->mapped()) {
    SetGLError(GL_INVALID_OPERATION, "glUnmapBufferCHROMIUM", "not mapped");
    return false;
  }
  buffer->set_mapped(false);
  CheckGLError();
  return true;
}

bool GLES2Implementation::EnsureAsyncUploadSync() {
  if (async_upload_sync_)
    return true;

  int32 shm_id;
  unsigned int shm_offset;
  void* mem = mapped_memory_->Alloc(sizeof(AsyncUploadSync),
                                    &shm_id,
                                    &shm_offset);
  if (!mem)
    return false;

  async_upload_sync_shm_id_ = shm_id;
  async_upload_sync_shm_offset_ = shm_offset;
  async_upload_sync_ = static_cast<AsyncUploadSync*>(mem);
  async_upload_sync_->Reset();

  return true;
}

uint32 GLES2Implementation::NextAsyncUploadToken() {
  async_upload_token_++;
  if (async_upload_token_ == 0)
    async_upload_token_++;
  return async_upload_token_;
}

void GLES2Implementation::PollAsyncUploads() {
  if (!async_upload_sync_)
    return;

  if (helper_->IsContextLost()) {
    DetachedAsyncUploadMemoryList::iterator it =
        detached_async_upload_memory_.begin();
    while (it != detached_async_upload_memory_.end()) {
      mapped_memory_->Free(it->first);
      it = detached_async_upload_memory_.erase(it);
    }
    return;
  }

  DetachedAsyncUploadMemoryList::iterator it =
      detached_async_upload_memory_.begin();
  while (it != detached_async_upload_memory_.end()) {
    if (HasAsyncUploadTokenPassed(it->second)) {
      mapped_memory_->Free(it->first);
      it = detached_async_upload_memory_.erase(it);
    } else {
      break;
    }
  }
}

void GLES2Implementation::FreeAllAsyncUploadBuffers() {
  // Free all completed unmanaged async uploads buffers.
  PollAsyncUploads();

  // Synchronously free rest of the unmanaged async upload buffers.
  if (!detached_async_upload_memory_.empty()) {
    WaitAllAsyncTexImage2DCHROMIUM();
    WaitForCmd();
    PollAsyncUploads();
  }
}

void GLES2Implementation::AsyncTexImage2DCHROMIUM(
    GLenum target, GLint level, GLenum internalformat, GLsizei width,
    GLsizei height, GLint border, GLenum format, GLenum type,
    const void* pixels) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glTexImage2D("
      << GLES2Util::GetStringTextureTarget(target) << ", "
      << level << ", "
      << GLES2Util::GetStringTextureInternalFormat(internalformat) << ", "
      << width << ", " << height << ", " << border << ", "
      << GLES2Util::GetStringTextureFormat(format) << ", "
      << GLES2Util::GetStringPixelType(type) << ", "
      << static_cast<const void*>(pixels) << ")");
  if (level < 0 || height < 0 || width < 0) {
    SetGLError(GL_INVALID_VALUE, "glTexImage2D", "dimension < 0");
    return;
  }
  if (border != 0) {
    SetGLError(GL_INVALID_VALUE, "glTexImage2D", "border != 0");
    return;
  }
  uint32 size;
  uint32 unpadded_row_size;
  uint32 padded_row_size;
  if (!GLES2Util::ComputeImageDataSizes(
          width, height, format, type, unpack_alignment_, &size,
          &unpadded_row_size, &padded_row_size)) {
    SetGLError(GL_INVALID_VALUE, "glTexImage2D", "image size too large");
    return;
  }

  // If there's no data/buffer just issue the AsyncTexImage2D
  if (!pixels && !bound_pixel_unpack_transfer_buffer_id_) {
    helper_->AsyncTexImage2DCHROMIUM(
       target, level, internalformat, width, height, format, type,
       0, 0, 0, 0, 0);
    return;
  }

  if (!EnsureAsyncUploadSync()) {
    SetGLError(GL_OUT_OF_MEMORY, "glTexImage2D", "out of memory");
    return;
  }

  // Otherwise, async uploads require a transfer buffer to be bound.
  // TODO(hubbe): Make MapBufferCHROMIUM block if someone tries to re-use
  // the buffer before the transfer is finished. (Currently such
  // synchronization has to be handled manually.)
  GLuint offset = ToGLuint(pixels);
  BufferTracker::Buffer* buffer = GetBoundPixelUnpackTransferBufferIfValid(
      bound_pixel_unpack_transfer_buffer_id_,
      "glAsyncTexImage2DCHROMIUM", offset, size);
  if (buffer && buffer->shm_id() != -1) {
    uint32 async_token = NextAsyncUploadToken();
    buffer->set_last_async_upload_token(async_token);
    helper_->AsyncTexImage2DCHROMIUM(
        target, level, internalformat, width, height, format, type,
        buffer->shm_id(), buffer->shm_offset() + offset,
        async_token,
        async_upload_sync_shm_id_, async_upload_sync_shm_offset_);
  }
}

void GLES2Implementation::AsyncTexSubImage2DCHROMIUM(
    GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
    GLsizei height, GLenum format, GLenum type, const void* pixels) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glAsyncTexSubImage2DCHROMIUM("
      << GLES2Util::GetStringTextureTarget(target) << ", "
      << level << ", "
      << xoffset << ", " << yoffset << ", "
      << width << ", " << height << ", "
      << GLES2Util::GetStringTextureFormat(format) << ", "
      << GLES2Util::GetStringPixelType(type) << ", "
      << static_cast<const void*>(pixels) << ")");
  if (level < 0 || height < 0 || width < 0) {
    SetGLError(
        GL_INVALID_VALUE, "glAsyncTexSubImage2DCHROMIUM", "dimension < 0");
    return;
  }

  uint32 size;
  uint32 unpadded_row_size;
  uint32 padded_row_size;
  if (!GLES2Util::ComputeImageDataSizes(
        width, height, format, type, unpack_alignment_, &size,
        &unpadded_row_size, &padded_row_size)) {
    SetGLError(
        GL_INVALID_VALUE, "glAsyncTexSubImage2DCHROMIUM", "size to large");
    return;
  }

  if (!EnsureAsyncUploadSync()) {
    SetGLError(GL_OUT_OF_MEMORY, "glTexImage2D", "out of memory");
    return;
  }

  // Async uploads require a transfer buffer to be bound.
  // TODO(hubbe): Make MapBufferCHROMIUM block if someone tries to re-use
  // the buffer before the transfer is finished. (Currently such
  // synchronization has to be handled manually.)
  GLuint offset = ToGLuint(pixels);
  BufferTracker::Buffer* buffer = GetBoundPixelUnpackTransferBufferIfValid(
      bound_pixel_unpack_transfer_buffer_id_,
      "glAsyncTexSubImage2DCHROMIUM", offset, size);
  if (buffer && buffer->shm_id() != -1) {
    uint32 async_token = NextAsyncUploadToken();
    buffer->set_last_async_upload_token(async_token);
    helper_->AsyncTexSubImage2DCHROMIUM(
        target, level, xoffset, yoffset, width, height, format, type,
        buffer->shm_id(), buffer->shm_offset() + offset,
        async_token,
        async_upload_sync_shm_id_, async_upload_sync_shm_offset_);
  }
}

void GLES2Implementation::WaitAsyncTexImage2DCHROMIUM(GLenum target) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glWaitAsyncTexImage2DCHROMIUM("
      << GLES2Util::GetStringTextureTarget(target) << ")");
  helper_->WaitAsyncTexImage2DCHROMIUM(target);
  CheckGLError();
}

void GLES2Implementation::WaitAllAsyncTexImage2DCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix()
      << "] glWaitAllAsyncTexImage2DCHROMIUM()");
  helper_->WaitAllAsyncTexImage2DCHROMIUM();
  CheckGLError();
}

GLuint GLES2Implementation::InsertSyncPointCHROMIUM() {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glInsertSyncPointCHROMIUM");
  helper_->CommandBufferHelper::Flush();
  return gpu_control_->InsertSyncPoint();
}

GLuint GLES2Implementation::CreateImageCHROMIUMHelper(GLsizei width,
                                                      GLsizei height,
                                                      GLenum internalformat,
                                                      GLenum usage) {
  if (width <= 0) {
    SetGLError(GL_INVALID_VALUE, "glCreateImageCHROMIUM", "width <= 0");
    return 0;
  }

  if (height <= 0) {
    SetGLError(GL_INVALID_VALUE, "glCreateImageCHROMIUM", "height <= 0");
    return 0;
  }
  // Flush the command stream to ensure ordering in case the newly
  // returned image_id has recently been in use with a different buffer.
  helper_->CommandBufferHelper::Flush();

  // Create new buffer.
  GLuint buffer_id = gpu_memory_buffer_tracker_->CreateBuffer(
      width, height, internalformat, usage);
  if (buffer_id == 0) {
    SetGLError(GL_OUT_OF_MEMORY, "glCreateImageCHROMIUM", "out of GPU memory.");
    return 0;
  }
  return buffer_id;
}

GLuint GLES2Implementation::CreateImageCHROMIUM(GLsizei width,
                                                GLsizei height,
                                                GLenum internalformat,
                                                GLenum usage) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG(
      "[" << GetLogPrefix() << "] glCreateImageCHROMIUM(" << width << ", "
          << height << ", "
          << GLES2Util::GetStringTextureInternalFormat(internalformat) << ", "
          << GLES2Util::GetStringTextureInternalFormat(usage) << ")");
  GLuint image_id =
      CreateImageCHROMIUMHelper(width, height, internalformat, usage);
  CheckGLError();
  return image_id;
}

void GLES2Implementation::DestroyImageCHROMIUMHelper(GLuint image_id) {
  gfx::GpuMemoryBuffer* gpu_buffer = gpu_memory_buffer_tracker_->GetBuffer(
      image_id);
  if (!gpu_buffer) {
    SetGLError(GL_INVALID_OPERATION, "glDestroyImageCHROMIUM", "invalid image");
    return;
  }

  // Flush the command stream to make sure all pending commands
  // that may refer to the image_id are executed on the service side.
  helper_->CommandBufferHelper::Flush();
  gpu_memory_buffer_tracker_->RemoveBuffer(image_id);
}

void GLES2Implementation::DestroyImageCHROMIUM(GLuint image_id) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glDestroyImageCHROMIUM("
      << image_id << ")");
  DestroyImageCHROMIUMHelper(image_id);
  CheckGLError();
}

void GLES2Implementation::UnmapImageCHROMIUMHelper(GLuint image_id) {
  gfx::GpuMemoryBuffer* gpu_buffer = gpu_memory_buffer_tracker_->GetBuffer(
      image_id);
  if (!gpu_buffer) {
    SetGLError(GL_INVALID_OPERATION, "glUnmapImageCHROMIUM", "invalid image");
    return;
  }

  if (!gpu_buffer->IsMapped()) {
    SetGLError(GL_INVALID_OPERATION, "glUnmapImageCHROMIUM", "not mapped");
    return;
  }
  gpu_buffer->Unmap();
}

void GLES2Implementation::UnmapImageCHROMIUM(GLuint image_id) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glUnmapImageCHROMIUM("
       << image_id << ")");

  UnmapImageCHROMIUMHelper(image_id);
  CheckGLError();
}

void* GLES2Implementation::MapImageCHROMIUMHelper(GLuint image_id) {
  gfx::GpuMemoryBuffer* gpu_buffer = gpu_memory_buffer_tracker_->GetBuffer(
      image_id);
  if (!gpu_buffer) {
    SetGLError(GL_INVALID_OPERATION, "glMapImageCHROMIUM", "invalid image");
    return NULL;
  }

  if (gpu_buffer->IsMapped()) {
    SetGLError(GL_INVALID_OPERATION, "glMapImageCHROMIUM", "already mapped");
    return NULL;
  }

  return gpu_buffer->Map();
}

void* GLES2Implementation::MapImageCHROMIUM(GLuint image_id) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glMapImageCHROMIUM(" << image_id
                     << ")");

  void* mapped = MapImageCHROMIUMHelper(image_id);
  CheckGLError();
  return mapped;
}

void GLES2Implementation::GetImageParameterivCHROMIUMHelper(
    GLuint image_id, GLenum pname, GLint* params) {
  if (pname != GL_IMAGE_ROWBYTES_CHROMIUM) {
    SetGLError(GL_INVALID_ENUM, "glGetImageParameterivCHROMIUM",
               "invalid parameter");
    return;
  }

  gfx::GpuMemoryBuffer* gpu_buffer = gpu_memory_buffer_tracker_->GetBuffer(
      image_id);
  if (!gpu_buffer) {
    SetGLError(GL_INVALID_OPERATION, "glGetImageParameterivCHROMIUM",
               "invalid image");
    return;
  }

  if (!gpu_buffer->IsMapped()) {
    SetGLError(
        GL_INVALID_OPERATION, "glGetImageParameterivCHROMIUM", "not mapped");
    return;
  }

  *params = gpu_buffer->GetStride();
}

void GLES2Implementation::GetImageParameterivCHROMIUM(
    GLuint image_id, GLenum pname, GLint* params) {
  GPU_CLIENT_SINGLE_THREAD_CHECK();
  GPU_CLIENT_VALIDATE_DESTINATION_INITALIZATION(GLint, params);
  GPU_CLIENT_LOG("[" << GetLogPrefix() << "] glImageParameterivCHROMIUM("
      << image_id << ", "
      << GLES2Util::GetStringBufferParameter(pname) << ", "
      << static_cast<const void*>(params) << ")");
  GetImageParameterivCHROMIUMHelper(image_id, pname, params);
  CheckGLError();
}

bool GLES2Implementation::ValidateSize(const char* func, GLsizeiptr size) {
  if (size < 0) {
    SetGLError(GL_INVALID_VALUE, func, "size < 0");
    return false;
  }
  if (!FitInt32NonNegative<GLsizeiptr>(size)) {
    SetGLError(GL_INVALID_OPERATION, func, "size more than 32-bit");
    return false;
  }
  return true;
}

bool GLES2Implementation::ValidateOffset(const char* func, GLintptr offset) {
  if (offset < 0) {
    SetGLError(GL_INVALID_VALUE, func, "offset < 0");
    return false;
  }
  if (!FitInt32NonNegative<GLintptr>(offset)) {
    SetGLError(GL_INVALID_OPERATION, func, "offset more than 32-bit");
    return false;
  }
  return true;
}

// Include the auto-generated part of this file. We split this because it means
// we can easily edit the non-auto generated parts right here in this file
// instead of having to edit some template or the code generator.
#include "gpu/command_buffer/client/gles2_implementation_impl_autogen.h"

}  // namespace gles2
}  // namespace gpu
