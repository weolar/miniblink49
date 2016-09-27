/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BlobData_h
#define BlobData_h

#include "platform/FileMetadata.h"
#include "platform/weborigin/KURL.h"
#include "wtf/Forward.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/ThreadSafeRefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class BlobDataHandle;

class PLATFORM_EXPORT RawData : public ThreadSafeRefCounted<RawData> {
public:
    static PassRefPtr<RawData> create()
    {
        return adoptRef(new RawData());
    }

    void detachFromCurrentThread();

    const char* data() const { return m_data.data(); }
    size_t length() const { return m_data.size(); }
    Vector<char>* mutableData() { return &m_data; }

private:
    RawData();

    Vector<char> m_data;
};

struct PLATFORM_EXPORT BlobDataItem {
    static const long long toEndOfFile;

    // Default constructor.
    BlobDataItem()
        : type(Data)
        , offset(0)
        , length(toEndOfFile)
        , expectedModificationTime(invalidFileTime())
    {
    }

    // Constructor for String type (complete string).
    explicit BlobDataItem(PassRefPtr<RawData> data)
        : type(Data)
        , data(data)
        , offset(0)
        , length(toEndOfFile)
        , expectedModificationTime(invalidFileTime())
    {
    }

    // Constructor for File type (complete file).
    explicit BlobDataItem(const String& path)
        : type(File)
        , path(path)
        , offset(0)
        , length(toEndOfFile)
        , expectedModificationTime(invalidFileTime())
    {
    }

    // Constructor for File type (partial file).
    BlobDataItem(const String& path, long long offset, long long length, double expectedModificationTime)
        : type(File)
        , path(path)
        , offset(offset)
        , length(length)
        , expectedModificationTime(expectedModificationTime)
    {
    }

    // Constructor for Blob type.
    BlobDataItem(PassRefPtr<BlobDataHandle> blobDataHandle, long long offset, long long length)
        : type(Blob)
        , blobDataHandle(blobDataHandle)
        , offset(offset)
        , length(length)
        , expectedModificationTime(invalidFileTime())
    {
    }

    // Constructor for FileSystem file type.
    BlobDataItem(const KURL& fileSystemURL, long long offset, long long length, double expectedModificationTime)
        : type(FileSystemURL)
        , fileSystemURL(fileSystemURL)
        , offset(offset)
        , length(length)
        , expectedModificationTime(expectedModificationTime)
    {
    }

    // Detaches from current thread so that it can be passed to another thread.
    void detachFromCurrentThread();

    const enum {
        Data,
        File,
        Blob,
        FileSystemURL
    } type;

    RefPtr<RawData> data; // For Data type.
    String path; // For File type.
    KURL fileSystemURL; // For FileSystemURL type.
    RefPtr<BlobDataHandle> blobDataHandle; // For Blob type.

    long long offset;
    long long length;
    double expectedModificationTime;

private:
    friend class BlobData;

    // Constructor for String type (partial string).
    BlobDataItem(PassRefPtr<RawData> data, long long offset, long long length)
        : type(Data)
        , data(data)
        , offset(offset)
        , length(length)
        , expectedModificationTime(invalidFileTime())
    {
    }
};

typedef Vector<BlobDataItem> BlobDataItemList;

class PLATFORM_EXPORT BlobData {
    WTF_MAKE_FAST_ALLOCATED(BlobData);
public:
    static PassOwnPtr<BlobData> create();

    // Detaches from current thread so that it can be passed to another thread.
    void detachFromCurrentThread();

    const String& contentType() const { return m_contentType; }
    void setContentType(const String&);

    const BlobDataItemList& items() const { return m_items; }

    void appendBytes(const void*, size_t length);
    void appendData(PassRefPtr<RawData>, long long offset, long long length);
    void appendFile(const String& path);
    void appendFile(const String& path, long long offset, long long length, double expectedModificationTime);
    void appendBlob(PassRefPtr<BlobDataHandle>, long long offset, long long length);
    void appendFileSystemURL(const KURL&, long long offset, long long length, double expectedModificationTime);
    void appendText(const String&, bool normalizeLineEndingsToNative);

    // The value of the size property for a Blob who has this data.
    // BlobDataItem::toEndOfFile if the Blob has a file whose size was not yet determined.
    long long length() const;

private:
    friend class BlobDataTest_Consolidation_Test;

    BlobData() { }

    bool canConsolidateData(size_t length);

    // Make this private so that the otherwise-generated implicit assignment
    // operator doesn't reference BlobDataItemList's operator=, which would
    // require BlobDataItem to have an implicit operator= which it can't have
    // because it has a const member.
    BlobData& operator=(const BlobData&);

    String m_contentType;
    BlobDataItemList m_items;
};


class PLATFORM_EXPORT BlobDataHandle : public ThreadSafeRefCounted<BlobDataHandle> {
public:
    // For empty blob construction.
    static PassRefPtr<BlobDataHandle> create()
    {
        return adoptRef(new BlobDataHandle());
    }

    // For initial creation.
    static PassRefPtr<BlobDataHandle> create(PassOwnPtr<BlobData> data, long long size)
    {
        return adoptRef(new BlobDataHandle(data, size));
    }

    // For deserialization of script values and ipc messages.
    static PassRefPtr<BlobDataHandle> create(const String& uuid, const String& type, long long size)
    {
        return adoptRef(new BlobDataHandle(uuid, type, size));
    }

    String uuid() const { return m_uuid.isolatedCopy(); }
    String type() const { return m_type.isolatedCopy(); }
    unsigned long long size() { return m_size; }

    ~BlobDataHandle();

private:
    BlobDataHandle();
    BlobDataHandle(PassOwnPtr<BlobData>, long long size);
    BlobDataHandle(const String& uuid, const String& type, long long size);

    const String m_uuid;
    const String m_type;
    const long long m_size;
};

} // namespace blink

#endif // BlobData_h
