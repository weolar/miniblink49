// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LinkHeader_h
#define LinkHeader_h

#include "core/CoreExport.h"
#include "core/html/CrossOriginAttribute.h"
#include "wtf/text/WTFString.h"

namespace blink {

class LinkHeader {
public:
    template <typename CharType>
    LinkHeader(CharType*& position, CharType* end);

    const String url() const { return m_url; }
    const String rel() const { return m_rel; }
    CrossOriginAttributeValue crossOrigin() const { return m_crossOrigin; }
    bool valid() const { return m_isValid; }

    enum LinkParameterName {
        LinkParameterUnknown,
        LinkParameterRel,
        LinkParameterAnchor,
        LinkParameterCrossOrigin,
    };

private:
    void setValue(LinkParameterName, String value);

    String m_url;
    String m_rel;
    CrossOriginAttributeValue m_crossOrigin;
    bool m_isValid;
};

class CORE_EXPORT LinkHeaderSet {
public:
    LinkHeaderSet(const String& header);

    Vector<LinkHeader>::const_iterator begin() const { return m_headerSet.begin(); }
    Vector<LinkHeader>::const_iterator end() const { return m_headerSet.end(); }
    LinkHeader& operator[](size_t i) { return m_headerSet[i]; }
    size_t size() { return m_headerSet.size(); }

private:
    template <typename CharType>
    void init(CharType* headerValue, unsigned len);

    Vector<LinkHeader> m_headerSet;
};

}

#endif
