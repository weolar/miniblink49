// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/texture_definition.h"

#include <list>
#include <map>

#include "base/memory/linked_ptr.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#ifdef TENCENT_CHANGES
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/gles2_cmd_copy_texture_chromium.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/mailbox_synchronizer.h"
#include "gpu/config/gpu_bug_manager.h"
#endif
#include "gpu/command_buffer/service/texture_manager.h"
#include "ui/gl/gl_image.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/scoped_binders.h"

#if !defined(OS_MACOSX)
#include "ui/gl/gl_fence_egl.h"
#include "ui/gl/gl_surface_egl.h"
#endif

namespace gpu {
namespace gles2 {

namespace {

class GLImageSync : public gfx::GLImage {
 public:
  explicit GLImageSync(const scoped_refptr<NativeImageBuffer>& buffer,
                       const gfx::Size& size);

  // Implement GLImage.
  virtual void Destroy() OVERRIDE;
  virtual gfx::Size GetSize() OVERRIDE;
  virtual bool BindTexImage(unsigned target) OVERRIDE;
  virtual void ReleaseTexImage(unsigned target) OVERRIDE;
  virtual void WillUseTexImage() OVERRIDE;
  virtual void WillModifyTexImage() OVERRIDE;
  virtual void DidModifyTexImage() OVERRIDE;

  virtual void DidUseTexImage() OVERRIDE;
  virtual void SetReleaseAfterUse() OVERRIDE;

 protected:
  virtual ~GLImageSync();

 private:
  scoped_refptr<NativeImageBuffer> buffer_;
  gfx::Size size_;

  DISALLOW_COPY_AND_ASSIGN(GLImageSync);
};

GLImageSync::GLImageSync(const scoped_refptr<NativeImageBuffer>& buffer,
                         const gfx::Size& size)
    : buffer_(buffer), size_(size) {
  if (buffer)
    buffer->AddClient(this);
}

GLImageSync::~GLImageSync() {
  if (buffer_)
    buffer_->RemoveClient(this);
}

void GLImageSync::Destroy() {}

gfx::Size GLImageSync::GetSize() {
  return size_;
}

bool GLImageSync::BindTexImage(unsigned target) {
  NOTREACHED();
  return false;
}

void GLImageSync::ReleaseTexImage(unsigned target) {
  NOTREACHED();
}

void GLImageSync::WillUseTexImage() {
  if (buffer_)
    buffer_->WillRead(this);
}

void GLImageSync::DidUseTexImage() {
  if (buffer_)
    buffer_->DidRead(this);
}

void GLImageSync::WillModifyTexImage() {
  if (buffer_)
    buffer_->WillWrite(this);
}

void GLImageSync::DidModifyTexImage() {
  if (buffer_)
    buffer_->DidWrite(this);
}

void GLImageSync::SetReleaseAfterUse() {
  NOTREACHED();
}

#if !defined(OS_MACOSX)
class NativeImageBufferEGL : public NativeImageBuffer {
 public:
#ifdef TENCENT_CHANGES
  static scoped_refptr<NativeImageBufferEGL> Create(GLuint texture_id, gles2::GLES2Decoder* decoder);
#else
  static scoped_refptr<NativeImageBufferEGL> Create(GLuint texture_id);
#endif

 private:
  NativeImageBufferEGL(EGLDisplay display, EGLImageKHR image);
  virtual ~NativeImageBufferEGL();
  virtual void AddClient(gfx::GLImage* client) OVERRIDE;
  virtual void RemoveClient(gfx::GLImage* client) OVERRIDE;
  virtual bool IsClient(gfx::GLImage* client) OVERRIDE;
  virtual void BindToTexture(GLenum target) OVERRIDE;
  virtual void WillRead(gfx::GLImage* client) OVERRIDE;
  virtual void WillWrite(gfx::GLImage* client) OVERRIDE;
  virtual void DidRead(gfx::GLImage* client) OVERRIDE;
  virtual void DidWrite(gfx::GLImage* client) OVERRIDE;
#ifdef TENCENT_CHANGES
  virtual void DidLastWrite(gfx::GLImage* client) OVERRIDE;
  virtual void* getHandle(TargetName targe_name) {return egl_image_;}
#endif

  void ClearCompletedReadFencesLocked();

  EGLDisplay egl_display_;
  EGLImageKHR egl_image_;

  base::Lock lock_;

  struct ClientInfo {
    ClientInfo(gfx::GLImage* client);
    ~ClientInfo();

    gfx::GLImage* client;
    bool needs_wait_before_read;
    linked_ptr<gfx::GLFence> read_fence;
  };
  std::list<ClientInfo> client_infos_;
  scoped_ptr<gfx::GLFence> write_fence_;
  gfx::GLImage* write_client_;

  DISALLOW_COPY_AND_ASSIGN(NativeImageBufferEGL);
};

#ifdef TENCENT_CHANGES
scoped_refptr<NativeImageBufferEGL> NativeImageBufferEGL::Create(
    GLuint texture_id, gles2::GLES2Decoder* decoder) {
#else
scoped_refptr<NativeImageBufferEGL> NativeImageBufferEGL::Create(
	GLuint texture_id) {
#endif
  TRACE_EVENT0("gpu", "NativeImageBufferEGL::Create");
  EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();
  EGLContext egl_context = eglGetCurrentContext();

  DCHECK_NE(EGL_NO_CONTEXT, egl_context);
  DCHECK_NE(EGL_NO_DISPLAY, egl_display);
  DCHECK(glIsTexture(texture_id));

  DCHECK(gfx::g_driver_egl.ext.b_EGL_KHR_image_base &&
         gfx::g_driver_egl.ext.b_EGL_KHR_gl_texture_2D_image &&
         gfx::g_driver_gl.ext.b_GL_OES_EGL_image &&
         gfx::g_driver_egl.ext.b_EGL_KHR_fence_sync);

  const EGLint egl_attrib_list[] = {
      EGL_GL_TEXTURE_LEVEL_KHR, 0, EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE};
  EGLClientBuffer egl_buffer = reinterpret_cast<EGLClientBuffer>(texture_id);
  EGLenum egl_target = EGL_GL_TEXTURE_2D_KHR; // TODO

  EGLImageKHR egl_image = eglCreateImageKHR(
      egl_display, egl_context, egl_target, egl_buffer, egl_attrib_list);
  if (egl_image == EGL_NO_IMAGE_KHR)
    return NULL;

#ifdef TENCENT_CHANGES
  //在部分机型，如小米1中。在与image绑定的资源（纹理）未使用之前，重新改写源纹理，可能会导致错乱。
  //具体现象为花屏、闪烁，和浏览器UI界面文字黑块。
  //khronos网站中有文章说(https://www.khronos.org/registry/implementers_guide.html)，
  //EGLImage内存可以延时创建，使用时才创建。与我们遇到的现象基本吻合。
  //下面代码，创建临时对象，使用下EGLImage。以确保EGLImage完全创建。
  unsigned int tempTexture;
  char tmpData[4];
  glGenTextures(1, &tempTexture);
  gfx::ScopedTextureBinder texture_binder(GL_TEXTURE_2D, tempTexture);
  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, egl_image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // TODO(Perryuwang): 在PowerVR的机器上，如果Canvas的宽度都是2的n次方，
  //   glReadPixels操作异常耗时(高达10s左右)，间接导致UI线程卡死。
  //   所以，下面采用copy texture的方式替代glReadPixels，
  //   虽然可解决PowerVR卡死问题，但是仍然是有风险的。
  if (decoder) {
    TRACE_EVENT0("gpu", "NativeImageBufferEGL::Create. DoCopyTexture");
    GLuint destTexture;
    glGenTextures(1, &destTexture);
    glBindTexture(GL_TEXTURE_2D, destTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    CopyTextureCHROMIUMResourceManager* copy_texture_CHROMIUM = new CopyTextureCHROMIUMResourceManager();
    copy_texture_CHROMIUM->Initialize(decoder);

    copy_texture_CHROMIUM->DoCopyTexture(
	                      decoder,
	                      GL_TEXTURE_2D,
	                      GL_TEXTURE_2D,
	                      tempTexture,
	                      destTexture,
	                      0,
	                      1,
	                      1,
	                      false,
	                      false,
	                      false);
//    LOG(INFO) << "NativeImageBufferEGL::Create, glGetError:" << glGetError()
//              << ", tempTexture:" << tempTexture << ", destTexture:" << destTexture;
    delete copy_texture_CHROMIUM;
    glDeleteTextures(1, &destTexture);
    glFinish();
  } else {
    TRACE_EVENT0("gpu", "NativeImageBufferEGL::Create. glReadPixels");
    unsigned int tempFBO;
    glGenFramebuffersEXT(1, &tempFBO);
    gfx::ScopedFrameBufferBinder framebuffer_binder(tempFBO);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempTexture, 0);
    glReadPixels(0,0,1,1,GL_RGBA, GL_UNSIGNED_BYTE, tmpData);
    glDeleteFramebuffersEXT(1, &tempFBO);
  }

  glDeleteTextures(1, &tempTexture);
#endif

  return new NativeImageBufferEGL(egl_display, egl_image);
}

NativeImageBufferEGL::ClientInfo::ClientInfo(gfx::GLImage* client)
    : client(client), needs_wait_before_read(true) {}

NativeImageBufferEGL::ClientInfo::~ClientInfo() {}

NativeImageBufferEGL::NativeImageBufferEGL(EGLDisplay display,
                                           EGLImageKHR image)
    : NativeImageBuffer(),
      egl_display_(display),
      egl_image_(image),
#ifdef TENCENT_CHANGES
      write_fence_(new gfx::GLFenceEGL(true, GpuBugManager::GetInstance()->disable_egl_khr_client_wait_sync())),
#else
      write_fence_(new gfx::GLFenceEGL(true)),
#endif
      write_client_(NULL) {
  DCHECK(egl_display_ != EGL_NO_DISPLAY);
  DCHECK(egl_image_ != EGL_NO_IMAGE_KHR);
}

NativeImageBufferEGL::~NativeImageBufferEGL() {
  DCHECK(client_infos_.empty());
  if (egl_image_ != EGL_NO_IMAGE_KHR)
    eglDestroyImageKHR(egl_display_, egl_image_);
}

void NativeImageBufferEGL::AddClient(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  client_infos_.push_back(ClientInfo(client));
}

void NativeImageBufferEGL::RemoveClient(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  if (write_client_ == client)
    write_client_ = NULL;
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client) {
      if (it->read_fence.get()) {
        it->client = NULL;
      } else {
        client_infos_.erase(it);
      }
      ClearCompletedReadFencesLocked();
      return;
    }
  }
  NOTREACHED();
}

bool NativeImageBufferEGL::IsClient(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client)
      return true;
  }
  return false;
}

void NativeImageBufferEGL::BindToTexture(GLenum target) {
  DCHECK(egl_image_ != EGL_NO_IMAGE_KHR);
  glEGLImageTargetTexture2DOES(target, egl_image_);
  DCHECK_EQ(static_cast<EGLint>(EGL_SUCCESS), eglGetError());
  DCHECK_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
}

void NativeImageBufferEGL::WillRead(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  if (!write_fence_.get() || write_client_ == client)
    return;

  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client) {
      if (it->needs_wait_before_read) {
        it->needs_wait_before_read = false;
        write_fence_->ServerWait();
      }
      return;
    }
  }
  NOTREACHED();
}

void NativeImageBufferEGL::WillWrite(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  if (write_client_ != client)
    write_fence_->ServerWait();

  ClearCompletedReadFencesLocked();
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->read_fence.get() && it->client != client)
      it->read_fence->ServerWait();
  }
}

void NativeImageBufferEGL::DidRead(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  ClearCompletedReadFencesLocked();
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client) {
#ifdef TENCENT_CHANGES
      it->read_fence = make_linked_ptr(new gfx::GLFenceEGL(true, GpuBugManager::GetInstance()->disable_egl_khr_client_wait_sync()));
#else
      it->read_fence = make_linked_ptr(new gfx::GLFenceEGL(true));
#endif
      return;
    }
  }
  NOTREACHED();
}

void NativeImageBufferEGL::DidWrite(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  // Sharing semantics require the client to flush in order to make changes
  // visible to other clients.
#ifndef TENCENT_CHANGES
  write_fence_.reset(new gfx::GLFenceEGL(false));
#endif
  write_client_ = client;
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    it->needs_wait_before_read = true;
  }
}

void NativeImageBufferEGL::ClearCompletedReadFencesLocked() {
  lock_.AssertAcquired();
  std::list<ClientInfo>::iterator it = client_infos_.begin();
  while (it != client_infos_.end()) {
    if (!it->client && it->read_fence->HasCompleted()) {
      it = client_infos_.erase(it);
    } else {
      it++;
    }
  }
}

#ifdef TENCENT_CHANGES
void NativeImageBufferEGL::DidLastWrite(gfx::GLImage* client) {
  write_fence_.reset(new gfx::GLFenceEGL(false, GpuBugManager::GetInstance()->disable_egl_khr_client_wait_sync()));
  write_client_ = client;
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
		  it != client_infos_.end();
		  it++) {
    it->needs_wait_before_read = true;
  }
}

//NativeImageBufferCopyTexture是为了基于sharegroup的跨线程访问而引入的。
//功能：等每帧H5 Canvas绘制完成后，将绘制结果copy到另一张纹理上。并通过sharegroup共享给CC用。（src texture-》copy texture）
//read/write fence: 这里可能与chromium原有逻辑不太吻合。 write fence原义应该是用来判断src texture读写的。
//                  但是给CC线程读取的是copy texture，所以必须用write fence来保证读取安全。
//copy texture：由NativeImageBufferCopyTexture创建。为了保证写入安全，则必须持有一个引用计数。
//				每个mailbox，对因一个texture。可在MailboxToTextureMap表中查找。

class NativeImageBufferCopyTexture : public NativeImageBuffer {
 public:
  static scoped_refptr<NativeImageBuffer> Create(Texture* src_texture,
		  NativeImageBuffer::TargetName target_name,
		  gles2::GLES2Decoder* decoder);

 private:
  NativeImageBufferCopyTexture();
  virtual ~NativeImageBufferCopyTexture();
  virtual void AddClient(gfx::GLImage* client) OVERRIDE;
  virtual void RemoveClient(gfx::GLImage* client) OVERRIDE;
  virtual bool IsClient(gfx::GLImage* client) OVERRIDE;
  virtual void BindToTexture(GLenum target) OVERRIDE;
  virtual void WillRead(gfx::GLImage* client) OVERRIDE;
  virtual void WillWrite(gfx::GLImage* client) OVERRIDE;
  virtual void DidRead(gfx::GLImage* client) OVERRIDE;
  virtual void DidWrite(gfx::GLImage* client) OVERRIDE;
  virtual void DidLastWrite(gfx::GLImage* client) OVERRIDE;
  virtual Texture* UpdateCopyTexture(Texture* src_texture,
		  NativeImageBuffer::TargetName target_name,
		  gles2::GLES2Decoder* decoder) OVERRIDE;
  virtual void* getHandle(NativeImageBuffer::TargetName target_name);

  void ClearCompletedReadFencesLocked();

  base::Lock lock_;
  GLuint src_texture_;

  struct ClientInfo {
    ClientInfo(gfx::GLImage* client);
    ~ClientInfo();

    gfx::GLImage* client;
    bool needs_wait_before_read;
    linked_ptr<gfx::GLFence> read_fence;
  };
  std::list<ClientInfo> client_infos_;
  scoped_ptr<gfx::GLFence> write_fence_;
  gfx::GLImage* write_client_;
  scoped_ptr<CopyTextureCHROMIUMResourceManager> copy_texture_CHROMIUM_;

  DISALLOW_COPY_AND_ASSIGN(NativeImageBufferCopyTexture);
};

scoped_refptr<NativeImageBuffer> NativeImageBufferCopyTexture::Create(Texture* src_texture,
		NativeImageBuffer::TargetName target_name,
		gles2::GLES2Decoder* decoder) {
  NativeImageBufferCopyTexture* iamge_buffer = new NativeImageBufferCopyTexture();
  if (!iamge_buffer)
	  return NULL;
  iamge_buffer->UpdateCopyTexture(src_texture, target_name, decoder);
  return iamge_buffer;
}

NativeImageBufferCopyTexture::ClientInfo::ClientInfo(gfx::GLImage* client)
    : client(client), needs_wait_before_read(true) {}

NativeImageBufferCopyTexture::ClientInfo::~ClientInfo() {}

NativeImageBufferCopyTexture::NativeImageBufferCopyTexture()
    : NativeImageBuffer(),
      src_texture_(0),
      write_fence_(new gfx::GLFenceEGL(true, GpuBugManager::GetInstance()->disable_egl_khr_client_wait_sync())),
      write_client_(NULL){

}

NativeImageBufferCopyTexture::~NativeImageBufferCopyTexture() {
  DCHECK(client_infos_.empty());
}

Texture* NativeImageBufferCopyTexture::UpdateCopyTexture(Texture* src_texture,
		NativeImageBuffer::TargetName target_name,
		gles2::GLES2Decoder* decoder) {
  if (!decoder)
    return NULL;

  GLuint texture_id;
  GLenum type;
  GLenum internal_format;
  GLsizei width, height;
  src_texture->GetLevelType(src_texture->target(), 0, &type, &internal_format);
  src_texture->GetLevelSize(src_texture->target(), 0, &width, &height);

  CopyTexturesManager* copy_texture_manager =
      		MailboxSynchronizer::GetInstance()->GetCopyTexturesManager();
  scoped_refptr<TextureRef> texture = copy_texture_manager->FindTexture(target_name.target, target_name.mailbox);
  if (texture.get()) {
    texture_id = texture->texture()->service_id();
  } else {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, src_texture->min_filter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, src_texture->mag_filter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, src_texture->wrap_s());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, src_texture->wrap_t());
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
	    		width, height, 0, internal_format, type, NULL);
    TextureManager* texture_manager = decoder->GetContextGroup()->texture_manager();
    texture = TextureRef::Create(texture_manager, 0, texture_id);
    copy_texture_manager->AddTexture(target_name.target, target_name.mailbox, texture);
  }

  if (!copy_texture_CHROMIUM_.get()) {
    copy_texture_CHROMIUM_.reset(new CopyTextureCHROMIUMResourceManager());
    copy_texture_CHROMIUM_->Initialize(decoder);
  }

  copy_texture_CHROMIUM_->DoCopyTexture(
                    decoder,
                    src_texture->target(),
                    src_texture->target(),
                    src_texture->service_id(),
                    texture_id,
                    0,
                    width,
                    height,
                    false,
                    false,
                    false);
  write_fence_.reset(new gfx::GLFenceEGL(true, GpuBugManager::GetInstance()->disable_egl_khr_client_wait_sync()));
  return texture->texture();
}

void* NativeImageBufferCopyTexture::getHandle(NativeImageBuffer::TargetName target_name) {
  CopyTexturesManager* copy_texture_manager =
		  MailboxSynchronizer::GetInstance()->GetCopyTexturesManager();
  scoped_refptr<TextureRef> texture =
		  copy_texture_manager->FindTexture(target_name.target, target_name.mailbox);
  if (texture.get())
    return texture->texture();
  return NULL;
}

void NativeImageBufferCopyTexture::AddClient(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  client_infos_.push_back(ClientInfo(client));
}

void NativeImageBufferCopyTexture::RemoveClient(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  if (write_client_ == client) {
    write_client_ = NULL;
  }
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client) {
      if (it->read_fence.get()) {
        it->client = NULL;
      } else {
        client_infos_.erase(it);
      }
      ClearCompletedReadFencesLocked();
      break;
    }
  }
  NOTREACHED();
}

bool NativeImageBufferCopyTexture::IsClient(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client)
      return true;
  }
  return false;
}

void NativeImageBufferCopyTexture::BindToTexture(GLenum target) {
  NOTREACHED();
}

void NativeImageBufferCopyTexture::WillRead(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  if (!write_fence_.get() || write_client_ == client)
    return;

  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client) {
      if (it->needs_wait_before_read) {
        it->needs_wait_before_read = false;
        write_fence_->ServerWait();
      }
      return;
    }
  }
  NOTREACHED();
}

void NativeImageBufferCopyTexture::WillWrite(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  if (write_client_ != client)
    write_fence_->ServerWait();

  ClearCompletedReadFencesLocked();
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->read_fence.get() && it->client != client)
      it->read_fence->ServerWait();
  }
}

void NativeImageBufferCopyTexture::DidRead(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  ClearCompletedReadFencesLocked();
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    if (it->client == client) {
      it->read_fence = make_linked_ptr(new gfx::GLFenceEGL(true, GpuBugManager::GetInstance()->disable_egl_khr_client_wait_sync()));
      return;
    }
  }
  NOTREACHED();
}

void NativeImageBufferCopyTexture::DidWrite(gfx::GLImage* client) {
  base::AutoLock lock(lock_);
  write_client_ = client;
  for (std::list<ClientInfo>::iterator it = client_infos_.begin();
       it != client_infos_.end();
       it++) {
    it->needs_wait_before_read = true;
  }
}

void NativeImageBufferCopyTexture::DidLastWrite(gfx::GLImage* client) {
}


void NativeImageBufferCopyTexture::ClearCompletedReadFencesLocked() {
  lock_.AssertAcquired();
  std::list<ClientInfo>::iterator it = client_infos_.begin();
  while (it != client_infos_.end()) {
    if (!it->client && it->read_fence->HasCompleted()) {
      it = client_infos_.erase(it);
    } else {
      it++;
    }
  }
}
#endif

#endif

class NativeImageBufferStub : public NativeImageBuffer {
 public:
  NativeImageBufferStub() : NativeImageBuffer() {}

 private:
  virtual ~NativeImageBufferStub() {}
  virtual void AddClient(gfx::GLImage* client) OVERRIDE {}
  virtual void RemoveClient(gfx::GLImage* client) OVERRIDE {}
  virtual bool IsClient(gfx::GLImage* client) OVERRIDE { return true; }
  virtual void BindToTexture(GLenum target) OVERRIDE {}
  virtual void WillRead(gfx::GLImage* client) OVERRIDE {}
  virtual void WillWrite(gfx::GLImage* client) OVERRIDE {}
  virtual void DidRead(gfx::GLImage* client) OVERRIDE {}
  virtual void DidWrite(gfx::GLImage* client) OVERRIDE {}

  DISALLOW_COPY_AND_ASSIGN(NativeImageBufferStub);
};

}  // anonymous namespace

// static
#ifdef TENCENT_CHANGES
scoped_refptr<NativeImageBuffer> NativeImageBuffer::Create(GLuint texture_id, gles2::GLES2Decoder* decoder) {
#else
scoped_refptr<NativeImageBuffer> NativeImageBuffer::Create(GLuint texture_id) {
#endif
  switch (gfx::GetGLImplementation()) {
#if !defined(OS_MACOSX)
    case gfx::kGLImplementationEGLGLES2:
#ifdef TENCENT_CHANGES
      return NativeImageBufferEGL::Create(texture_id, decoder);
#else
      return NativeImageBufferEGL::Create(texture_id);
#endif // TENCENT_CHANGES
#endif
    case gfx::kGLImplementationMockGL:
      return new NativeImageBufferStub;
    default:
      NOTREACHED();
      return NULL;
  }
}

TextureDefinition::LevelInfo::LevelInfo(GLenum target,
                                        GLenum internal_format,
                                        GLsizei width,
                                        GLsizei height,
                                        GLsizei depth,
                                        GLint border,
                                        GLenum format,
                                        GLenum type,
                                        bool cleared)
    : target(target),
      internal_format(internal_format),
      width(width),
      height(height),
      depth(depth),
      border(border),
      format(format),
      type(type),
      cleared(cleared) {}

TextureDefinition::LevelInfo::~LevelInfo() {}

#ifdef TENCENT_CHANGES
TextureDefinition::TextureDefinition(
    GLenum target,
    const Mailbox& mailbox,
    Texture* texture,
    unsigned int version,
    const scoped_refptr<NativeImageBuffer>& image_buffer,
    gles2::GLES2Decoder* decoder)
    : version_(version),
      target_(target),
      image_buffer_(image_buffer),
      min_filter_(texture->min_filter()),
      mag_filter_(texture->mag_filter()),
      wrap_s_(texture->wrap_s()),
      wrap_t_(texture->wrap_t()),
      usage_(texture->usage()),
      immutable_(texture->IsImmutable()) {
#else
TextureDefinition::TextureDefinition(
    GLenum target,
    Texture* texture,
    unsigned int version,
    const scoped_refptr<NativeImageBuffer>& image_buffer)
    : version_(version),
      target_(target),
      image_buffer_(image_buffer ? image_buffer : NativeImageBuffer::Create(
                                                      texture->service_id())),
      min_filter_(texture->min_filter()),
      mag_filter_(texture->mag_filter()),
      wrap_s_(texture->wrap_s()),
      wrap_t_(texture->wrap_t()),
      usage_(texture->usage()),
      immutable_(texture->IsImmutable()) {
#endif

  // TODO
  DCHECK(!texture->level_infos_.empty());
  DCHECK(!texture->level_infos_[0].empty());
  DCHECK(!texture->NeedsMips());
  DCHECK(texture->level_infos_[0][0].width);
  DCHECK(texture->level_infos_[0][0].height);

#ifdef TENCENT_CHANGES
  NativeImageBuffer::TargetName target_name(target, mailbox);
  if (!image_buffer) {
    if (MailboxSynchronizer::GetInstance()->IsUseShareGroup()) {
    	image_buffer_ = NativeImageBufferCopyTexture::Create(texture, target_name, decoder);
    } else {
    	image_buffer_ = NativeImageBuffer::Create(texture->service_id(), decoder);
    }
  }
#endif
  scoped_refptr<gfx::GLImage> gl_image(
      new GLImageSync(image_buffer_,
                      gfx::Size(texture->level_infos_[0][0].width,
                                texture->level_infos_[0][0].height)));

  texture->SetLevelImage(NULL, target, 0, gl_image);

  // TODO: all levels
  level_infos_.clear();
  const Texture::LevelInfo& level = texture->level_infos_[0][0];
  LevelInfo info(level.target,
                 level.internal_format,
                 level.width,
                 level.height,
                 level.depth,
                 level.border,
                 level.format,
                 level.type,
                 level.cleared);
  std::vector<LevelInfo> infos;
  infos.push_back(info);
  level_infos_.push_back(infos);
}

TextureDefinition::~TextureDefinition() {
}

#ifdef TENCENT_CHANGES
Texture* TextureDefinition::CreateTexture(GLenum target, const Mailbox& mailbox) const {
#else
Texture* TextureDefinition::CreateTexture() const {
#endif
  if (!image_buffer_)
    return NULL;

#ifdef TENCENT_CHANGES
  if (MailboxSynchronizer::GetInstance()->IsUseShareGroup()) {
    NativeImageBuffer::TargetName target_name(target, mailbox);
    Texture* texture = static_cast<Texture*>(image_buffer_->getHandle(target_name));
    if (!texture) {
      return NULL;
    }

    UpdateTexture(texture);
    return texture;
  }
#endif

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  Texture* texture(new Texture(texture_id));
  UpdateTexture(texture);

  return texture;
}

void TextureDefinition::UpdateTexture(Texture* texture) const {
#ifdef TENCENT_CHANGES
  if (!MailboxSynchronizer::GetInstance()->IsUseShareGroup())
#endif
  {
    gfx::ScopedTextureBinder texture_binder(target_, texture->service_id());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t_);
    if (image_buffer_)
      image_buffer_->BindToTexture(target_);
    // We have to make sure the changes are visible to other clients in this share
    // group. As far as the clients are concerned, the mailbox semantics only
    // demand a single flush from the client after changes are first made,
    // and it is not visible to them when another share group boundary is crossed.
    // We could probably track this and be a bit smarter about when to flush
    // though.
    glFlush();
  }

  texture->level_infos_.resize(1);
  for (size_t i = 0; i < level_infos_.size(); i++) {
    const LevelInfo& base_info = level_infos_[i][0];
    const size_t levels_needed = TextureManager::ComputeMipMapCount(
        base_info.target, base_info.width, base_info.height, base_info.depth);
    DCHECK(level_infos_.size() <= levels_needed);
    texture->level_infos_[0].resize(levels_needed);
    for (size_t n = 0; n < level_infos_.size(); n++) {
      const LevelInfo& info = level_infos_[i][n];
#ifdef TENCENT_CHANGES
      if (MailboxSynchronizer::GetInstance()->IsUseShareGroup())
        texture->GetMemTracker()->TrackMemFree(texture->estimated_size());
#endif
      texture->SetLevelInfo(NULL,
                            info.target,
                            i,
                            info.internal_format,
                            info.width,
                            info.height,
                            info.depth,
                            info.border,
                            info.format,
                            info.type,
                            info.cleared);
#ifdef TENCENT_CHANGES
      if (MailboxSynchronizer::GetInstance()->IsUseShareGroup())
        texture->GetMemTracker()->TrackMemAlloc(texture->estimated_size());
#endif
    }
  }
  if (image_buffer_) {
    texture->SetLevelImage(
        NULL,
        target_,
        0,
        new GLImageSync(
            image_buffer_,
            gfx::Size(level_infos_[0][0].width, level_infos_[0][0].height)));
  }

  texture->target_ = target_;
  texture->SetImmutable(immutable_);
  texture->min_filter_ = min_filter_;
  texture->mag_filter_ = mag_filter_;
  texture->wrap_s_ = wrap_s_;
  texture->wrap_t_ = wrap_t_;
  texture->usage_ = usage_;
}

bool TextureDefinition::Matches(const Texture* texture) const {
  DCHECK(target_ == texture->target());
  if (texture->min_filter_ != min_filter_ ||
      texture->mag_filter_ != mag_filter_ ||
      texture->wrap_s_ != wrap_s_ ||
      texture->wrap_t_ != wrap_t_) {
    return false;
  }

  // All structural changes should have orphaned the texture.
  if (image_buffer_ && !texture->GetLevelImage(texture->target(), 0))
    return false;

  return true;
}

}  // namespace gles2
}  // namespace gpu
