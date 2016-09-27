/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#include "public/platform/WebMediaStreamSource.h"

#include "platform/audio/AudioBus.h"
#include "platform/mediastream/MediaStreamSource.h"
#include "public/platform/WebAudioDestinationConsumer.h"
#include "public/platform/WebMediaConstraints.h"
#include "public/platform/WebString.h"
#include "wtf/MainThread.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

namespace {

class ExtraDataContainer : public MediaStreamSource::ExtraData {
public:
    ExtraDataContainer(PassOwnPtr<WebMediaStreamSource::ExtraData> extraData) : m_extraData(extraData) { }

    WebMediaStreamSource::ExtraData* extraData() { return m_extraData.get(); }

private:
    OwnPtr<WebMediaStreamSource::ExtraData> m_extraData;
};

} // namespace

WebMediaStreamSource WebMediaStreamSource::ExtraData::owner()
{
    ASSERT(m_owner);
    return WebMediaStreamSource(m_owner);
}

void WebMediaStreamSource::ExtraData::setOwner(MediaStreamSource* owner)
{
    ASSERT(!m_owner);
    m_owner = owner;
}

WebMediaStreamSource::WebMediaStreamSource(const PassRefPtr<MediaStreamSource>& mediaStreamSource)
    : m_private(mediaStreamSource)
{
}

WebMediaStreamSource& WebMediaStreamSource::operator=(MediaStreamSource* mediaStreamSource)
{
    m_private = mediaStreamSource;
    return *this;
}

void WebMediaStreamSource::assign(const WebMediaStreamSource& other)
{
    m_private = other.m_private;
}

void WebMediaStreamSource::reset()
{
    m_private.reset();
}

WebMediaStreamSource::operator PassRefPtr<MediaStreamSource>() const
{
    return m_private.get();
}

WebMediaStreamSource::operator MediaStreamSource*() const
{
    return m_private.get();
}

void WebMediaStreamSource::initialize(const WebString& id, Type type, const WebString& name)
{
    m_private = MediaStreamSource::create(id, static_cast<MediaStreamSource::Type>(type), name, false, true);
}

void WebMediaStreamSource::initialize(const WebString& id, Type type, const WebString& name, bool remote, bool readonly)
{
    m_private = MediaStreamSource::create(id, static_cast<MediaStreamSource::Type>(type), name, remote, readonly);
}

WebString WebMediaStreamSource::id() const
{
    ASSERT(!m_private.isNull());
    return m_private.get()->id();
}

WebMediaStreamSource::Type WebMediaStreamSource::type() const
{
    ASSERT(!m_private.isNull());
    return static_cast<Type>(m_private.get()->type());
}

WebString WebMediaStreamSource::name() const
{
    ASSERT(!m_private.isNull());
    return m_private.get()->name();
}

void WebMediaStreamSource::setReadyState(ReadyState state)
{
    ASSERT(!m_private.isNull());
    m_private->setReadyState(static_cast<MediaStreamSource::ReadyState>(state));
}

WebMediaStreamSource::ReadyState WebMediaStreamSource::readyState() const
{
    ASSERT(!m_private.isNull());
    return static_cast<ReadyState>(m_private->readyState());
}

WebMediaStreamSource::ExtraData* WebMediaStreamSource::extraData() const
{
    ASSERT(!m_private.isNull());
    MediaStreamSource::ExtraData* data = m_private->extraData();
    if (!data)
        return 0;
    return static_cast<ExtraDataContainer*>(data)->extraData();
}

void WebMediaStreamSource::setExtraData(ExtraData* extraData)
{
    ASSERT(!m_private.isNull());

    if (extraData)
        extraData->setOwner(m_private.get());

    m_private->setExtraData(adoptPtr(new ExtraDataContainer(adoptPtr(extraData))));
}

WebMediaConstraints WebMediaStreamSource::constraints()
{
    ASSERT(!m_private.isNull());
    return m_private->constraints();
}

bool WebMediaStreamSource::requiresAudioConsumer() const
{
    ASSERT(!m_private.isNull());
    return m_private->requiresAudioConsumer();
}

class ConsumerWrapper final : public AudioDestinationConsumer {
public:
    static ConsumerWrapper* create(WebAudioDestinationConsumer* consumer)
    {
        return new ConsumerWrapper(consumer);
    }

    void setFormat(size_t numberOfChannels, float sampleRate) override;
    void consumeAudio(AudioBus*, size_t numberOfFrames) override;

    WebAudioDestinationConsumer* consumer() { return m_consumer; }

private:
    explicit ConsumerWrapper(WebAudioDestinationConsumer* consumer) : m_consumer(consumer) { }

    // m_consumer is not owned by this class.
    WebAudioDestinationConsumer* m_consumer;
};

void ConsumerWrapper::setFormat(size_t numberOfChannels, float sampleRate)
{
    m_consumer->setFormat(numberOfChannels, sampleRate);
}

void ConsumerWrapper::consumeAudio(AudioBus* bus, size_t numberOfFrames)
{
    if (!bus)
        return;

    // Wrap AudioBus.
    size_t numberOfChannels = bus->numberOfChannels();
    WebVector<const float*> busVector(numberOfChannels);
    for (size_t i = 0; i < numberOfChannels; ++i)
        busVector[i] = bus->channel(i)->data();

    m_consumer->consumeAudio(busVector, numberOfFrames);
}

void WebMediaStreamSource::addAudioConsumer(WebAudioDestinationConsumer* consumer)
{
    ASSERT(isMainThread());
    ASSERT(!m_private.isNull() && consumer);

    m_private->addAudioConsumer(ConsumerWrapper::create(consumer));
}

bool WebMediaStreamSource::removeAudioConsumer(WebAudioDestinationConsumer* consumer)
{
    ASSERT(isMainThread());
    ASSERT(!m_private.isNull() && consumer);

    const HeapHashSet<Member<AudioDestinationConsumer>>& consumers = m_private->audioConsumers();
    for (HeapHashSet<Member<AudioDestinationConsumer>>::const_iterator it = consumers.begin(); it != consumers.end(); ++it) {
        ConsumerWrapper* wrapper = static_cast<ConsumerWrapper*>(it->get());
        if (wrapper->consumer() == consumer) {
            m_private->removeAudioConsumer(wrapper);
            return true;
        }
    }
    return false;
}

} // namespace blink
