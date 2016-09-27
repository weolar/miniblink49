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

#include "config.h"
#include "modules/webmidi/MIDIAccessor.h"

#include "modules/webmidi/MIDIAccessorClient.h"
#include "public/platform/Platform.h"
#include "wtf/text/WTFString.h"

using blink::WebString;

namespace blink {

// Factory method
PassOwnPtr<MIDIAccessor> MIDIAccessor::create(MIDIAccessorClient* client)
{
    return adoptPtr(new MIDIAccessor(client));
}

MIDIAccessor::MIDIAccessor(MIDIAccessorClient* client)
    : m_client(client)
{
    ASSERT(client);

    m_accessor = adoptPtr(Platform::current()->createMIDIAccessor(this));

    ASSERT(m_accessor);
}

void MIDIAccessor::startSession()
{
    m_accessor->startSession();
}

void MIDIAccessor::sendMIDIData(unsigned portIndex, const unsigned char* data, size_t length, double timeStamp)
{
    m_accessor->sendMIDIData(portIndex, data, length, timeStamp);
}

void MIDIAccessor::didAddInputPort(const WebString& id, const WebString& manufacturer, const WebString& name, const WebString& version, MIDIPortState state)
{
    m_client->didAddInputPort(id, manufacturer, name, version, state);
}

void MIDIAccessor::didAddOutputPort(const WebString& id, const WebString& manufacturer, const WebString& name, const WebString& version, MIDIPortState state)
{
    m_client->didAddOutputPort(id, manufacturer, name, version, state);
}

void MIDIAccessor::didSetInputPortState(unsigned portIndex, MIDIPortState state)
{
    m_client->didSetInputPortState(portIndex, state);
}

void MIDIAccessor::didSetOutputPortState(unsigned portIndex, MIDIPortState state)
{
    m_client->didSetOutputPortState(portIndex, state);
}

void MIDIAccessor::didStartSession(bool success, const WebString& error, const WebString& message)
{
    m_client->didStartSession(success, error, message);
}

void MIDIAccessor::didReceiveMIDIData(unsigned portIndex, const unsigned char* data, size_t length, double timeStamp)
{
    m_client->didReceiveMIDIData(portIndex, data, length, timeStamp);
}

} // namespace blink
