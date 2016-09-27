// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/in_process_command_buffer.h"

#include <queue>
#include <set>
#include <utility>

#include <GLES2/gl2.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>

#ifdef TENCENT_CHANGES
#include "base/android/sys_utils.h"
#endif
#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/weak_ptr.h"
#include "base/message_loop/message_loop_proxy.h"
#include "base/sequence_checker.h"
#include "base/synchronization/condition_variable.h"
#include "base/threading/thread.h"
#include "gpu/command_buffer/client/gpu_memory_buffer_factory.h"
#include "gpu/command_buffer/service/command_buffer_service.h"
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/gl_context_virtual.h"
#include "gpu/command_buffer/service/gpu_control_service.h"
#include "gpu/command_buffer/service/gpu_scheduler.h"
#ifdef TENCENT_CHANGES
#include "gpu/command_buffer/service/gpu_switches.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#endif
#include "gpu/command_buffer/service/image_manager.h"
#include "gpu/command_buffer/service/mailbox_manager.h"
#include "gpu/command_buffer/service/transfer_buffer_manager.h"
#include "ui/gfx/size.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_image.h"
#include "ui/gl/gl_share_group.h"
#ifdef TENCENT_CHANGES
#include "content/renderer/tencent/gpu_memory_tracker_tencent.h"
#endif

#if defined(OS_ANDROID)
#include "gpu/command_buffer/service/stream_texture_manager_in_process_android.h"
#include "ui/gl/android/surface_texture.h"
#endif

namespace gpu {

namespace {

static GpuMemoryBufferFactory* g_gpu_memory_buffer_factory = NULL;

template <typename T>
static void RunTaskWithResult(base::Callback<T(void)> task,
                              T* result,
                              base::WaitableEvent* completion) {
  *result = task.Run();
  completion->Signal();
}

class GpuInProcessThread
    : public base::Thread,
      public InProcessCommandBuffer::Service,
      public base::RefCountedThreadSafe<GpuInProcessThread> {
 public:
  GpuInProcessThread();

  virtual void AddRef() const OVERRIDE {
    base::RefCountedThreadSafe<GpuInProcessThread>::AddRef();
  }
  virtual void Release() const OVERRIDE {
    base::RefCountedThreadSafe<GpuInProcessThread>::Release();
  }

  virtual void ScheduleTask(const base::Closure& task) OVERRIDE;
  virtual void ScheduleIdleWork(const base::Closure& callback) OVERRIDE;
  virtual bool UseVirtualizedGLContexts() OVERRIDE { return false; }
  virtual scoped_refptr<gles2::ShaderTranslatorCache> shader_translator_cache()
      OVERRIDE;

#ifdef TENCENT_CHANGES
  virtual GpuMemoryTrackerTencent* GetMemoryTracker() OVERRIDE;
#endif

 private:
  virtual ~GpuInProcessThread();
  friend class base::RefCountedThreadSafe<GpuInProcessThread>;

  scoped_refptr<gpu::gles2::ShaderTranslatorCache> shader_translator_cache_;
  DISALLOW_COPY_AND_ASSIGN(GpuInProcessThread);
};

GpuInProcessThread::GpuInProcessThread() : base::Thread("GpuThread") {
  Start();
}

GpuInProcessThread::~GpuInProcessThread() {
  Stop();
}

void GpuInProcessThread::ScheduleTask(const base::Closure& task) {
  message_loop()->PostTask(FROM_HERE, task);
}

void GpuInProcessThread::ScheduleIdleWork(const base::Closure& callback) {
  message_loop()->PostDelayedTask(
      FROM_HERE, callback, base::TimeDelta::FromMilliseconds(5));
}

scoped_refptr<gles2::ShaderTranslatorCache>
GpuInProcessThread::shader_translator_cache() {
  if (!shader_translator_cache_.get())
    shader_translator_cache_ = new gpu::gles2::ShaderTranslatorCache;
  return shader_translator_cache_;
}

#ifdef TENCENT_CHANGES
GpuMemoryTrackerTencent* GpuInProcessThread::GetMemoryTracker() {
  return GpuMemoryTrackerTencent::GetInstance();
}
#endif

base::LazyInstance<std::set<InProcessCommandBuffer*> > default_thread_clients_ =
    LAZY_INSTANCE_INITIALIZER;
base::LazyInstance<base::Lock> default_thread_clients_lock_ =
    LAZY_INSTANCE_INITIALIZER;

class ScopedEvent {
 public:
  ScopedEvent(base::WaitableEvent* event) : event_(event) {}
  ~ScopedEvent() { event_->Signal(); }

 private:
  base::WaitableEvent* event_;
};

class SyncPointManager {
 public:
  SyncPointManager();
  ~SyncPointManager();

  uint32 GenerateSyncPoint();
  void RetireSyncPoint(uint32 sync_point);

  bool IsSyncPointPassed(uint32 sync_point);
  void WaitSyncPoint(uint32 sync_point);

private:
  // This lock protects access to pending_sync_points_ and next_sync_point_ and
  // is used with the ConditionVariable to signal when a sync point is retired.
  base::Lock lock_;
  std::set<uint32> pending_sync_points_;
  uint32 next_sync_point_;
  base::ConditionVariable cond_var_;
};

SyncPointManager::SyncPointManager() : next_sync_point_(1), cond_var_(&lock_) {}

SyncPointManager::~SyncPointManager() {
  DCHECK_EQ(pending_sync_points_.size(), 0U);
}

uint32 SyncPointManager::GenerateSyncPoint() {
  base::AutoLock lock(lock_);
  uint32 sync_point = next_sync_point_++;
  DCHECK_EQ(pending_sync_points_.count(sync_point), 0U);
  pending_sync_points_.insert(sync_point);
  return sync_point;
}

void SyncPointManager::RetireSyncPoint(uint32 sync_point) {
  base::AutoLock lock(lock_);
  DCHECK(pending_sync_points_.count(sync_point));
  pending_sync_points_.erase(sync_point);
  cond_var_.Broadcast();
}

bool SyncPointManager::IsSyncPointPassed(uint32 sync_point) {
  base::AutoLock lock(lock_);
  return pending_sync_points_.count(sync_point) == 0;
}

void SyncPointManager::WaitSyncPoint(uint32 sync_point) {
  base::AutoLock lock(lock_);
  while (pending_sync_points_.count(sync_point)) {
    cond_var_.Wait();
  }
}

base::LazyInstance<SyncPointManager> g_sync_point_manager =
    LAZY_INSTANCE_INITIALIZER;

bool WaitSyncPoint(uint32 sync_point) {
  g_sync_point_manager.Get().WaitSyncPoint(sync_point);
  return true;
}

}  // anonyous namespace

InProcessCommandBuffer::Service::Service() {}

InProcessCommandBuffer::Service::~Service() {}

scoped_refptr<InProcessCommandBuffer::Service>
InProcessCommandBuffer::GetDefaultService() {
  base::AutoLock lock(default_thread_clients_lock_.Get());
  scoped_refptr<Service> service;
  if (!default_thread_clients_.Get().empty()) {
    InProcessCommandBuffer* other = *default_thread_clients_.Get().begin();
    service = other->service_;
    DCHECK(service.get());
  } else {
    service = new GpuInProcessThread;
  }
  return service;
}

InProcessCommandBuffer::InProcessCommandBuffer(
    const scoped_refptr<Service>& service)
    : context_lost_(false),
      idle_work_pending_(false),
      last_put_offset_(-1),
      flush_event_(false, false),
      service_(service.get() ? service : GetDefaultService()),
      gpu_thread_weak_ptr_factory_(this) {
  if (!service) {
    base::AutoLock lock(default_thread_clients_lock_.Get());
    default_thread_clients_.Get().insert(this);
  }
}

InProcessCommandBuffer::~InProcessCommandBuffer() {
  Destroy();
  base::AutoLock lock(default_thread_clients_lock_.Get());
  default_thread_clients_.Get().erase(this);
}

void InProcessCommandBuffer::OnResizeView(gfx::Size size, float scale_factor) {
  CheckSequencedThread();
  DCHECK(!surface_->IsOffscreen());
  surface_->Resize(size);
}

bool InProcessCommandBuffer::MakeCurrent() {
  CheckSequencedThread();
  command_buffer_lock_.AssertAcquired();

  if (!context_lost_ && decoder_->MakeCurrent())
    return true;
  DLOG(ERROR) << "Context lost because MakeCurrent failed.";
  command_buffer_->SetContextLostReason(decoder_->GetContextLostReason());
  command_buffer_->SetParseError(gpu::error::kLostContext);
  return false;
}

void InProcessCommandBuffer::PumpCommands() {
  CheckSequencedThread();
  command_buffer_lock_.AssertAcquired();

  if (!MakeCurrent())
    return;

  gpu_scheduler_->PutChanged();
}

bool InProcessCommandBuffer::GetBufferChanged(int32 transfer_buffer_id) {
  CheckSequencedThread();
  command_buffer_lock_.AssertAcquired();
  command_buffer_->SetGetBuffer(transfer_buffer_id);
  return true;
}

bool InProcessCommandBuffer::Initialize(
    scoped_refptr<gfx::GLSurface> surface,
    bool is_offscreen,
    gfx::AcceleratedWidget window,
    const gfx::Size& size,
    const std::vector<int32>& attribs,
    gfx::GpuPreference gpu_preference,
    const base::Closure& context_lost_callback,
    InProcessCommandBuffer* share_group) {
  DCHECK(!share_group || service_ == share_group->service_);
  context_lost_callback_ = WrapCallback(context_lost_callback);

  if (surface) {
    // GPU thread must be the same as client thread due to GLSurface not being
    // thread safe.
    sequence_checker_.reset(new base::SequenceChecker);
    surface_ = surface;
  }

  gpu::Capabilities capabilities;
  InitializeOnGpuThreadParams params(is_offscreen,
                                     window,
                                     size,
                                     attribs,
                                     gpu_preference,
                                     &capabilities,
                                     share_group);

  base::Callback<bool(void)> init_task =
      base::Bind(&InProcessCommandBuffer::InitializeOnGpuThread,
                 base::Unretained(this),
                 params);

  base::WaitableEvent completion(true, false);
  bool result = false;
  QueueTask(
      base::Bind(&RunTaskWithResult<bool>, init_task, &result, &completion));
  completion.Wait();

  if (result) {
    capabilities_ = capabilities;
    capabilities_.map_image =
        capabilities_.map_image && g_gpu_memory_buffer_factory;
  }
  return result;
}

bool InProcessCommandBuffer::InitializeOnGpuThread(
    const InitializeOnGpuThreadParams& params) {
  CheckSequencedThread();
  gpu_thread_weak_ptr_ = gpu_thread_weak_ptr_factory_.GetWeakPtr();

  DCHECK(params.size.width() >= 0 && params.size.height() >= 0);

  TransferBufferManager* manager = new TransferBufferManager();
  transfer_buffer_manager_.reset(manager);
  manager->Initialize();

  scoped_ptr<CommandBufferService> command_buffer(
      new CommandBufferService(transfer_buffer_manager_.get()));
  command_buffer->SetPutOffsetChangeCallback(base::Bind(
      &InProcessCommandBuffer::PumpCommands, gpu_thread_weak_ptr_));
  command_buffer->SetParseErrorCallback(base::Bind(
      &InProcessCommandBuffer::OnContextLost, gpu_thread_weak_ptr_));

  if (!command_buffer->Initialize()) {
    LOG(ERROR) << "Could not initialize command buffer.";
    DestroyOnGpuThread();
    return false;
  }

  gl_share_group_ = params.context_group
                        ? params.context_group->gl_share_group_.get()
                        : new gfx::GLShareGroup;

#if defined(OS_ANDROID)
  stream_texture_manager_.reset(new StreamTextureManagerInProcess);
#endif

#ifdef TENCENT_CHANGES
  gles2::MemoryTracker* memory_tracker = service_->GetMemoryTracker();
#endif

  bool bind_generates_resource = false;
  decoder_.reset(gles2::GLES2Decoder::Create(
      params.context_group
          ? params.context_group->decoder_->GetContextGroup()
          : new gles2::ContextGroup(NULL,
                                    NULL,
#ifdef TENCENT_CHANGES
                                    memory_tracker,
#else
                                    NULL,
#endif
                                    service_->shader_translator_cache(),
                                    NULL,
                                    bind_generates_resource)));

  gpu_scheduler_.reset(
      new GpuScheduler(command_buffer.get(), decoder_.get(), decoder_.get()));
  command_buffer->SetGetBufferChangeCallback(base::Bind(
      &GpuScheduler::SetGetBuffer, base::Unretained(gpu_scheduler_.get())));
  command_buffer_ = command_buffer.Pass();

  decoder_->set_engine(gpu_scheduler_.get());

  if (!surface_) {
    if (params.is_offscreen)
      surface_ = gfx::GLSurface::CreateOffscreenGLSurface(params.size);
    else
      surface_ = gfx::GLSurface::CreateViewGLSurface(params.window);
  }

  if (!surface_.get()) {
    LOG(ERROR) << "Could not create GLSurface.";
    DestroyOnGpuThread();
    return false;
  }

#ifdef TENCENT_CHANGES
  //在这里处理，当MailBoxSynchronizer中的use sharegroup开启时，所有没有sharegroup的eglcontext都与android提供的context sharegroup。
  //其实在上层处理是最佳选择。但是上层使用sharegroup后，会导致GLES2Decoder中持有的***manager成员都共享了。可能存在问题。
  //最主要还是需要共享的eglcontext与android提供的context属于不同线程，不同的service。共享也会导致chromium逻辑问题。
  bool isSharedGroupFromSysContext = false;
  if (params.is_offscreen && NULL == gl_share_group_->GetHandle() &&
      UseShareGroupInMailBoxSynchronizer()) {
    bool share_sys_context = false;
    gles2::ContextCreationAttribHelper attrib_parser;
    if (attrib_parser.Parse(params.attribs))
      share_sys_context = attrib_parser.share_sys_context_;
    if (share_sys_context && !gfx::GLContext::android_sys_eglcontext) {
      LOG(ERROR) << "android_sys_eglcontext is NULL.";
      DestroyOnGpuThread();
      return false;
    }
    gl_share_group_->AddContext(gfx::GLContext::android_sys_eglcontext);
    isSharedGroupFromSysContext = true;
  }
#endif

  if (service_->UseVirtualizedGLContexts()) {
    context_ = gl_share_group_->GetSharedContext();
    if (!context_.get()) {
      context_ = gfx::GLContext::CreateGLContext(
          gl_share_group_.get(), surface_.get(), params.gpu_preference);
      gl_share_group_->SetSharedContext(context_.get());
    }

    context_ = new GLContextVirtual(
        gl_share_group_.get(), context_.get(), decoder_->AsWeakPtr());
    if (context_->Initialize(surface_.get(), params.gpu_preference)) {
      VLOG(1) << "Created virtual GL context.";
    } else {
      context_ = NULL;
    }
  } else {
    context_ = gfx::GLContext::CreateGLContext(
        gl_share_group_.get(), surface_.get(), params.gpu_preference);
  }

#ifdef TENCENT_CHANGES
  if (isSharedGroupFromSysContext) {
    gl_share_group_->RemoveContext(gfx::GLContext::android_sys_eglcontext);
  }
#endif

  if (!context_.get()) {
    LOG(ERROR) << "Could not create GLContext.";
    DestroyOnGpuThread();
    return false;
  }

  if (!context_->MakeCurrent(surface_.get())) {
    LOG(ERROR) << "Could not make context current.";
    DestroyOnGpuThread();
    return false;
  }

  gles2::DisallowedFeatures disallowed_features;
  disallowed_features.gpu_memory_manager = true;
  if (!decoder_->Initialize(surface_,
                            context_,
                            params.is_offscreen,
                            params.size,
                            disallowed_features,
                            params.attribs)) {
    LOG(ERROR) << "Could not initialize decoder.";
    DestroyOnGpuThread();
    return false;
  }
  *params.capabilities = decoder_->GetCapabilities();

  gpu_control_.reset(
      new GpuControlService(decoder_->GetContextGroup()->image_manager(),
                            decoder_->GetQueryManager()));

  if (!params.is_offscreen) {
    decoder_->SetResizeCallback(base::Bind(
        &InProcessCommandBuffer::OnResizeView, gpu_thread_weak_ptr_));
  }
  decoder_->SetWaitSyncPointCallback(base::Bind(&WaitSyncPoint));

  return true;
}

void InProcessCommandBuffer::Destroy() {
  CheckSequencedThread();

  base::WaitableEvent completion(true, false);
  bool result = false;
  base::Callback<bool(void)> destroy_task = base::Bind(
      &InProcessCommandBuffer::DestroyOnGpuThread, base::Unretained(this));
  QueueTask(
      base::Bind(&RunTaskWithResult<bool>, destroy_task, &result, &completion));
  completion.Wait();
}

bool InProcessCommandBuffer::DestroyOnGpuThread() {
  CheckSequencedThread();
  gpu_thread_weak_ptr_factory_.InvalidateWeakPtrs();
  command_buffer_.reset();
  // Clean up GL resources if possible.
  bool have_context = context_ && context_->MakeCurrent(surface_);
  if (decoder_) {
    decoder_->Destroy(have_context);
    decoder_.reset();
  }
  context_ = NULL;
  surface_ = NULL;
  gl_share_group_ = NULL;
#if defined(OS_ANDROID)
  stream_texture_manager_.reset();
#endif

  return true;
}

void InProcessCommandBuffer::CheckSequencedThread() {
  DCHECK(!sequence_checker_ ||
         sequence_checker_->CalledOnValidSequencedThread());
}

void InProcessCommandBuffer::OnContextLost() {
  CheckSequencedThread();
  if (!context_lost_callback_.is_null()) {
    context_lost_callback_.Run();
    context_lost_callback_.Reset();
  }

  context_lost_ = true;
}

CommandBuffer::State InProcessCommandBuffer::GetStateFast() {
  CheckSequencedThread();
  base::AutoLock lock(state_after_last_flush_lock_);
  if (state_after_last_flush_.generation - last_state_.generation < 0x80000000U)
    last_state_ = state_after_last_flush_;
  return last_state_;
}

CommandBuffer::State InProcessCommandBuffer::GetLastState() {
  CheckSequencedThread();
  return last_state_;
}

int32 InProcessCommandBuffer::GetLastToken() {
  CheckSequencedThread();
  GetStateFast();
  return last_state_.token;
}

void InProcessCommandBuffer::FlushOnGpuThread(int32 put_offset) {
  CheckSequencedThread();
  ScopedEvent handle_flush(&flush_event_);
  base::AutoLock lock(command_buffer_lock_);
  command_buffer_->Flush(put_offset);
  {
    // Update state before signaling the flush event.
    base::AutoLock lock(state_after_last_flush_lock_);
    state_after_last_flush_ = command_buffer_->GetLastState();
  }
  DCHECK((!error::IsError(state_after_last_flush_.error) && !context_lost_) ||
         (error::IsError(state_after_last_flush_.error) && context_lost_));

  // If we've processed all pending commands but still have pending queries,
  // pump idle work until the query is passed.
  if (put_offset == state_after_last_flush_.get_offset &&
      gpu_scheduler_->HasMoreWork()) {
    ScheduleIdleWorkOnGpuThread();
  }
}

void InProcessCommandBuffer::PerformIdleWork() {
  CheckSequencedThread();
  idle_work_pending_ = false;
  base::AutoLock lock(command_buffer_lock_);
  if (MakeCurrent() && gpu_scheduler_->HasMoreWork()) {
    gpu_scheduler_->PerformIdleWork();
    ScheduleIdleWorkOnGpuThread();
  }
}

void InProcessCommandBuffer::ScheduleIdleWorkOnGpuThread() {
  CheckSequencedThread();
  if (idle_work_pending_)
    return;
  idle_work_pending_ = true;
  service_->ScheduleIdleWork(
      base::Bind(&InProcessCommandBuffer::PerformIdleWork,
                 gpu_thread_weak_ptr_));
}

void InProcessCommandBuffer::Flush(int32 put_offset) {
  CheckSequencedThread();
  if (last_state_.error != gpu::error::kNoError)
    return;

  if (last_put_offset_ == put_offset)
    return;

  last_put_offset_ = put_offset;
  base::Closure task = base::Bind(&InProcessCommandBuffer::FlushOnGpuThread,
                                  gpu_thread_weak_ptr_,
                                  put_offset);
  QueueTask(task);
}

void InProcessCommandBuffer::WaitForTokenInRange(int32 start, int32 end) {
  CheckSequencedThread();
  while (!InRange(start, end, GetLastToken()) &&
         last_state_.error == gpu::error::kNoError)
    flush_event_.Wait();
}

void InProcessCommandBuffer::WaitForGetOffsetInRange(int32 start, int32 end) {
  CheckSequencedThread();

  GetStateFast();
  while (!InRange(start, end, last_state_.get_offset) &&
         last_state_.error == gpu::error::kNoError) {
    flush_event_.Wait();
    GetStateFast();
  }
}

void InProcessCommandBuffer::SetGetBuffer(int32 shm_id) {
  CheckSequencedThread();
  if (last_state_.error != gpu::error::kNoError)
    return;

  {
    base::AutoLock lock(command_buffer_lock_);
    command_buffer_->SetGetBuffer(shm_id);
    last_put_offset_ = 0;
  }
  {
    base::AutoLock lock(state_after_last_flush_lock_);
    state_after_last_flush_ = command_buffer_->GetLastState();
  }
}

scoped_refptr<Buffer> InProcessCommandBuffer::CreateTransferBuffer(size_t size,
                                                                   int32* id) {
  CheckSequencedThread();
  base::AutoLock lock(command_buffer_lock_);
  return command_buffer_->CreateTransferBuffer(size, id);
}

void InProcessCommandBuffer::DestroyTransferBuffer(int32 id) {
  CheckSequencedThread();
  base::Closure task =
      base::Bind(&InProcessCommandBuffer::DestroyTransferBufferOnGputhread,
                 base::Unretained(this),
                 id);

  QueueTask(task);
}

void InProcessCommandBuffer::DestroyTransferBufferOnGputhread(int32 id) {
  base::AutoLock lock(command_buffer_lock_);
  command_buffer_->DestroyTransferBuffer(id);
}

gpu::Capabilities InProcessCommandBuffer::GetCapabilities() {
  return capabilities_;
}

gfx::GpuMemoryBuffer* InProcessCommandBuffer::CreateGpuMemoryBuffer(
    size_t width,
    size_t height,
    unsigned internalformat,
    unsigned usage,
    int32* id) {
  CheckSequencedThread();

  *id = -1;
  linked_ptr<gfx::GpuMemoryBuffer> buffer =
      make_linked_ptr(g_gpu_memory_buffer_factory->CreateGpuMemoryBuffer(
          width, height, internalformat, usage));
  if (!buffer.get())
    return NULL;

  static int32 next_id = 1;
  *id = next_id++;

  base::Closure task = base::Bind(&GpuControlService::RegisterGpuMemoryBuffer,
                                  base::Unretained(gpu_control_.get()),
                                  *id,
                                  buffer->GetHandle(),
                                  width,
                                  height,
                                  internalformat);

  QueueTask(task);

  gpu_memory_buffers_[*id] = buffer;
  return buffer.get();
}

void InProcessCommandBuffer::DestroyGpuMemoryBuffer(int32 id) {
  CheckSequencedThread();
  GpuMemoryBufferMap::iterator it = gpu_memory_buffers_.find(id);
  if (it != gpu_memory_buffers_.end())
    gpu_memory_buffers_.erase(it);
  base::Closure task = base::Bind(&GpuControlService::UnregisterGpuMemoryBuffer,
                                  base::Unretained(gpu_control_.get()),
                                  id);

  QueueTask(task);
}

uint32 InProcessCommandBuffer::InsertSyncPoint() {
  uint32 sync_point = g_sync_point_manager.Get().GenerateSyncPoint();
  QueueTask(base::Bind(&InProcessCommandBuffer::RetireSyncPointOnGpuThread,
                       base::Unretained(this),
                       sync_point));
#ifdef TENCENT_CHANGES
  if (!base::android::SysUtils::RunningOnJellyBeanOrHigher())
    service_->AddPendingSyncPoint(sync_point);
#endif
  return sync_point;
}

void InProcessCommandBuffer::RetireSyncPointOnGpuThread(uint32 sync_point) {
  gles2::MailboxManager* mailbox_manager =
      decoder_->GetContextGroup()->mailbox_manager();
  if (mailbox_manager->UsesSync()) {
    bool make_current_success = false;
    {
      base::AutoLock lock(command_buffer_lock_);
      make_current_success = MakeCurrent();
    }
    if (make_current_success)
#ifdef TENCENT_CHANGES
      mailbox_manager->PushTextureUpdates(decoder_.get());
#else
      mailbox_manager->PushTextureUpdates();
#endif
  }
#ifdef TENCENT_CHANGES
  if (!base::android::SysUtils::RunningOnJellyBeanOrHigher() &&
		  !service_->RemovePendingSyncPoint(sync_point)) {
    return;
  }
#endif
  g_sync_point_manager.Get().RetireSyncPoint(sync_point);
}

void InProcessCommandBuffer::SignalSyncPoint(unsigned sync_point,
                                             const base::Closure& callback) {
  CheckSequencedThread();
  QueueTask(base::Bind(&InProcessCommandBuffer::SignalSyncPointOnGpuThread,
                       base::Unretained(this),
                       sync_point,
                       WrapCallback(callback)));
}

void InProcessCommandBuffer::SignalSyncPointOnGpuThread(
    unsigned sync_point,
    const base::Closure& callback) {
  if (g_sync_point_manager.Get().IsSyncPointPassed(sync_point)) {
    callback.Run();
  } else {
    service_->ScheduleIdleWork(
        base::Bind(&InProcessCommandBuffer::SignalSyncPointOnGpuThread,
                   gpu_thread_weak_ptr_,
                   sync_point,
                   callback));
  }
}

void InProcessCommandBuffer::SignalQuery(unsigned query,
                                         const base::Closure& callback) {
  CheckSequencedThread();
  QueueTask(base::Bind(&GpuControlService::SignalQuery,
                       base::Unretained(gpu_control_.get()),
                       query,
                       WrapCallback(callback)));
}

void InProcessCommandBuffer::SetSurfaceVisible(bool visible) {}

void InProcessCommandBuffer::Echo(const base::Closure& callback) {
  QueueTask(WrapCallback(callback));
}

uint32 InProcessCommandBuffer::CreateStreamTexture(uint32 texture_id) {
  base::WaitableEvent completion(true, false);
  uint32 stream_id = 0;
  base::Callback<uint32(void)> task =
      base::Bind(&InProcessCommandBuffer::CreateStreamTextureOnGpuThread,
                 base::Unretained(this),
                 texture_id);
  QueueTask(
      base::Bind(&RunTaskWithResult<uint32>, task, &stream_id, &completion));
  completion.Wait();
  return stream_id;
}

uint32 InProcessCommandBuffer::CreateStreamTextureOnGpuThread(
    uint32 client_texture_id) {
#if defined(OS_ANDROID)
  return stream_texture_manager_->CreateStreamTexture(
      client_texture_id, decoder_->GetContextGroup()->texture_manager());
#else
  return 0;
#endif
}

gpu::error::Error InProcessCommandBuffer::GetLastError() {
  CheckSequencedThread();
  return last_state_.error;
}

bool InProcessCommandBuffer::Initialize() {
  NOTREACHED();
  return false;
}

namespace {

void PostCallback(const scoped_refptr<base::MessageLoopProxy>& loop,
                         const base::Closure& callback) {
  if (!loop->BelongsToCurrentThread()) {
    loop->PostTask(FROM_HERE, callback);
  } else {
    callback.Run();
  }
}

void RunOnTargetThread(scoped_ptr<base::Closure> callback) {
  DCHECK(callback.get());
  callback->Run();
}

}  // anonymous namespace

base::Closure InProcessCommandBuffer::WrapCallback(
    const base::Closure& callback) {
  // Make sure the callback gets deleted on the target thread by passing
  // ownership.
  scoped_ptr<base::Closure> scoped_callback(new base::Closure(callback));
  base::Closure callback_on_client_thread =
      base::Bind(&RunOnTargetThread, base::Passed(&scoped_callback));
  base::Closure wrapped_callback =
      base::Bind(&PostCallback, base::MessageLoopProxy::current(),
                 callback_on_client_thread);
  return wrapped_callback;
}

#if defined(OS_ANDROID)
scoped_refptr<gfx::SurfaceTexture>
InProcessCommandBuffer::GetSurfaceTexture(uint32 stream_id) {
  DCHECK(stream_texture_manager_);
  return stream_texture_manager_->GetSurfaceTexture(stream_id);
}
#endif

// static
void InProcessCommandBuffer::SetGpuMemoryBufferFactory(
    GpuMemoryBufferFactory* factory) {
  g_gpu_memory_buffer_factory = factory;
}

#ifdef TENCENT_CHANGES
void InProcessCommandBuffer::RemoveSyncPoint(uint32 sync_point) {
  g_sync_point_manager.Get().RetireSyncPoint(sync_point);
}
#endif

}  // namespace gpu
