// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_TEXTURE_DEFINITION_H_
#define GPU_COMMAND_BUFFER_SERVICE_TEXTURE_DEFINITION_H_

#include <vector>

#include "base/memory/ref_counted.h"
#include "gpu/command_buffer/service/gl_utils.h"
#ifdef TENCENT_CHANGES
#include "gpu/command_buffer/common/mailbox.h"
#endif

namespace gfx {
class GLImage;
}

namespace gpu {
namespace gles2 {

class Texture;
#ifdef TENCENT_CHANGES
class GLES2Decoder;
#endif

class NativeImageBuffer : public base::RefCountedThreadSafe<NativeImageBuffer> {
 public:
#ifdef TENCENT_CHANGES
  static scoped_refptr<NativeImageBuffer> Create(GLuint texture_id, gles2::GLES2Decoder* decoder);
#else
  static scoped_refptr<NativeImageBuffer> Create(GLuint texture_id);
#endif

  virtual void AddClient(gfx::GLImage* client) = 0;
  virtual void RemoveClient(gfx::GLImage* client) = 0;
  virtual bool IsClient(gfx::GLImage* client) = 0;
  virtual void BindToTexture(GLenum target) = 0;
  virtual void WillRead(gfx::GLImage* client) = 0;
  virtual void WillWrite(gfx::GLImage* client) = 0;
  virtual void DidRead(gfx::GLImage* client) = 0;
  virtual void DidWrite(gfx::GLImage* client) = 0;

#ifdef TENCENT_CHANGES
  //增加这个函数是为了解决频繁创建EGLFence耗时的问题。不在DidWrite中频繁创建，而是每帧只在DidLastWrite中创建一次。
  virtual void DidLastWrite(gfx::GLImage* client) {}

  struct TargetName {
    TargetName(unsigned target, const Mailbox& mailbox)
    : target(target), mailbox(mailbox) {}

    bool operator<(const TargetName& rhs) const {
      return memcmp(this, &rhs, sizeof(rhs)) < 0;
    }
    bool operator!=(const TargetName& rhs) const {
      return memcmp(this, &rhs, sizeof(rhs)) != 0;
    }
    bool operator==(const TargetName& rhs) const {
      return !operator!=(rhs);
    }
    unsigned target;
    Mailbox mailbox;
  };

  virtual void* getHandle(TargetName target_name) {return NULL;}

  virtual Texture* UpdateCopyTexture(Texture* src_texture, TargetName target_name,
		  gles2::GLES2Decoder* decoder) {return 0;}
#endif

 protected:
  friend class base::RefCountedThreadSafe<NativeImageBuffer>;
  NativeImageBuffer() {}
  virtual ~NativeImageBuffer() {}

  DISALLOW_COPY_AND_ASSIGN(NativeImageBuffer);
};

// An immutable description that can be used to create a texture that shares
// the underlying image buffer(s).
class TextureDefinition {
 public:
#ifdef TENCENT_CHANGES
  TextureDefinition(GLenum target,
		  	  	  	const Mailbox& mailbox,
                    Texture* texture,
                    unsigned int version,
                    const scoped_refptr<NativeImageBuffer>& image,
                    gles2::GLES2Decoder* decoder);
#else
  TextureDefinition(GLenum target,
                    Texture* texture,
                    unsigned int version,
                    const scoped_refptr<NativeImageBuffer>& image);
#endif
  virtual ~TextureDefinition();

#ifdef TENCENT_CHANGES
  Texture* CreateTexture(GLenum target, const Mailbox& mailbox) const;
#else
  Texture* CreateTexture() const;
#endif
  void UpdateTexture(Texture* texture) const;

  unsigned int version() const { return version_; }
  bool IsOlderThan(unsigned int version) const {
    return (version - version_) < 0x80000000;
  }
  bool Matches(const Texture* texture) const;

  scoped_refptr<NativeImageBuffer> image() { return image_buffer_; }

 private:
  struct LevelInfo {
    LevelInfo(GLenum target,
              GLenum internal_format,
              GLsizei width,
              GLsizei height,
              GLsizei depth,
              GLint border,
              GLenum format,
              GLenum type,
              bool cleared);
    ~LevelInfo();

    GLenum target;
    GLenum internal_format;
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    GLint border;
    GLenum format;
    GLenum type;
    bool cleared;
  };

  typedef std::vector<std::vector<LevelInfo> > LevelInfos;

  unsigned int version_;
  GLenum target_;
  scoped_refptr<NativeImageBuffer> image_buffer_;
  GLenum min_filter_;
  GLenum mag_filter_;
  GLenum wrap_s_;
  GLenum wrap_t_;
  GLenum usage_;
  bool immutable_;
  LevelInfos level_infos_;
};

}  // namespage gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_TEXTURE_DEFINITION_H_
