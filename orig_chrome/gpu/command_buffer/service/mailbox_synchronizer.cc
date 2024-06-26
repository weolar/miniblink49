// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/mailbox_synchronizer.h"

#if 0 // def TENCENT_CHANGES
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/config/gpu_info.h"
//#include "content/browser/gpu/gpu_data_manager_impl.h"
//#include "base/android/sys_utils.h"
//#include "gpu/config/gpu_bug_manager.h"
#endif
#include "base/bind.h"
#include "gpu/command_buffer/service/mailbox_manager.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "ui/gl/gl_implementation.h"

namespace gpu {
namespace gles2 {

    namespace {

        MailboxSynchronizer* g_instance = NULL;

    } // anonymous namespace

#if 0 // def TENCENT_CHANGES
class CopyTexturesManagerImpl:
	public CopyTexturesManager, TextureManager::DestructionObserver {
public:
  CopyTexturesManagerImpl(){}
  virtual ~CopyTexturesManagerImpl(){
    CHECK(textures_.size() == 0);
    CHECK(textures_will_deleted_.size() == 0);
  }

  //TextureManager::DestructionObserver
  virtual void OnTextureManagerDestroying(TextureManager* manager);
  virtual void OnTextureRefDestroying(TextureRef* texture) {}

  //CopyTexturesManager
  virtual int DeleteUnusedTexture(TextureManager* manager);
  virtual void AddTexture(unsigned target, const Mailbox& mailbox, scoped_refptr<TextureRef> texture);
  virtual scoped_refptr<TextureRef> FindTexture(unsigned target, const Mailbox& mailbox);
  virtual void MailboxDeleted(unsigned target, const Mailbox& mailbox);
private:
  typedef std::multimap<TextureManager*, scoped_refptr<TextureRef> > TextureManagerToTextureMap;
  typedef std::map<NativeImageBuffer::TargetName, scoped_refptr<TextureRef> > MailboxToTextureMap;

  //需要释放的texture，在合适的时候，去释放它。保存在这里是因为它们可能属于不同的TextureManager。需要在不同的context中释放。
  TextureManagerToTextureMap textures_will_deleted_;

  //每个mailbox都对应一个texture
  MailboxToTextureMap textures_;

  base::Lock lock_;
};

void CopyTexturesManagerImpl::OnTextureManagerDestroying(TextureManager* manager) {
  base::AutoLock lock(lock_);
  std::pair<TextureManagerToTextureMap::iterator,
  	  	  TextureManagerToTextureMap::iterator> range =
  	  			textures_will_deleted_.equal_range(manager);
  textures_will_deleted_.erase(range.first, range.second);
}

int CopyTexturesManagerImpl::DeleteUnusedTexture(TextureManager* manager) {
  TextureManagerToTextureMap deleted_map;
  {
    base::AutoLock lock(lock_);
    std::pair<TextureManagerToTextureMap::iterator,
  	  	    TextureManagerToTextureMap::iterator> range =
  	  			  textures_will_deleted_.equal_range(manager);
    deleted_map.insert(range.first, range.second);
    textures_will_deleted_.erase(range.first, range.second);
    manager->RemoveObserver(this);
  }
  deleted_map.clear();

  return textures_will_deleted_.size();
}

void CopyTexturesManagerImpl::AddTexture(unsigned target, const Mailbox& mailbox, scoped_refptr<TextureRef> texture) {
  base::AutoLock lock(lock_);
  NativeImageBuffer::TargetName target_name(target, mailbox);
  DCHECK(textures_.find(target_name) == textures_.end());

  textures_.insert(std::make_pair(target_name, texture));
}

scoped_refptr<TextureRef> CopyTexturesManagerImpl::FindTexture(unsigned target, const Mailbox& mailbox) {
  NativeImageBuffer::TargetName target_name(target, mailbox);
  MailboxToTextureMap::iterator it = textures_.find(target_name);
  if (it != textures_.end())
    return it->second;
  return NULL;
}

void CopyTexturesManagerImpl::MailboxDeleted(unsigned target, const Mailbox& mailbox) {
  NativeImageBuffer::TargetName target_name(target, mailbox);
  MailboxToTextureMap::iterator it = textures_.find(target_name);
  if (it != textures_.end()) {
    base::AutoLock lock(lock_);
    scoped_refptr<TextureRef> texture = it->second;
    TextureManager* manager = texture->manager();

    //第一次有manager的纹理在删除队列中。
    if (textures_will_deleted_.find(manager) == textures_will_deleted_.end()) {
      manager->AddObserver(this);
    }

    textures_will_deleted_.insert(std::make_pair(manager, texture));
    textures_.erase(it);
  }
}

#endif

    // static
    bool MailboxSynchronizer::Initialize()
    {
        DCHECK(!g_instance);
        DCHECK(gfx::GetGLImplementation() != gfx::kGLImplementationNone)
            << "GL bindings not initialized";
#if 0 // def TENCENT_CHANGES
  bool use_share_group = false;
  switch (gfx::GetGLImplementation()) {
    case gfx::kGLImplementationMockGL:
      break;
    case gfx::kGLImplementationEGLGLES2:
      {
    	bool disable_sharegroup_texture_access = false;

        if (!gfx::g_driver_egl.ext.b_EGL_KHR_image_base ||
            !gfx::g_driver_egl.ext.b_EGL_KHR_gl_texture_2D_image ||
            !gfx::g_driver_gl.ext.b_GL_OES_EGL_image ||
            !gfx::g_driver_egl.ext.b_EGL_KHR_fence_sync ||
            GpuBugManager::GetInstance()->disable_egl_image_texture_access()) {
          LOG(WARNING) << "MailboxSync not supported due to missing EGL "
                          "image/fence support";
          //TODO：这里应该还需要判断是否支持share_group。目前的流程是如果不支持，则会创建context失败，走到失败处理流程，并走软绘。
          if (gfx::g_driver_egl.ext.b_EGL_KHR_fence_sync &&
        		  !base::android::SysUtils::RunningOnLollipopOrHigher() &&
        		  !disable_sharegroup_texture_access) {
            use_share_group = true;
          } else {
            return false;
          }
        }
      }
      break;
    default:
      NOTREACHED();
      return false;
  }
#else
        switch (gfx::GetGLImplementation()) {
        case gfx::kGLImplementationMockGL:
            break;
        case gfx::kGLImplementationEGLGLES2:
#if !defined(OS_MACOSX)
        {
            if (!gfx::g_driver_egl.ext.b_EGL_KHR_image_base || !gfx::g_driver_egl.ext.b_EGL_KHR_gl_texture_2D_image || !gfx::g_driver_gl.ext.b_GL_OES_EGL_image || !gfx::g_driver_egl.ext.b_EGL_KHR_fence_sync) {
                LOG(WARNING) << "MailboxSync not supported due to missing EGL "
                                "image/fence support";
                return false;
            }
        } break;
#endif
        default:
            NOTREACHED();
            return false;
        }
#endif
        g_instance = new MailboxSynchronizer;
#if 0 // def TENCENT_CHANGES
  g_instance->SetUseShareGroup(use_share_group);
#endif
        return true;
    }

    // static
    void MailboxSynchronizer::Terminate()
    {
        DCHECK(g_instance);
        delete g_instance;
        g_instance = NULL;
    }

    // static
    MailboxSynchronizer* MailboxSynchronizer::GetInstance()
    {
        return g_instance;
    }

    MailboxSynchronizer::TargetName::TargetName(unsigned target,
        const Mailbox& mailbox)
        : target(target)
        , mailbox(mailbox)
    {
    }

    MailboxSynchronizer::TextureGroup::TextureGroup(
        const TextureDefinition& definition)
        : definition(definition)
    {
    }

    MailboxSynchronizer::TextureGroup::~TextureGroup() { }

    MailboxSynchronizer::TextureVersion::TextureVersion(
        linked_ptr<TextureGroup> group)
        : version(group->definition.version())
        , group(group)
    {
    }

    MailboxSynchronizer::TextureVersion::~TextureVersion() { }

#if 0 // def TENCENT_CHANGES
MailboxSynchronizer::MailboxSynchronizer()
  :copy_textures_manager_(new CopyTexturesManagerImpl)
{}
#else
    MailboxSynchronizer::MailboxSynchronizer()
    {
    }
#endif

    MailboxSynchronizer::~MailboxSynchronizer()
    {
        DCHECK_EQ(0U, textures_.size());
    }

    void MailboxSynchronizer::ReassociateMailboxLocked(
        const TargetName& target_name,
        TextureGroup* group)
    {
        lock_.AssertAcquired();
        for (TextureMap::iterator it = textures_.begin(); it != textures_.end();
             it++) {
            std::set<TargetName>::iterator mb_it = it->second.group->mailboxes.find(target_name);
            if (it->second.group != group && mb_it != it->second.group->mailboxes.end()) {
                it->second.group->mailboxes.erase(mb_it);
            }
        }
        group->mailboxes.insert(target_name);
    }

    linked_ptr<MailboxSynchronizer::TextureGroup>
    MailboxSynchronizer::GetGroupForMailboxLocked(const TargetName& target_name)
    {
        lock_.AssertAcquired();
        for (TextureMap::iterator it = textures_.begin(); it != textures_.end();
             it++) {
            std::set<TargetName>::const_iterator mb_it = it->second.group->mailboxes.find(target_name);
            if (mb_it != it->second.group->mailboxes.end())
                return it->second.group;
        }
        return make_linked_ptr<MailboxSynchronizer::TextureGroup>(NULL);
    }

    Texture* MailboxSynchronizer::CreateTextureFromMailbox(unsigned target,
        const Mailbox& mailbox)
    {
        base::AutoLock lock(lock_);
        TargetName target_name(target, mailbox);
        linked_ptr<TextureGroup> group = GetGroupForMailboxLocked(target_name);
        if (group.get()) {
#if 0 // def TENCENT_CHANGES
    Texture* new_texture = group->definition.CreateTexture(target, mailbox);
#else
            Texture* new_texture = group->definition.CreateTexture();
#endif
            if (new_texture)
                textures_.insert(std::make_pair(new_texture, TextureVersion(group)));
            return new_texture;
        }

        return NULL;
    }

    void MailboxSynchronizer::TextureDeleted(Texture* texture)
    {
        base::AutoLock lock(lock_);
        TextureMap::iterator it = textures_.find(texture);
        if (it != textures_.end()) {
            // TODO: We could avoid the update if this was the last ref.
            UpdateTextureLocked(it->first, it->second);
            textures_.erase(it);
        }
    }

#if 0 // def TENCENT_CHANGES
void MailboxSynchronizer::UpdateTexture(unsigned target, const Mailbox& mailbox, Texture* texture, gles2::GLES2Decoder* decoder) {
  if (!use_sharegroup_)
    return;
  DCHECK(decoder);

  NativeImageBuffer::TargetName target_name(target, mailbox);
  TextureMap::iterator it = textures_.find(texture);
  if (it != textures_.end()) {
    TextureVersion& texture_version = it->second;
    TextureGroup* group = texture_version.group.get();
    scoped_refptr<NativeImageBuffer> image_buffer = group->definition.image();
    image_buffer->UpdateCopyTexture(texture, target_name, decoder);
  }

  if (copy_textures_manager_) {
    TextureManager* texture_manager = decoder->GetContextGroup()->texture_manager();
    copy_textures_manager_->DeleteUnusedTexture(texture_manager);
  }
}

void MailboxSynchronizer::PushTextureUpdates(MailboxManager* manager, gles2::GLES2Decoder* decoder) {
#else
    void MailboxSynchronizer::PushTextureUpdates(MailboxManager* manager)
    {
#endif
    base::AutoLock lock(lock_);
    for (MailboxManager::MailboxToTextureMap::const_iterator texture_it = manager->mailbox_to_textures_.begin();
         texture_it != manager->mailbox_to_textures_.end();
         texture_it++) {
        TargetName target_name(texture_it->first.target, texture_it->first.mailbox);
        Texture* texture = texture_it->second->first;
        // TODO(sievers): crbug.com/352274
        // Should probably only fail if it already *has* mipmaps, while allowing
        // incomplete textures here. Also reconsider how to fail otherwise.
        bool needs_mips = texture->min_filter() != GL_NEAREST && texture->min_filter() != GL_LINEAR;
        if (target_name.target != GL_TEXTURE_2D || needs_mips)
            continue;

        TextureMap::iterator it = textures_.find(texture);
        if (it != textures_.end()) {
            TextureVersion& texture_version = it->second;
            TextureGroup* group = texture_version.group.get();
            std::set<TargetName>::const_iterator mb_it = group->mailboxes.find(target_name);
            if (mb_it == group->mailboxes.end()) {
                // We previously did not associate this texture with the given mailbox.
                // Unlink other texture groups from the mailbox.
                ReassociateMailboxLocked(target_name, group);
            }
            UpdateTextureLocked(texture, texture_version);
        } else {
            // Skip compositor resources/tile textures.
            // TODO: Remove this, see crbug.com/399226.
            if (texture->pool() == GL_TEXTURE_POOL_MANAGED_CHROMIUM)
                continue;

#if 0 // def TENCENT_CHANGES
      linked_ptr<TextureGroup> group = make_linked_ptr(new TextureGroup(
            TextureDefinition(target_name.target, target_name.mailbox, texture, 1, NULL, decoder)));
#else
                linked_ptr<TextureGroup> group = make_linked_ptr(new TextureGroup(
                    TextureDefinition(target_name.target, texture, 1, NULL)));
#endif

            // Unlink other textures from this mailbox in case the name is not new.
            ReassociateMailboxLocked(target_name, group.get());
            textures_.insert(std::make_pair(texture, TextureVersion(group)));
        }
    }
#if 0 // def TENCENT_CHANGES
  if (decoder && copy_textures_manager_ && use_sharegroup_) {
    TextureManager* texture_manager = decoder->GetContextGroup()->texture_manager();
    copy_textures_manager_->DeleteUnusedTexture(texture_manager);
  }
  //为了规避gl_fence_egl.cc中对context有无flush的检查不通过。
  glFlush();
#endif
}

void MailboxSynchronizer::UpdateTextureLocked(Texture* texture,
    TextureVersion& texture_version)
{
    lock_.AssertAcquired();
    gfx::GLImage* gl_image = texture->GetLevelImage(texture->target(), 0);
    TextureGroup* group = texture_version.group.get();
    scoped_refptr<NativeImageBuffer> image_buffer = group->definition.image();

    // Make sure we don't clobber with an older version
    if (!group->definition.IsOlderThan(texture_version.version))
        return;

    // Also don't push redundant updates. Note that it would break the
    // versioning.
    if (group->definition.Matches(texture))
        return;

    if (gl_image && !image_buffer->IsClient(gl_image)) {
        LOG(ERROR) << "MailboxSync: Incompatible attachment";
        return;
    }
#if 0 // def TENCENT_CHANGES
  if (gl_image && image_buffer) {
    image_buffer->DidLastWrite(gl_image);
  }

  group->definition = TextureDefinition(texture->target(),
		  	  	  	  	  	  	  	  	group->mailboxes.begin()->mailbox,
                                        texture,
                                        ++texture_version.version,
                                        gl_image ? image_buffer : NULL,
                                        NULL);
#else
        group->definition = TextureDefinition(texture->target(),
            texture,
            ++texture_version.version,
            gl_image ? image_buffer : NULL);
#endif
}

void MailboxSynchronizer::PullTextureUpdates(MailboxManager* manager)
{
    base::AutoLock lock(lock_);
    for (MailboxManager::MailboxToTextureMap::const_iterator texture_it = manager->mailbox_to_textures_.begin();
         texture_it != manager->mailbox_to_textures_.end();
         texture_it++) {
        Texture* texture = texture_it->second->first;
        TextureMap::iterator it = textures_.find(texture);
        if (it != textures_.end()) {
            TextureDefinition& definition = it->second.group->definition;
            if (it->second.version == definition.version() || definition.IsOlderThan(it->second.version))
                continue;
            it->second.version = definition.version();
            definition.UpdateTexture(texture);
        }
    }
}

#if 0 // def TENCENT_CHANGES
bool MailboxSynchronizer::SetUseShareGroup(bool use_sharegroup) {
	use_sharegroup_ = use_sharegroup;
	return true;
}
#endif

} // namespace gles2
} // namespace gpu
