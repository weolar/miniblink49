// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP4_BOX_READER_H_
#define MEDIA_FORMATS_MP4_BOX_READER_H_

#include <map>
#include <vector>

#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "media/base/media_export.h"
#include "media/base/media_log.h"
#include "media/formats/mp4/fourccs.h"
#include "media/formats/mp4/rcheck.h"

namespace media {
namespace mp4 {

    class BoxReader;

    struct MEDIA_EXPORT Box {
        virtual ~Box();

        // Parse errors may be logged using the BoxReader's media log.
        virtual bool Parse(BoxReader* reader) = 0;

        virtual FourCC BoxType() const = 0;
    };

    class MEDIA_EXPORT BufferReader {
    public:
        BufferReader(const uint8* buf, const int size)
            : buf_(buf)
            , pos_(0)
        {
            CHECK(buf);
            size_ = base::checked_cast<uint64>(size);
        }

        bool HasBytes(uint64 count)
        {
            // As the size of a box is implementation limited to 2^31, fail if
            // attempting to check for too many bytes.
            return (pos_ <= size_ && count < static_cast<uint64>(kint32max) && size_ - pos_ >= count);
        }

        // Read a value from the stream, perfoming endian correction, and advance the
        // stream pointer.
        bool Read1(uint8* v) WARN_UNUSED_RESULT;
        bool Read2(uint16* v) WARN_UNUSED_RESULT;
        bool Read2s(int16* v) WARN_UNUSED_RESULT;
        bool Read4(uint32* v) WARN_UNUSED_RESULT;
        bool Read4s(int32* v) WARN_UNUSED_RESULT;
        bool Read8(uint64* v) WARN_UNUSED_RESULT;
        bool Read8s(int64* v) WARN_UNUSED_RESULT;

        bool ReadFourCC(FourCC* v) WARN_UNUSED_RESULT;

        bool ReadVec(std::vector<uint8>* t, uint64 count) WARN_UNUSED_RESULT;

        // These variants read a 4-byte integer of the corresponding signedness and
        // store it in the 8-byte return type.
        bool Read4Into8(uint64* v) WARN_UNUSED_RESULT;
        bool Read4sInto8s(int64* v) WARN_UNUSED_RESULT;

        // Advance the stream by this many bytes.
        bool SkipBytes(uint64 nbytes) WARN_UNUSED_RESULT;

        const uint8* data() const { return buf_; }

        // This returns the size of the box as specified in the box header. Initially
        // it is the buffer size until the header is read. Note that the size
        // specified in the box header may be different than the number of bytes
        // actually provided.
        uint64 size() const { return size_; }
        uint64 pos() const { return pos_; }

    protected:
        const uint8* buf_;
        uint64 size_;
        uint64 pos_;

        template <typename T>
        bool Read(T* t) WARN_UNUSED_RESULT;
    };

    class MEDIA_EXPORT BoxReader : public BufferReader {
    public:
        ~BoxReader();

        // Create a BoxReader from a buffer. Note that this function may return NULL
        // if an intact, complete box was not available in the buffer. If |*err| is
        // set, there was a stream-level error when creating the box; otherwise, NULL
        // values are only expected when insufficient data is available.
        //
        // |buf| is retained but not owned, and must outlive the BoxReader instance.
        static BoxReader* ReadTopLevelBox(const uint8* buf,
            const int buf_size,
            const scoped_refptr<MediaLog>& media_log,
            bool* err);

        // Read the box header from the current buffer. This function returns true if
        // there is enough data to read the header and the header is sane; that is, it
        // does not check to ensure the entire box is in the buffer before returning
        // true. The semantics of |*err| are the same as above.
        //
        // |buf| is not retained.
        static bool StartTopLevelBox(const uint8* buf,
            const int buf_size,
            const scoped_refptr<MediaLog>& media_log,
            FourCC* type,
            int* box_size,
            bool* err) WARN_UNUSED_RESULT;

        // Create a BoxReader from a buffer. |buf| must be the complete buffer, as
        // errors are returned when sufficient data is not available. |buf| can start
        // with any type of box -- it does not have to be IsValidTopLevelBox().
        //
        // |buf| is retained but not owned, and must outlive the BoxReader instance.
        static BoxReader* ReadConcatentatedBoxes(const uint8* buf,
            const int buf_size);

        // Returns true if |type| is recognized to be a top-level box, false
        // otherwise. This returns true for some boxes which we do not parse.
        // Helpful in debugging misaligned appends.
        static bool IsValidTopLevelBox(const FourCC& type,
            const scoped_refptr<MediaLog>& media_log);

        // Scan through all boxes within the current box, starting at the current
        // buffer position. Must be called before any of the *Child functions work.
        bool ScanChildren() WARN_UNUSED_RESULT;

        // Return true if child with type |child.BoxType()| exists.
        bool HasChild(Box* child) WARN_UNUSED_RESULT;

        // Read exactly one child box from the set of children. The type of the child
        // will be determined by the BoxType() method of |child|.
        bool ReadChild(Box* child) WARN_UNUSED_RESULT;

        // Read one child if available. Returns false on error, true on successful
        // read or on child absent.
        bool MaybeReadChild(Box* child) WARN_UNUSED_RESULT;

        // Read at least one child. False means error or no such child present.
        template <typename T>
        bool ReadChildren(
            std::vector<T>* children) WARN_UNUSED_RESULT;

        // Read any number of children. False means error.
        template <typename T>
        bool MaybeReadChildren(
            std::vector<T>* children) WARN_UNUSED_RESULT;

        // Read all children, regardless of FourCC. This is used from exactly one box,
        // corresponding to a rather significant inconsistency in the BMFF spec.
        // Note that this method is mutually exclusive with ScanChildren() and
        // ReadAllChildrenAndCheckFourCC().
        template <typename T>
        bool ReadAllChildren(std::vector<T>* children) WARN_UNUSED_RESULT;

        // Read all children and verify that the FourCC matches what is expected.
        // Returns true if all children are successfully parsed and have the correct
        // box type for |T|. Note that this method is mutually exclusive with
        // ScanChildren() and ReadAllChildren().
        template <typename T>
        bool ReadAllChildrenAndCheckFourCC(std::vector<T>* children)
            WARN_UNUSED_RESULT;

        // Populate the values of 'version()' and 'flags()' from a full box header.
        // Many boxes, but not all, use these values. This call should happen after
        // the box has been initialized, and does not re-read the main box header.
        bool ReadFullBoxHeader() WARN_UNUSED_RESULT;

        FourCC type() const { return type_; }
        uint8 version() const { return version_; }
        uint32 flags() const { return flags_; }

        const scoped_refptr<MediaLog>& media_log() const { return media_log_; }

    private:
        // Create a BoxReader from |buf|. |is_EOS| should be true if |buf| is
        // complete stream (i.e. no additional data is expected to be appended).
        BoxReader(const uint8* buf,
            const int size,
            const scoped_refptr<MediaLog>& media_log,
            bool is_EOS);

        // Must be called immediately after init. If the return is false, this
        // indicates that the box header and its contents were not available in the
        // stream or were nonsensical, and that the box must not be used further. In
        // this case, if |*err| is false, the problem was simply a lack of data, and
        // should only be an error condition if some higher-level component knows that
        // no more data is coming (i.e. EOS or end of containing box). If |*err| is
        // true, the error is unrecoverable and the stream should be aborted.
        bool ReadHeader(bool* err);

        // Read all children, optionally checking FourCC. Returns true if all
        // children are successfully parsed and, if |check_box_type|, have the
        // correct box type for |T|. Note that this method is mutually exclusive
        // with ScanChildren().
        template <typename T>
        bool ReadAllChildrenInternal(std::vector<T>* children, bool check_box_type);

        scoped_refptr<MediaLog> media_log_;
        FourCC type_;
        uint8 version_;
        uint32 flags_;

        typedef std::multimap<FourCC, BoxReader> ChildMap;

        // The set of child box FourCCs and their corresponding buffer readers. Only
        // valid if scanned_ is true.
        ChildMap children_;
        bool scanned_;

        // True if the buffer provided to the reader is the complete stream.
        const bool is_EOS_;
    };

    // Template definitions
    template <typename T>
    bool BoxReader::ReadChildren(std::vector<T>* children)
    {
        RCHECK(MaybeReadChildren(children) && !children->empty());
        return true;
    }

    template <typename T>
    bool BoxReader::MaybeReadChildren(std::vector<T>* children)
    {
        DCHECK(scanned_);
        DCHECK(children->empty());

        children->resize(1);
        FourCC child_type = (*children)[0].BoxType();

        ChildMap::iterator start_itr = children_.lower_bound(child_type);
        ChildMap::iterator end_itr = children_.upper_bound(child_type);
        children->resize(std::distance(start_itr, end_itr));
        typename std::vector<T>::iterator child_itr = children->begin();
        for (ChildMap::iterator itr = start_itr; itr != end_itr; ++itr) {
            RCHECK(child_itr->Parse(&itr->second));
            ++child_itr;
        }
        children_.erase(start_itr, end_itr);

        DVLOG(2) << "Found " << children->size() << " "
                 << FourCCToString(child_type) << " boxes.";
        return true;
    }

    template <typename T>
    bool BoxReader::ReadAllChildren(std::vector<T>* children)
    {
        return ReadAllChildrenInternal(children, false);
    }

    template <typename T>
    bool BoxReader::ReadAllChildrenAndCheckFourCC(std::vector<T>* children)
    {
        return ReadAllChildrenInternal(children, true);
    }

    template <typename T>
    bool BoxReader::ReadAllChildrenInternal(std::vector<T>* children,
        bool check_box_type)
    {
        DCHECK(!scanned_);
        scanned_ = true;

        bool err = false;
        while (pos_ < size_) {
            BoxReader child_reader(&buf_[pos_], size_ - pos_, media_log_, is_EOS_);
            if (!child_reader.ReadHeader(&err))
                break;
            T child;
            RCHECK(!check_box_type || child_reader.type() == child.BoxType());
            RCHECK(child.Parse(&child_reader));
            children->push_back(child);
            pos_ += child_reader.size();
        }

        return !err;
    }

} // namespace mp4
} // namespace media

#endif // MEDIA_FORMATS_MP4_BOX_READER_H_
