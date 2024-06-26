// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_MULTIBUFFER_H_
#define MEDIA_BLINK_MULTIBUFFER_H_

#include <stdint.h>

#include <limits>
#include <map>
#include <set>
#include <vector>

#include "base/callback.h"
#include "base/containers/hash_tables.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "media/base/data_buffer.h"
#include "media/blink/interval_map.h"
#include "media/blink/lru.h"
#include "media/blink/media_blink_export.h"

namespace media {

typedef int32_t MultiBufferBlockId;
class MultiBuffer;
typedef std::pair<MultiBuffer*, MultiBufferBlockId> MultiBufferGlobalBlockId;

} // namespace media

namespace BASE_HASH_NAMESPACE {

#if USING_VC6RT != 1

template <>
struct hash<media::MultiBufferGlobalBlockId> {
    std::size_t operator()(const media::MultiBufferGlobalBlockId& key) const
    {
// It would be nice if we could use intptr_t instead of int64_t here, but
// on some platforms, int64_t is declared as "long" which doesn't match
// any of the HashPair() functions. This leads to a compile error since
// the compiler can't decide which HashPair() function to call.
#if defined(ARCH_CPU_64_BITS)
        return base::HashPair(reinterpret_cast<int64_t>(key.first), key.second);
#else
        return base::HashPair(reinterpret_cast<int32_t>(key.first), key.second);
#endif
    }
};

#endif // USING_VC6RT

} // namespace BASE_HASH_NAMESPACE

namespace media {

// Freeing a lot of blocks can be expensive, to keep thing
// flowing smoothly we only free a maximum of |kMaxFreesPerAdd|
// blocks when a new block is added to the cache.
const int kMaxFreesPerAdd = 10;

// There is a simple logic for creating, destroying and deferring
// data providers. Every data provider has a look-ahead region and
// a look-behind region. If there are readers in the look-ahead
// region, we keep reading. If not, but there are readers in the
// look-behind region, we defer. If there are no readers in either
// region, we destroy the data provider.

// When new readers are added, new data providers are created if
// the new reader doesn't fall into the look-ahead region of
// an existing data provider.

// This is the size of the look-ahead region.
const int kMaxWaitForWriterOffset = 5;

// This is the size of the look-behind region.
const int kMaxWaitForReaderOffset = 50;

class MultiBuffer;

// MultiBuffers are multi-reader multi-writer cache/buffers with
// prefetching and pinning. Data is stored internally in ref-counted
// blocks of identical size. |block_size_shift| is log2 of the block
// size.
//
// Users should inherit this class and implement CreateWriter().
// TODO(hubbe): Make the multibuffer respond to memory pressure.
class MEDIA_BLINK_EXPORT MultiBuffer {
public:
    // Interface for clients wishing to read data out of this cache.
    // Note: It might look tempting to replace this with a callback,
    // but we keep and compare pointers to Readers internally.
    class Reader {
    public:
        Reader() { }
        virtual ~Reader() { }
        // Notifies the reader that the range of available blocks has changed.
        // The reader must call MultiBuffer::Observe() to activate this callback.
        virtual void NotifyAvailableRange(
            const Interval<MultiBufferBlockId>& range)
            = 0;

    private:
        DISALLOW_COPY_AND_ASSIGN(Reader);
    };

    // DataProvider is the interface that MultiBuffer
    // uses to get data into the cache.
    class DataProvider {
    public:
        virtual ~DataProvider() { }

        // Returns the block number that is to be returned
        // by the next Read() call.
        virtual MultiBufferBlockId Tell() const = 0;

        // Returns true if one (or more) blocks are
        // availble to read.
        virtual bool Available() const = 0;

        // Returns the next block. Only valid if Available()
        // returns true. Last block might be of a smaller size
        // and after the last block we will get an end-of-stream
        // DataBuffer.
        virtual scoped_refptr<DataBuffer> Read() = 0;

        // |cb| is called every time Available() becomes true.
        virtual void SetAvailableCallback(const base::Closure& cb) = 0;

        // Ask the data provider to stop giving us data.
        // It's ok if the effect is not immediate.
        virtual void SetDeferred(bool deferred) = 0;
    };

    // Multibuffers use a global shared LRU to free memory.
    // This effectively means that recently used multibuffers can
    // borrow memory from less recently used ones.
    class MEDIA_BLINK_EXPORT GlobalLRU : public base::RefCounted<GlobalLRU> {
    public:
        typedef MultiBufferGlobalBlockId GlobalBlockId;
        GlobalLRU();

        // Free elements from cache if needed and possible.
        // Don't free more than |max_to_free| blocks.
        // Virtual for testing purposes.
        void Prune(int64_t max_to_free);

        void IncrementDataSize(int64_t blocks);
        void IncrementMaxSize(int64_t blocks);

        // LRU operations.
        void Use(MultiBuffer* multibuffer, MultiBufferBlockId id);
        void Remove(MultiBuffer* multibuffer, MultiBufferBlockId id);
        void Insert(MultiBuffer* multibuffer, MultiBufferBlockId id);
        bool Contains(MultiBuffer* multibuffer, MultiBufferBlockId id);
        int64_t Size() const;

    private:
        friend class base::RefCounted<GlobalLRU>;
        ~GlobalLRU();

        // Max number of blocks.
        int64_t max_size_;

        // Sum of all multibuffer::data_.size().
        int64_t data_size_;

        // The LRU should contain all blocks which are not pinned from
        // all multibuffers.
        LRU<GlobalBlockId> lru_;
    };

    MultiBuffer(int32_t block_size_shift,
        const scoped_refptr<GlobalLRU>& global_lru);
    virtual ~MultiBuffer();

    // Identifies a block in the cache.
    // Block numbers can be calculated from byte positions as:
    // block_num = byte_pos >> block_size_shift
    typedef MultiBufferBlockId BlockId;
    typedef base::hash_map<BlockId, scoped_refptr<DataBuffer>> DataMap;

    // Registers a reader at the given position.
    // If the cache does not already contain |pos|, it will activate
    // or create data providers to make sure that the block becomes
    // available soon. If |pos| is already in the cache, no action is
    // taken, it simply lets the cache know that this reader is likely
    // to read pos+1, pos+2.. soon.
    //
    // Registered readers will be notified when the available range
    // at their position changes. The available range at |pos| is a range
    // from A to B where: A <= |pos|, B >= |pos| and all blocks in [A..B)
    // are present in the cache.  When this changes, we will call
    // NotifyAvailableRange() on the reader.
    void AddReader(const BlockId& pos, Reader* reader);

    // Unregister a reader at block |pos|.
    // Often followed by a call to AddReader(pos + 1, ...);
    // Idempotent.
    void RemoveReader(const BlockId& pos, Reader* reader);

    // Immediately remove writers at or before |pos| if nobody needs them.
    // Note that we can't really do this in StopWaitFor(), because it's very
    // likely that StopWaitFor() is immediately followed by a call to WaitFor().
    // It is also a bad idea to wait for the writers to clean themselves up when
    // they try to provide unwanted data to the cache. Besides the obvoius
    // inefficiency, it will also cause the http_cache to bypass the disk/memory
    // cache if we have multiple simultaneous requests going against the same
    // url.
    void CleanupWriters(const BlockId& pos);

    // Returns true if block |pos| is available in the cache.
    bool Contains(const BlockId& pos) const;

    // Returns the next unavailable block at or after |pos|.
    BlockId FindNextUnavailable(const BlockId& pos) const;

    // Change the pin count for a range of data blocks.
    // Note that blocks do not have to be present in the
    // cache to be pinned.
    // Examples:
    // Pin block 3, 4 & 5: PinRange(3, 6, 1);
    // Unpin block 4 & 5: PinRange(4, 6, -1);
    void PinRange(const BlockId& from, const BlockId& to, int32_t how_much);

    // Calls PinRange for each range in |ranges|, convenience
    // function for applying multiple changes to the pinned ranges.
    void PinRanges(const IntervalMap<BlockId, int32_t>& ranges);

    // Increment max cache size by |size| (counted in blocks).
    void IncrementMaxSize(int32_t size);

    // Caller takes ownership of 'provider', cache will
    // not call it anymore.
    scoped_ptr<DataProvider> RemoveProvider(DataProvider* provider);

    // Add a writer to this cache. Cache takes ownership and
    // may choose to destroy it.
    void AddProvider(scoped_ptr<DataProvider> provider);

    // Transfer all data from |other| to this.
    void MergeFrom(MultiBuffer* other);

    // Accessors.
    const DataMap& map() const { return data_; }
    int32_t block_size_shift() const { return block_size_shift_; }

protected:
    // Create a new writer at |pos| and return it.
    // Users needs to implemement this method.
    virtual DataProvider* CreateWriter(const BlockId& pos) = 0;

    virtual bool RangeSupported() const = 0;

private:
    // For testing.
    friend class TestMultiBuffer;

    enum ProviderState {
        ProviderStateDead,
        ProviderStateDefer,
        ProviderStateLoad
    };

    // Can be overriden for testing.
    virtual void Prune(size_t max_to_free);

    // Remove the given blocks from the multibuffer, called from
    // GlobalLRU::Prune().
    void ReleaseBlocks(const std::vector<MultiBufferBlockId>& blocks);

    // Figure out what state a writer at |pos| should be in.
    ProviderState SuggestProviderState(const BlockId& pos) const;

    // Returns true if a writer at |pos| is colliding with
    // output of another writer.
    bool ProviderCollision(const BlockId& pos) const;

    // Call NotifyAvailableRange(new_range) on all readers waiting
    // for a block in |observer_range|
    void NotifyAvailableRange(const Interval<MultiBufferBlockId>& observer_range,
        const Interval<MultiBufferBlockId>& new_range);

    // Callback which notifies us that a data provider has
    // some data for us. Also called when it might be apprperiate
    // for a provider in a deferred state to wake up.
    void DataProviderEvent(DataProvider* provider);

    // Max number of blocks.
    int64_t max_size_;

    // log2 of block size.
    int32_t block_size_shift_;

    // Stores the actual data.
    DataMap data_;

    // Keeps track of readers waiting for data.
    std::map<MultiBufferBlockId, std::set<Reader*>> readers_;

    // Keeps track of writers by their position.
    // The writers are owned by this class.
    // TODO(hubbe): Use ScopedPtrMap here. (must add upper/lower_bound first)
    std::map<BlockId, DataProvider*> writer_index_;

    // Gloabally shared LRU, decides which block to free next.
    scoped_refptr<GlobalLRU> lru_;

    // Keeps track of what blocks are pinned. If block p is pinned,
    // then pinned_[p] > 0. Pinned blocks cannot be freed and should not
    // be present in |lru_|.
    IntervalMap<BlockId, int32_t> pinned_;

    // present_[block] should be 1 for all blocks that are present
    // and 0 for all blocks that are not. Used to quickly figure out
    // ranges of available/unavailable blocks without iterating.
    IntervalMap<BlockId, int32_t> present_;
};

} // namespace media

#endif // MEDIA_BLINK_MULTIBUFFER_H_
