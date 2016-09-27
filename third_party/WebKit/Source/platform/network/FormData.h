/*
 * Copyright (C) 2004, 2006, 2008, 2011 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef FormData_h
#define FormData_h

#include "platform/blob/BlobData.h"
#include "platform/weborigin/KURL.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class BlobDataHandle;

class PLATFORM_EXPORT FormDataElement {
public:
    FormDataElement() : m_type(data) { }
    explicit FormDataElement(const Vector<char>& array) : m_type(data), m_data(array) { }

    FormDataElement(const String& filename, long long fileStart, long long fileLength, double expectedFileModificationTime) : m_type(encodedFile), m_filename(filename), m_fileStart(fileStart), m_fileLength(fileLength), m_expectedFileModificationTime(expectedFileModificationTime) { }
    explicit FormDataElement(const String& blobUUID, PassRefPtr<BlobDataHandle> optionalHandle) : m_type(encodedBlob), m_blobUUID(blobUUID), m_optionalBlobDataHandle(optionalHandle) { }
    FormDataElement(const KURL& fileSystemURL, long long start, long long length, double expectedFileModificationTime) : m_type(encodedFileSystemURL), m_fileSystemURL(fileSystemURL), m_fileStart(start), m_fileLength(length), m_expectedFileModificationTime(expectedFileModificationTime) { }

    enum Type {
        data,
        encodedFile,
        encodedBlob,
        encodedFileSystemURL
    } m_type;
    Vector<char> m_data;
    String m_filename;
    String m_blobUUID;
    RefPtr<BlobDataHandle> m_optionalBlobDataHandle;
    KURL m_fileSystemURL;
    long long m_fileStart;
    long long m_fileLength;
    double m_expectedFileModificationTime;
};

inline bool operator==(const FormDataElement& a, const FormDataElement& b)
{
    if (&a == &b)
        return true;

    if (a.m_type != b.m_type)
        return false;
    if (a.m_type == FormDataElement::data)
        return a.m_data == b.m_data;
    if (a.m_type == FormDataElement::encodedFile)
        return a.m_filename == b.m_filename && a.m_fileStart == b.m_fileStart && a.m_fileLength == b.m_fileLength && a.m_expectedFileModificationTime == b.m_expectedFileModificationTime;
    if (a.m_type == FormDataElement::encodedBlob)
        return a.m_blobUUID == b.m_blobUUID;
    if (a.m_type == FormDataElement::encodedFileSystemURL)
        return a.m_fileSystemURL == b.m_fileSystemURL;

    return true;
}

inline bool operator!=(const FormDataElement& a, const FormDataElement& b)
{
    return !(a == b);
}

class PLATFORM_EXPORT FormData : public RefCounted<FormData> {
public:
    enum EncodingType {
        FormURLEncoded, // for application/x-www-form-urlencoded
        TextPlain, // for text/plain
        MultipartFormData // for multipart/form-data
    };

    static PassRefPtr<FormData> create();
    static PassRefPtr<FormData> create(const void*, size_t);
    static PassRefPtr<FormData> create(const CString&);
    static PassRefPtr<FormData> create(const Vector<char>&);
    PassRefPtr<FormData> copy() const;
    PassRefPtr<FormData> deepCopy() const;
    ~FormData();

    void appendData(const void* data, size_t);
    void appendFile(const String& filePath);
    void appendFileRange(const String& filename, long long start, long long length, double expectedModificationTime);
    void appendBlob(const String& blobUUID, PassRefPtr<BlobDataHandle> optionalHandle);
    void appendFileSystemURL(const KURL&);
    void appendFileSystemURLRange(const KURL&, long long start, long long length, double expectedModificationTime);

    void flatten(Vector<char>&) const; // omits files
    String flattenToString() const; // omits files

    bool isEmpty() const { return m_elements.isEmpty(); }
    const Vector<FormDataElement>& elements() const { return m_elements; }

    const Vector<char>& boundary() const { return m_boundary; }
    void setBoundary(Vector<char> boundary) { m_boundary = boundary; }

    // Identifies a particular form submission instance.  A value of 0 is used
    // to indicate an unspecified identifier.
    void setIdentifier(int64_t identifier) { m_identifier = identifier; }
    int64_t identifier() const { return m_identifier; }

    bool containsPasswordData() const { return m_containsPasswordData; }
    void setContainsPasswordData(bool containsPasswordData) { m_containsPasswordData = containsPasswordData; }

    static EncodingType parseEncodingType(const String& type)
    {
        if (equalIgnoringCase(type, "text/plain"))
            return TextPlain;
        if (equalIgnoringCase(type, "multipart/form-data"))
            return MultipartFormData;
        return FormURLEncoded;
    }

    // Size of the elements making up the FormData.
    unsigned long long sizeInBytes() const;

private:
    FormData();
    FormData(const FormData&);

    Vector<FormDataElement> m_elements;

    int64_t m_identifier;
    Vector<char> m_boundary;
    bool m_containsPasswordData;
};

inline bool operator==(const FormData& a, const FormData& b)
{
    return a.elements() == b.elements();
}

inline bool operator!=(const FormData& a, const FormData& b)
{
    return !(a == b);
}

} // namespace blink

#endif
