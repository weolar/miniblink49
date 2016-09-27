// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/mailbox_manager.h"

#include <algorithm>

#include "crypto/random.h"
#include "gpu/command_buffer/service/mailbox_synchronizer.h"
#include "gpu/command_buffer/service/texture_manager.h"

namespace gpu {
namespace gles2 {

MailboxManager::MailboxManager()
    : mailbox_to_textures_(std::ptr_fun(&MailboxManager::TargetNameLess)),
      sync_(MailboxSynchronizer::GetInstance()) {
}

MailboxManager::~MailboxManager() {
  DCHECK(mailbox_to_textures_.empty());
  DCHECK(textures_to_mailboxes_.empty());
}

Texture* MailboxManager::ConsumeTexture(unsigned target,
                                        const Mailbox& mailbox) {
  TargetName target_name(target, mailbox);
  MailboxToTextureMap::iterator it =
      mailbox_to_textures_.find(target_name);
  if (it != mailbox_to_textures_.end())
    return it->second->first;

  if (sync_) {
    // See if it's visible in another mailbox manager, and if so make it visible
    // here too.
    Texture* texture = sync_->CreateTextureFromMailbox(target, mailbox);
    if (texture) {
      InsertTexture(target_name, texture);
      DCHECK_EQ(0U, texture->refs_.size());
    }
    return texture;
  }

  return NULL;
}

#ifdef TENCENT_CHANGES
void MailboxManager::ProduceTexture(unsigned target,
                                    const Mailbox& mailbox,
                                    Texture* texture,
                                    gles2::GLES2Decoder* decoder) {
  if (sync_ && decoder)
    sync_->UpdateTexture(target, mailbox, texture, decoder);
#else
void MailboxManager::ProduceTexture(unsigned target,
                                    const Mailbox& mailbox,
                                    Texture* texture) {
#endif
  TargetName target_name(target, mailbox);
  MailboxToTextureMap::iterator it = mailbox_to_textures_.find(target_name);
  if (it != mailbox_to_textures_.end()) {
    if (it->second->first == texture)
      return;
    TextureToMailboxMap::iterator texture_it = it->second;
    mailbox_to_textures_.erase(it);
    textures_to_mailboxes_.erase(texture_it);
  }
  InsertTexture(target_name, texture);
}

void MailboxManager::InsertTexture(TargetName target_name, Texture* texture) {
  texture->SetMailboxManager(this);
  TextureToMailboxMap::iterator texture_it =
      textures_to_mailboxes_.insert(std::make_pair(texture, target_name));
  mailbox_to_textures_.insert(std::make_pair(target_name, texture_it));
  DCHECK_EQ(mailbox_to_textures_.size(), textures_to_mailboxes_.size());
}

void MailboxManager::TextureDeleted(Texture* texture) {
  std::pair<TextureToMailboxMap::iterator,
            TextureToMailboxMap::iterator> range =
      textures_to_mailboxes_.equal_range(texture);
  for (TextureToMailboxMap::iterator it = range.first;
       it != range.second; ++it) {
    size_t count = mailbox_to_textures_.erase(it->second);
#ifdef TENCENT_CHANGES
    if (sync_ && sync_->IsUseShareGroup() && sync_->GetCopyTexturesManager())
    	sync_->GetCopyTexturesManager()->MailboxDeleted(it->second.target, it->second.mailbox);
#endif
    DCHECK(count == 1);
  }
  textures_to_mailboxes_.erase(range.first, range.second);
  DCHECK_EQ(mailbox_to_textures_.size(), textures_to_mailboxes_.size());

  if (sync_)
    sync_->TextureDeleted(texture);
}

#ifdef TENCENT_CHANGES
void MailboxManager::PushTextureUpdates(gles2::GLES2Decoder* decoder) {
  if (sync_)
    sync_->PushTextureUpdates(this, decoder);
}
#else
void MailboxManager::PushTextureUpdates() {
  if (sync_)
    sync_->PushTextureUpdates(this);
}
#endif

void MailboxManager::PullTextureUpdates() {
  if (sync_)
    sync_->PullTextureUpdates(this);
}

MailboxManager::TargetName::TargetName(unsigned target, const Mailbox& mailbox)
    : target(target),
      mailbox(mailbox) {
}

bool MailboxManager::TargetNameLess(const MailboxManager::TargetName& lhs,
                                    const MailboxManager::TargetName& rhs) {
  return memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

}  // namespace gles2
}  // namespace gpu
