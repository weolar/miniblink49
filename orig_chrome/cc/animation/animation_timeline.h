// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_ANIMATION_TIMELINE_H_
#define CC_ANIMATION_ANIMATION_TIMELINE_H_

#include <vector>

#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"

namespace cc {

class AnimationHost;
class AnimationPlayer;

typedef std::vector<scoped_refptr<AnimationPlayer>> AnimationPlayerList;

// An AnimationTimeline owns a group of AnimationPlayers.
// This is a cc counterpart for blink::AnimationTimeline (in 1:1 relationship).
// Each AnimationTimeline and its AnimationPlayers have their copies on
// the impl thread. We synchronize main thread and impl thread instances
// using integer IDs.
class CC_EXPORT AnimationTimeline : public base::RefCounted<AnimationTimeline> {
public:
    static scoped_refptr<AnimationTimeline> Create(int id);
    scoped_refptr<AnimationTimeline> CreateImplInstance() const;

    int id() const { return id_; }

    // Parent AnimationHost.
    AnimationHost* animation_host() { return animation_host_; }
    const AnimationHost* animation_host() const { return animation_host_; }
    void SetAnimationHost(AnimationHost* animation_host);

    void set_is_impl_only(bool is_impl_only) { is_impl_only_ = is_impl_only; }
    bool is_impl_only() const { return is_impl_only_; }

    void AttachPlayer(scoped_refptr<AnimationPlayer> player);
    void DetachPlayer(scoped_refptr<AnimationPlayer> player);

    void ClearPlayers();

    void PushPropertiesTo(AnimationTimeline* timeline_impl);

    AnimationPlayer* GetPlayerById(int player_id) const;

private:
    friend class base::RefCounted<AnimationTimeline>;

    explicit AnimationTimeline(int id);
    virtual ~AnimationTimeline();

    void PushAttachedPlayersToImplThread(AnimationTimeline* timeline) const;
    void RemoveDetachedPlayersFromImplThread(AnimationTimeline* timeline) const;
    void PushPropertiesToImplThread(AnimationTimeline* timeline);

    void ErasePlayers(AnimationPlayerList::iterator begin,
        AnimationPlayerList::iterator end);

    AnimationPlayerList players_;
    int id_;
    AnimationHost* animation_host_;

    // Impl-only AnimationTimeline has no main thread instance and lives on
    // it's own.
    bool is_impl_only_;

    DISALLOW_COPY_AND_ASSIGN(AnimationTimeline);
};

} // namespace cc

#endif // CC_ANIMATION_ANIMATION_TIMELINE_H_
