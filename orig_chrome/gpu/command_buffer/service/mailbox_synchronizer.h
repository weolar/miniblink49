// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_MAILBOX_SYNCHRONIZER_H_
#define GPU_COMMAND_BUFFER_SERVICE_MAILBOX_SYNCHRONIZER_H_

#include "gpu/command_buffer/common/mailbox.h"

#include <map>
#include <set>

#if 0 // def TENCENT_CHANGES
#include "base/memory/scoped_ptr.h"
#endif
#include "base/memory/linked_ptr.h"
#include "base/synchronization/lock.h"
#include "gpu/command_buffer/service/texture_definition.h"
#include "gpu/gpu_export.h"

namespace gpu {

#if 0 // def TENCENT_CHANGES
class CopyTextureCHROMIUMResourceManager;
#endif

namespace gles2 {

    class MailboxManager;
    class Texture;
#if 0 // def TENCENT_CHANGES
class GLES2Decoder;
class TextureManager;
class TextureRef;


class CopyTexturesManager{
public:
  CopyTexturesManager(){}
  virtual ~CopyTexturesManager(){}

  virtual int DeleteUnusedTexture(TextureManager* manager) = 0;
  virtual void AddTexture(unsigned target, const Mailbox& mailbox, scoped_refptr<TextureRef> texture) = 0;
  virtual scoped_refptr<TextureRef> FindTexture(unsigned target, const Mailbox& mailbox) = 0;
  virtual void MailboxDeleted(unsigned target, const Mailbox& mailbox) = 0;
};
#endif

    // A thread-safe proxy that can be used to emulate texture sharing across
    // share-groups.
    class MailboxSynchronizer {
    public:
        ~MailboxSynchronizer();

        GPU_EXPORT static bool Initialize();
        GPU_EXPORT static void Terminate();
        static MailboxSynchronizer* GetInstance();

        // Create a texture from a globally visible mailbox.
        Texture* CreateTextureFromMailbox(unsigned target, const Mailbox& mailbox);

#if 0 // def TENCENT_CHANGES
  void PushTextureUpdates(MailboxManager* manager, gles2::GLES2Decoder* decoder);
#else
        void PushTextureUpdates(MailboxManager* manager);
#endif
        void PullTextureUpdates(MailboxManager* manager);

        void TextureDeleted(Texture* texture);

#if 0 // def TENCENT_CHANGES
  void UpdateTexture(unsigned target, const Mailbox& mailbox, Texture* texture, gles2::GLES2Decoder* decoder);
#endif

#if 0 // def TENCENT_CHANGES
  //返回值表示是否设置成功。只能在初始化时设置，中途修改可能会存在问题。
  bool SetUseShareGroup(bool use_sharegroup);

  bool IsUseShareGroup() {return use_sharegroup_;}

  CopyTexturesManager* GetCopyTexturesManager() {return copy_textures_manager_.get();}
#endif

    private:
        MailboxSynchronizer();

        struct TargetName {
            TargetName(unsigned target, const Mailbox& mailbox);
            bool operator<(const TargetName& rhs) const
            {
                return memcmp(this, &rhs, sizeof(rhs)) < 0;
            }
            bool operator!=(const TargetName& rhs) const
            {
                return memcmp(this, &rhs, sizeof(rhs)) != 0;
            }
            bool operator==(const TargetName& rhs) const
            {
                return !operator!=(rhs);
            }
            unsigned target;
            Mailbox mailbox;
        };

        base::Lock lock_;

        struct TextureGroup {
            explicit TextureGroup(const TextureDefinition& definition);
            ~TextureGroup();

            TextureDefinition definition;

            std::set<TargetName> mailboxes;

        private:
            DISALLOW_COPY_AND_ASSIGN(TextureGroup);
        };

        struct TextureVersion {
            explicit TextureVersion(linked_ptr<TextureGroup> group);
            ~TextureVersion();

            unsigned int version;
            linked_ptr<TextureGroup> group;
        };
        typedef std::map<Texture*, TextureVersion> TextureMap;
        TextureMap textures_;

        linked_ptr<TextureGroup> GetGroupForMailboxLocked(
            const TargetName& target_name);
        void ReassociateMailboxLocked(
            const TargetName& target_name,
            TextureGroup* group);
        void UpdateTextureLocked(Texture* texture, TextureVersion& texture_version);

#if 0 // def TENCENT_CHANGES
  bool use_sharegroup_;
  scoped_ptr<CopyTexturesManager> copy_textures_manager_;
#endif

        DISALLOW_COPY_AND_ASSIGN(MailboxSynchronizer);
    };

} // namespage gles2
} // namespace gpu

#endif // GPU_COMMAND_BUFFER_SERVICE_MAILBOX_SYNCHRONIZER_H_
