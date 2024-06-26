// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationTimeline_h
#define mc_animation_AnimationTimeline_h

#include <vector>

// #include "base/containers/hash_tables.h"
// #include "base/memory/ref_counted.h"
// #include "base/memory/scoped_ptr.h"
// #include "cc/base/cc_export.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassRefPtr.h"
#include "third_party/WebKit/Source/wtf/RefCounted.h"

namespace mc {

class AnimationHost;
class AnimationPlayer;

typedef std::vector<AnimationPlayer*> AnimationPlayerList; // scoped_refptr

// An AnimationTimeline owns a group of AnimationPlayers.
// This is a cc counterpart for blink::AnimationTimeline (in 1:1 relationship).
// Each AnimationTimeline and its AnimationPlayers have their copies on
// the impl thread. We synchronize main thread and impl thread instances
// using integer IDs.
class AnimationTimeline : public WTF::RefCounted<AnimationTimeline> {
public:
    static WTF::PassRefPtr<AnimationTimeline> create(int id);
    WTF::PassRefPtr<AnimationTimeline> createImplInstance() const;

    int id() const {
        return m_id;
    }

    // Parent AnimationHost.
    AnimationHost* getAnimationHost() {
        return m_animationHost;
    }
    const AnimationHost* getAnimationHost() const {
        return m_animationHost;
    }
    void setAnimationHost(AnimationHost* animation_host);

    void setIsImplOnly(bool is_impl_only) {
        m_isImplOnly = is_impl_only;
    }
    bool getIsImplOnly() const {
        return m_isImplOnly;
    }

    void attachPlayer(WTF::PassRefPtr<AnimationPlayer> player);
    void detachPlayer(WTF::PassRefPtr<AnimationPlayer> player);

    void clearPlayers();

    void pushPropertiesTo(AnimationTimeline* timelineImpl);

    AnimationPlayer* getPlayerById(int player_id) const;

private:
    friend class WTF::RefCounted<AnimationTimeline>;

    explicit AnimationTimeline(int id);
    virtual ~AnimationTimeline();

    void pushAttachedPlayersToImplThread(AnimationTimeline* timeline) const;
    void removeDetachedPlayersFromImplThread(AnimationTimeline* timeline) const;
    void pushPropertiesToImplThread(AnimationTimeline* timeline);

    void erasePlayers(AnimationPlayerList::iterator begin, AnimationPlayerList::iterator end);

    AnimationPlayerList m_players;
    int m_id;
    AnimationHost* m_animationHost;

    // Impl-only AnimationTimeline has no main thread instance and lives on it's own.
    bool m_isImplOnly;
};

}  // namespace mc

#endif  // mc_animation_AnimationTimeline_h
