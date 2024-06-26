// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_SYNCED_PROPERTY_H_
#define CC_BASE_SYNCED_PROPERTY_H_

#include "base/memory/ref_counted.h"

namespace cc {

// This class is the basic primitive used for impl-thread scrolling.  Its job is
// to sanely resolve the case where both the main and impl thread are
// concurrently updating the same value (for example, when Javascript sets the
// scroll offset during an ongoing impl-side scroll).
//
// There are three trees (main, pending, and active) and therefore also three
// places with their own idea of the scroll offsets (and analogous properties
// like page scale).  Objects of this class are meant to be held on the Impl
// side, and contain the canonical reference for the pending and active trees,
// as well as keeping track of the latest delta sent to the main thread (which
// is necessary for conflict resolution).

template <typename T>
class SyncedProperty : public base::RefCounted<SyncedProperty<T>> {
public:
    SyncedProperty()
        : clobber_active_value_(false)
    {
    }

    // Returns the canonical value for the specified tree, including the sum of
    // all deltas.  The pending tree should use this for activation purposes and
    // the active tree should use this for drawing.
    typename T::ValueType Current(bool is_active_tree) const
    {
        if (is_active_tree)
            return active_base_.Combine(active_delta_).get();
        else
            return pending_base_.Combine(PendingDelta()).get();
    }

    // Sets the value on the impl thread, due to an impl-thread-originating
    // action.  Returns true if this had any effect.  This will remain
    // impl-thread-only information at first, and will get pulled back to the main
    // thread on the next call of PullDeltaToMainThread (which happens right
    // before the commit).
    bool SetCurrent(typename T::ValueType current)
    {
        T delta = T(current).InverseCombine(active_base_);
        if (active_delta_.get() == delta.get())
            return false;

        active_delta_ = delta;
        return true;
    }

    // Returns the difference between the last value that was committed and
    // activated from the main thread, and the current total value.
    typename T::ValueType Delta() const { return active_delta_.get(); }

    // Returns the latest active tree delta and also makes a note that this value
    // was sent to the main thread.
    typename T::ValueType PullDeltaForMainThread()
    {
        sent_delta_ = active_delta_;
        return active_delta_.get();
    }

    // Push the latest value from the main thread onto pending tree-associated
    // state.  Returns true if this had any effect.
    bool PushFromMainThread(typename T::ValueType main_thread_value)
    {
        if (pending_base_.get() == main_thread_value)
            return false;

        pending_base_ = T(main_thread_value);

        return true;
    }

    // Push the value associated with the pending tree to be the active base
    // value.  As part of this, subtract the last sent value from the active tree
    // delta (which will make the delta zero at steady state, or make it contain
    // only the difference since the last send).
    bool PushPendingToActive()
    {
        if (active_base_.get() == pending_base_.get() && sent_delta_.get() == T::Identity().get())
            return false;

        active_base_ = pending_base_;
        active_delta_ = PendingDelta();
        sent_delta_ = T::Identity();
        clobber_active_value_ = false;

        return true;
    }

    // This simulates the consequences of the sent value getting committed and
    // activated.  The value sent to the main thread ends up combined with the
    // active value, and the sent_delta is subtracted from the delta.
    void AbortCommit()
    {
        active_base_ = active_base_.Combine(sent_delta_);
        active_delta_ = PendingDelta();
        sent_delta_ = T::Identity();
    }

    // Values as last pushed to the pending or active tree respectively, with no
    // impl-thread delta applied.
    typename T::ValueType PendingBase() const { return pending_base_.get(); }
    typename T::ValueType ActiveBase() const { return active_base_.get(); }

    // The new delta we would use if we decide to activate now.  This delta
    // excludes the amount that we expect the main thread to reflect back at the
    // impl thread during the commit.
    T PendingDelta() const
    {
        if (clobber_active_value_)
            return T::Identity();
        return active_delta_.InverseCombine(sent_delta_);
    }

    void set_clobber_active_value() { clobber_active_value_ = true; }

private:
    // Value last committed to the pending tree.
    T pending_base_;
    // Value last committed to the active tree (on the last activation).
    T active_base_;
    // The difference between the active_base_ and the user-perceived value.
    T active_delta_;
    // The value sent to the main thread (on the last BeginFrame); this is always
    // identity outside of the BeginFrame-to-activation interval.
    T sent_delta_;
    // When true the pending delta is always identity so that it does not change
    // and will clobber the active value on push.
    bool clobber_active_value_;

    friend class base::RefCounted<SyncedProperty<T>>;
    ~SyncedProperty() { }
};

// SyncedProperty's delta-based conflict resolution logic makes sense for any
// mathematical group.  In practice, there are two that are useful:
// 1. Numbers/vectors with addition and identity = 0 (like scroll offsets)
// 2. Real numbers with multiplication and identity = 1 (like page scale)

template <class V>
class AdditionGroup {
public:
    typedef V ValueType;

    AdditionGroup()
        : value_(Identity().get())
    {
    }
    explicit AdditionGroup(V value)
        : value_(value)
    {
    }

    V& get() { return value_; }
    const V& get() const { return value_; }

    static AdditionGroup<V> Identity() { return AdditionGroup(V()); } // zero
    AdditionGroup<V> Combine(AdditionGroup<V> p) const
    {
        return AdditionGroup<V>(value_ + p.value_);
    }
    AdditionGroup<V> InverseCombine(AdditionGroup<V> p) const
    {
        return AdditionGroup<V>(value_ - p.value_);
    }

private:
    V value_;
};

class ScaleGroup {
public:
    typedef float ValueType;

    ScaleGroup()
        : value_(Identity().get())
    {
    }
    explicit ScaleGroup(float value)
        : value_(value)
    {
    }

    float& get() { return value_; }
    const float& get() const { return value_; }

    static ScaleGroup Identity() { return ScaleGroup(1.f); }
    ScaleGroup Combine(ScaleGroup p) const
    {
        return ScaleGroup(value_ * p.value_);
    }
    ScaleGroup InverseCombine(ScaleGroup p) const
    {
        return ScaleGroup(value_ / p.value_);
    }

private:
    float value_;
};

} // namespace cc

#endif // CC_BASE_SYNCED_PROPERTY_H_
