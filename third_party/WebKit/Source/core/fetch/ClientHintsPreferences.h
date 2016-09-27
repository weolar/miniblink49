// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ClientHintsPreferences_h
#define ClientHintsPreferences_h

#include "core/CoreExport.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ResourceFetcher;

class ClientHintsPreferences {
public:
    ClientHintsPreferences()
        : m_shouldSendDPR(false)
        , m_shouldSendResourceWidth(false)
        , m_shouldSendViewportWidth(false)
    {
    }

    void set(const ClientHintsPreferences& other)
    {
        m_shouldSendDPR = other.m_shouldSendDPR;
        m_shouldSendResourceWidth = other.m_shouldSendResourceWidth;
        m_shouldSendViewportWidth = other.m_shouldSendViewportWidth;
    }

    void setShouldSendDPR(bool should) { m_shouldSendDPR = should; }
    void setShouldSendResourceWidth(bool should) { m_shouldSendResourceWidth = should; }
    void setShouldSendViewportWidth(bool should) { m_shouldSendViewportWidth = should; }
    bool shouldSendDPR() const { return m_shouldSendDPR; }
    bool shouldSendResourceWidth() const { return m_shouldSendResourceWidth; }
    bool shouldSendViewportWidth() const { return m_shouldSendViewportWidth; }

private:
    bool m_shouldSendDPR;
    bool m_shouldSendResourceWidth;
    bool m_shouldSendViewportWidth;
};

CORE_EXPORT void handleAcceptClientHintsHeader(const String& headerValue, ClientHintsPreferences&, ResourceFetcher*);
} // namespace blink
#endif

