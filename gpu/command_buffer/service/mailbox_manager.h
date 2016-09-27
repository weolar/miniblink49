// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_MAILBOX_MANAGER_H_
#define GPU_COMMAND_BUFFER_SERVICE_MAILBOX_MANAGER_H_

#include <functional>
#include <map>

#include "base/memory/linked_ptr.h"
#include "base/memory/ref_counted.h"
#include "gpu/command_buffer/common/constants.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/gpu_export.h"

typedef signed char GLbyte;

namespace gpu {
namespace gles2 {

class MailboxSynchronizer;
class Texture;
class TextureManager;
#ifdef TENCENT_CHANGES
class GLES2Decoder;
#endif

// Manages resources scoped beyond the context or context group level.
class GPU_EXPORT MailboxManager : public base::RefCounted<MailboxManager> {
 public:
  MailboxManager();

  // Look up the texture definition from the named mailbox.
  Texture* ConsumeTexture(unsigned target, const Mailbox& mailbox);

  // Put the texture into the named mailbox.
#ifdef TENCENT_CHANGES
  void ProduceTexture(unsigned target,
                      const Mailbox& mailbox,
                      Texture* texture,
                      gles2::GLES2Decoder* decoder = NULL);
#else
  void ProduceTexture(unsigned target,
                      const Mailbox& mailbox,
                      Texture* texture);
#endif

  // Returns whether this manager synchronizes with other instances.
  bool UsesSync() { return sync_ != NULL; }

  // Used with the MailboxSynchronizer to push/pull texture state to/from
  // other manager instances.
#ifdef TENCENT_CHANGES
  void PushTextureUpdates(gles2::GLES2Decoder* decoder = NULL);
#else
  void PushTextureUpdates();
#endif
  void PullTextureUpdates();

  // Destroy any mailbox that reference the given texture.
  void TextureDeleted(Texture* texture);

 private:
  friend class base::RefCounted<MailboxManager>;
  friend class MailboxSynchronizer;

  ~MailboxManager();

  struct TargetName {
    TargetName(unsigned target, const Mailbox& mailbox);
    unsigned target;
    Mailbox mailbox;
  };
  void InsertTexture(TargetName target_name, Texture* texture);

  static bool TargetNameLess(const TargetName& lhs, const TargetName& rhs);

  // This is a bidirectional map between mailbox and textures. We can have
  // multiple mailboxes per texture, but one texture per mailbox. We keep an
  // iterator in the MailboxToTextureMap to be able to manage changes to
  // the TextureToMailboxMap efficiently.
  typedef std::multimap<Texture*, TargetName> TextureToMailboxMap;
  typedef std::map<TargetName,
                   TextureToMailboxMap::iterator,
                   std::pointer_to_binary_function<const TargetName&,
                                                   const TargetName&,
                                                   bool> > MailboxToTextureMap;

  MailboxToTextureMap mailbox_to_textures_;
  TextureToMailboxMap textures_to_mailboxes_;

  MailboxSynchronizer* sync_;

  DISALLOW_COPY_AND_ASSIGN(MailboxManager);
};
}  // namespage gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_MAILBOX_MANAGER_H_

