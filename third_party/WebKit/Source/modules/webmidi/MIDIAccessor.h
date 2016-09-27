/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef MIDIAccessor_h
#define MIDIAccessor_h

#include "public/platform/WebMIDIAccessor.h"
#include "public/platform/WebMIDIAccessorClient.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class MIDIAccessorClient;

class MIDIAccessor final : public WebMIDIAccessorClient {
public:
    static PassOwnPtr<MIDIAccessor> create(MIDIAccessorClient*);

    ~MIDIAccessor() override { }

    void startSession();
    void sendMIDIData(unsigned portIndex, const unsigned char* data, size_t length, double timeStamp);
    // MIDIAccessInitializer and MIDIAccess are both MIDIAccessClient.
    // MIDIAccessInitializer is the first client and MIDIAccess takes over it
    // once the initialization successfully finishes.
    void setClient(MIDIAccessorClient* client) { m_client = client; }

    // WebMIDIAccessorClient
    void didAddInputPort(const WebString& id, const WebString& manufacturer, const WebString& name, const WebString& version, MIDIPortState) override;
    void didAddOutputPort(const WebString& id, const WebString& manufacturer, const WebString& name, const WebString& version, MIDIPortState) override;
    void didSetInputPortState(unsigned portIndex, MIDIPortState) override;
    void didSetOutputPortState(unsigned portIndex, MIDIPortState) override;
    void didStartSession(bool success, const WebString& error, const WebString& message) override;
    void didReceiveMIDIData(unsigned portIndex, const unsigned char* data, size_t length, double timeStamp) override;

private:
    explicit MIDIAccessor(MIDIAccessorClient*);

    MIDIAccessorClient* m_client;
    OwnPtr<WebMIDIAccessor> m_accessor;
};

} // namespace blink

#endif // MIDIAccessor_h
