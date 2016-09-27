/*
 * Copyright (C) 2005, 2006, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef FormDataList_h
#define FormDataList_h

#include "core/CoreExport.h"
#include "core/fileapi/Blob.h"
#include "core/fileapi/File.h"
#include "platform/heap/Handle.h"
#include "platform/network/FormData.h"
#include "wtf/Forward.h"
#include "wtf/text/CString.h"
#include "wtf/text/TextEncoding.h"

namespace blink {

class CORE_EXPORT FormDataList : public GarbageCollected<FormDataList> {
public:
    class Entry final {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        enum Type { None, StringType, FileType };

        Entry() : m_type(None) { }
        Entry(const String& name, const String& value) : m_type(StringType), m_name(name), m_string(value) { }
        Entry(const String& name, File* value) : m_type(FileType), m_name(name), m_file(value) { }

        bool isNone() const { return m_type == None; }
        bool isString() const { return m_type == StringType; }
        bool isFile() const { return m_type == FileType; }

        const String& name() const { ASSERT(m_type != None); return m_name; }
        const String& string() const { ASSERT(m_type == StringType); return m_string; }
        File* file() const { ASSERT(m_type == FileType); return m_file; }

        DECLARE_TRACE();

    private:
        const Type m_type;
        const String m_name;
        const String m_string;
        const Member<File> m_file;
    };

    class Item {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        Item() { }
        Item(const WTF::CString& data) : m_data(data) { }
        Item(Blob* blob, const String& filename) : m_blob(blob), m_filename(filename) { }

        const WTF::CString& data() const { return m_data; }
        Blob* blob() const { return m_blob.get(); }
        const String& filename() const { return m_filename; }

        DECLARE_TRACE();

    private:
        WTF::CString m_data;
        Member<Blob> m_blob;
        String m_filename;
    };

    static FormDataList* create(const WTF::TextEncoding& encoding)
    {
        return new FormDataList(encoding);
    }

    using FormDataListItems = HeapVector<FormDataList::Item>;

    void appendData(const String& key, const String& value)
    {
        appendString(key);
        appendString(value);
    }
    void appendData(const String& key, const CString& value)
    {
        appendString(key);
        appendString(value);
    }
    void appendData(const String& key, int value)
    {
        appendString(key);
        appendString(String::number(value));
    }
    void appendBlob(const String& key, Blob* blob, const String& filename = String())
    {
        appendString(key);
        appendBlob(blob, filename);
    }

    void deleteEntry(const String& key);
    Entry getEntry(const String& key) const;
    Entry getEntry(size_t index) const;
    HeapVector<Entry> getAll(const String& key) const;
    bool hasEntry(const String& key) const;
    void setBlob(const String& key, Blob*, const String& filename);
    void setData(const String& key, const String& value);
    size_t size() const { return m_items.size() / 2; }

    const FormDataListItems& items() const { return m_items; }
    const WTF::TextEncoding& encoding() const { return m_encoding; }

    PassRefPtr<FormData> createFormData(FormData::EncodingType = FormData::FormURLEncoded);
    PassRefPtr<FormData> createMultiPartFormData();

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit FormDataList(const WTF::TextEncoding&);

private:
    void appendKeyValuePairItemsTo(FormData*, const WTF::TextEncoding&, bool isMultiPartForm, FormData::EncodingType = FormData::FormURLEncoded);

    void appendString(const CString&);
    void appendString(const String&);
    void appendBlob(Blob*, const String& filename);
    void setEntry(const String& key, const Item&);
    Entry itemsToEntry(const Item& key, const Item& value) const;
    CString encodeAndNormalize(const String& key) const;

    WTF::TextEncoding m_encoding;
    FormDataListItems m_items;
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::FormDataList::Entry);
WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::FormDataList::Item);

#endif // FormDataList_h
