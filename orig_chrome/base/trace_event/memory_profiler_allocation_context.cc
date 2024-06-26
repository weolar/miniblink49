// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_profiler_allocation_context.h"

#include <algorithm>
#include <cstring>

#include "base/hash.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_local_storage.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/trace_event/trace_event_memory_overhead.h"

namespace base {
namespace trace_event {

    subtle::Atomic32 AllocationContextTracker::capture_enabled_ = 0;

    namespace {

        ThreadLocalStorage::StaticSlot g_tls_alloc_ctx_tracker = TLS_INITIALIZER;

        // This function is added to the TLS slot to clean up the instance when the
        // thread exits.
        void DestructAllocationContextTracker(void* alloc_ctx_tracker)
        {
            delete static_cast<AllocationContextTracker*>(alloc_ctx_tracker);
        }

    } // namespace

    AllocationStack::AllocationStack() { }
    AllocationStack::~AllocationStack() { }

    bool operator==(const Backtrace& lhs, const Backtrace& rhs)
    {
        // Pointer equality of the stack frames is assumed, so instead of doing a deep
        // string comparison on all of the frames, a |memcmp| suffices.
        return std::memcmp(lhs.frames, rhs.frames, sizeof(lhs.frames)) == 0;
    }

    StackFrameDeduplicator::FrameNode::FrameNode(StackFrame frame,
        int parent_frame_index)
        : frame(frame)
        , parent_frame_index(parent_frame_index)
    {
    }
    StackFrameDeduplicator::FrameNode::~FrameNode() { }

    StackFrameDeduplicator::StackFrameDeduplicator() { }
    StackFrameDeduplicator::~StackFrameDeduplicator() { }

    int StackFrameDeduplicator::Insert(const Backtrace& bt)
    {
        int frame_index = -1;
        std::map<StackFrame, int>* nodes = &roots_;

        for (size_t i = 0; i < arraysize(bt.frames); i++) {
            if (!bt.frames[i])
                break;

            auto node = nodes->find(bt.frames[i]);
            if (node == nodes->end()) {
                // There is no tree node for this frame yet, create it. The parent node
                // is the node associated with the previous frame.
                FrameNode frame_node(bt.frames[i], frame_index);

                // The new frame node will be appended, so its index is the current size
                // of the vector.
                frame_index = static_cast<int>(frames_.size());

                // Add the node to the trie so it will be found next time.
                nodes->insert(std::make_pair(bt.frames[i], frame_index));

                // Append the node after modifying |children|, because the vector might
                // need to resize, and this invalidates the |children| pointer.
                frames_.push_back(frame_node);
            } else {
                // A tree node for this frame exists. Look for the next one.
                frame_index = node->second;
            }

            nodes = &frames_[frame_index].children;
        }

        return frame_index;
    }

    void StackFrameDeduplicator::AppendAsTraceFormat(std::string* out) const
    {
        out->append("{"); // Begin the |stackFrames| dictionary.

        int i = 0;
        auto frame_node = begin();
        auto it_end = end();
        std::string stringify_buffer;
        while (frame_node != it_end) {
            // The |stackFrames| format is a dictionary, not an array, so the
            // keys are stringified indices. Write the index manually, then use
            // |TracedValue| to format the object. This is to avoid building the
            // entire dictionary as a |TracedValue| in memory.
            SStringPrintf(&stringify_buffer, "\"%d\":", i);
            out->append(stringify_buffer);

            scoped_refptr<TracedValue> frame_node_value = new TracedValue;
            frame_node_value->SetString("name", frame_node->frame);
            if (frame_node->parent_frame_index >= 0) {
                SStringPrintf(&stringify_buffer, "%d", frame_node->parent_frame_index);
                frame_node_value->SetString("parent", stringify_buffer);
            }
            frame_node_value->AppendAsTraceFormat(out);

            i++;
            frame_node++;

            if (frame_node != it_end)
                out->append(",");
        }

        out->append("}"); // End the |stackFrames| dictionary.
    }

    void StackFrameDeduplicator::EstimateTraceMemoryOverhead(
        TraceEventMemoryOverhead* overhead)
    {

        // The sizes here are only estimates; they fail to take into account the
        // overhead of the tree nodes for the map, but as an estimate this should be
        // fine.
        size_t maps_size = roots_.size() * sizeof(std::pair<StackFrame, int>);
        size_t frames_allocated = frames_.capacity() * sizeof(FrameNode);
        size_t frames_resident = frames_.size() * sizeof(FrameNode);

        for (const FrameNode& node : frames_)
            maps_size += node.children.size() * sizeof(std::pair<StackFrame, int>);

        overhead->Add("StackFrameDeduplicator",
            sizeof(StackFrameDeduplicator) + maps_size + frames_allocated,
            sizeof(StackFrameDeduplicator) + maps_size + frames_resident);
    }

    bool operator==(const AllocationContext& lhs, const AllocationContext& rhs)
    {
        return (lhs.backtrace == rhs.backtrace) && (lhs.type_id == rhs.type_id);
    }

    AllocationContextTracker* AllocationContextTracker::GetThreadLocalTracker()
    {
        auto tracker = static_cast<AllocationContextTracker*>(g_tls_alloc_ctx_tracker.Get());

        if (!tracker) {
            tracker = new AllocationContextTracker();
            g_tls_alloc_ctx_tracker.Set(tracker);
        }

        return tracker;
    }

    AllocationContextTracker::AllocationContextTracker() { }
    AllocationContextTracker::~AllocationContextTracker() { }

    // static
    void AllocationContextTracker::SetCaptureEnabled(bool enabled)
    {
        // When enabling capturing, also initialize the TLS slot. This does not create
        // a TLS instance yet.
        if (enabled && !g_tls_alloc_ctx_tracker.initialized())
            g_tls_alloc_ctx_tracker.Initialize(DestructAllocationContextTracker);

        // Release ordering ensures that when a thread observes |capture_enabled_| to
        // be true through an acquire load, the TLS slot has been initialized.
        subtle::Release_Store(&capture_enabled_, enabled);
    }

    // static
    void AllocationContextTracker::PushPseudoStackFrame(StackFrame frame)
    {
        auto tracker = AllocationContextTracker::GetThreadLocalTracker();
        tracker->pseudo_stack_.push(frame);
    }

    // static
    void AllocationContextTracker::PopPseudoStackFrame(StackFrame frame)
    {
        auto tracker = AllocationContextTracker::GetThreadLocalTracker();
        tracker->pseudo_stack_.pop(frame);
    }

    // Returns a pointer past the end of the fixed-size array |array| of |T| of
    // length |N|, identical to C++11 |std::end|.
    template <typename T, int N>
    T* End(T (&array)[N])
    {
        return array + N;
    }

    // static
    AllocationContext AllocationContextTracker::GetContextSnapshot()
    {
        AllocationContextTracker* tracker = GetThreadLocalTracker();
        AllocationContext ctx;

        // Fill the backtrace.
        {
            auto src = tracker->pseudo_stack_.bottom();
            auto dst = ctx.backtrace.frames;
            auto src_end = tracker->pseudo_stack_.top();
            auto dst_end = End(ctx.backtrace.frames);

            // Copy as much of the bottom of the pseudo stack into the backtrace as
            // possible.
            for (; src != src_end && dst != dst_end; src++, dst++)
                *dst = *src;

            // If there is room for more, fill the remaining slots with empty frames.
            std::fill(dst, dst_end, nullptr);
        }

        ctx.type_id = 0;

        return ctx;
    }

} // namespace trace_event
} // namespace base

namespace BASE_HASH_NAMESPACE {
using base::trace_event::AllocationContext;
using base::trace_event::Backtrace;

size_t hash<Backtrace>::operator()(const Backtrace& backtrace) const
{
    return base::SuperFastHash(reinterpret_cast<const char*>(backtrace.frames),
        sizeof(backtrace.frames));
}

size_t hash<AllocationContext>::operator()(const AllocationContext& ctx) const
{
    size_t ctx_hash = hash<Backtrace>()(ctx.backtrace);

    // Multiply one side to break the commutativity of +. Multiplication with a
    // number coprime to |numeric_limits<size_t>::max() + 1| is bijective so
    // randomness is preserved. The type ID is assumed to be distributed randomly
    // already so there is no need to hash it.
    return (ctx_hash * 3) + static_cast<size_t>(ctx.type_id);
}

} // BASE_HASH_NAMESPACE
