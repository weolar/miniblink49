// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/FetchHeaderList.h"

#include "core/fetch/FetchUtils.h"
#include "platform/network/HTTPParsers.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

FetchHeaderList* FetchHeaderList::create()
{
    return new FetchHeaderList();
}

FetchHeaderList* FetchHeaderList::clone()
{
    FetchHeaderList* list = create();
    for (size_t i = 0; i < m_headerList.size(); ++i)
        list->append(m_headerList[i]->first, m_headerList[i]->second);
    return list;
}

FetchHeaderList::FetchHeaderList()
{
}

FetchHeaderList::~FetchHeaderList()
{
}

void FetchHeaderList::append(const String& name, const String& value)
{
    // "To append a name/value (|name|/|value|) pair to a header list (|list|),
    // append a new header whose name is |name|, byte lowercased, and value is
    // |value|, to |list|."
    m_headerList.append(adoptPtr(new Header(name.lower(), value)));
}

void FetchHeaderList::set(const String& name, const String& value)
{
    // "To set a name/value (|name|/|value|) pair in a header list (|list|), run
    // these steps:
    // 1. Byte lowercase |name|.
    // 2. If there are any headers in |list| whose name is |name|, set the value
    //    of the first such header to |value| and remove the others.
    // 3. Otherwise, append a new header whose name is |name| and value is
    //    |value|, to |list|."
    const String lowercasedName = name.lower();
    for (size_t i = 0; i < m_headerList.size(); ++i) {
        if (m_headerList[i]->first == lowercasedName) {
            m_headerList[i]->second = value;
            for (size_t j = i + 1; j < m_headerList.size(); ) {
                if (m_headerList[j]->first == lowercasedName)
                    m_headerList.remove(j);
                else
                    ++j;
            }
            return;
        }
    }
    m_headerList.append(adoptPtr(new Header(lowercasedName, value)));
}

String FetchHeaderList::extractMIMEType() const
{
    // To extract a MIME type from a header list (headers), run these steps:
    // 1. Let MIMEType be the result of parsing `Content-Type` in headers.
    String mimeType;
    if (!get("Content-Type", mimeType)) {
        // 2. If MIMEType is null or failure, return the empty byte sequence.
        return String();
    }
    // 3. Return MIMEType, byte lowercased.
    return mimeType.lower();
}

size_t FetchHeaderList::size() const
{
    return m_headerList.size();
}

void FetchHeaderList::remove(const String& name)
{
    // "To delete a name (|name|) from a header list (|list|), remove all headers
    // whose name is |name|, byte lowercased, from |list|."
    const String lowercasedName = name.lower();
    for (size_t i = 0; i < m_headerList.size(); ) {
        if (m_headerList[i]->first == lowercasedName)
            m_headerList.remove(i);
        else
            ++i;
    }
}

bool FetchHeaderList::get(const String& name, String& result) const
{
    const String lowercasedName = name.lower();
    for (size_t i = 0; i < m_headerList.size(); ++i) {
        if (m_headerList[i]->first == lowercasedName) {
            result = m_headerList[i]->second;
            return true;
        }
    }
    return false;
}

void FetchHeaderList::getAll(const String& name, Vector<String>& result) const
{
    const String lowercasedName = name.lower();
    result.clear();
    for (size_t i = 0; i < m_headerList.size(); ++i) {
        if (m_headerList[i]->first == lowercasedName)
            result.append(m_headerList[i]->second);
    }
}

bool FetchHeaderList::has(const String& name) const
{
    const String lowercasedName = name.lower();
    for (size_t i = 0; i < m_headerList.size(); ++i) {
        if (m_headerList[i]->first == lowercasedName)
            return true;
    }
    return false;
}

void FetchHeaderList::clearList()
{
    m_headerList.clear();
}

bool FetchHeaderList::containsNonSimpleHeader() const
{
    for (size_t i = 0; i < m_headerList.size(); ++i) {
        if (!FetchUtils::isSimpleHeader(AtomicString(m_headerList[i]->first), AtomicString(m_headerList[i]->second)))
            return true;
    }
    return false;
}

bool FetchHeaderList::isValidHeaderName(const String& name)
{
    // "A name is a case-insensitive byte sequence that matches the field-name
    // token production."
    return isValidHTTPToken(name);
}

bool FetchHeaderList::isValidHeaderValue(const String& value)
{
    // "A value is a byte sequence that matches the field-value token production
    // and contains no 0x0A or 0x0D bytes."
    return isValidHTTPHeaderValue(value);
}

} // namespace blink
