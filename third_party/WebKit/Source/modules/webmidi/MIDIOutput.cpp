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
#include "modules/webmidi/MIDIOutput.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/timing/DOMWindowPerformance.h"
#include "core/timing/Performance.h"
#include "modules/webmidi/MIDIAccess.h"

namespace blink {

using PortState = MIDIAccessor::MIDIPortState;

namespace {

double now(ExecutionContext* context)
{
    LocalDOMWindow* window = context ? context->executingWindow() : nullptr;
    Performance* performance = window ? DOMWindowPerformance::performance(*window) : nullptr;
    return performance ? performance->now() : 0.0;
}

class MessageValidator {
public:
    static bool validate(DOMUint8Array* array, ExceptionState& exceptionState, bool sysexEnabled)
    {
        MessageValidator validator(array);
        return validator.process(exceptionState, sysexEnabled);
    }
private:
    MessageValidator(DOMUint8Array* array)
        : m_data(array->data())
        , m_length(array->length())
        , m_offset(0) { }

    bool process(ExceptionState& exceptionState, bool sysexEnabled)
    {
        while (!isEndOfData() && acceptRealTimeMessages()) {
            if (!isStatusByte()) {
                exceptionState.throwTypeError("Running status is not allowed " + getPositionString());
                return false;
            }
            if (isEndOfSysex()) {
                exceptionState.throwTypeError("Unexpected end of system exclusive message " + getPositionString());
                return false;
            }
            if (isReservedStatusByte()) {
                exceptionState.throwTypeError("Reserved status is not allowed " + getPositionString());
                return false;
            }
            if (isSysex()) {
                if (!sysexEnabled) {
                    exceptionState.throwDOMException(InvalidAccessError, "System exclusive message is not allowed " + getPositionString());
                    return false;
                }
                if (!acceptCurrentSysex()) {
                    if (isEndOfData())
                        exceptionState.throwTypeError("System exclusive message is not ended by end of system exclusive message.");
                    else
                        exceptionState.throwTypeError("System exclusive message contains a status byte " + getPositionString());
                    return false;
                }
            } else {
                if (!acceptCurrentMessage()) {
                    if (isEndOfData())
                        exceptionState.throwTypeError("Message is incomplete.");
                    else
                        exceptionState.throwTypeError("Unexpected status byte at index " + getPositionString());
                    return false;
                }
            }
        }
        return true;
    }

private:
    bool isEndOfData() { return m_offset >= m_length; }
    bool isSysex() { return m_data[m_offset] == 0xf0; }
    bool isSystemMessage() { return m_data[m_offset] >= 0xf0; }
    bool isEndOfSysex() { return m_data[m_offset] == 0xf7; }
    bool isRealTimeMessage() { return m_data[m_offset] >= 0xf8; }
    bool isStatusByte() { return m_data[m_offset] & 0x80; }
    bool isReservedStatusByte() { return m_data[m_offset] == 0xf4 || m_data[m_offset] == 0xf5 || m_data[m_offset] == 0xf9 || m_data[m_offset] == 0xfd; }

    bool acceptRealTimeMessages()
    {
        for (; !isEndOfData(); m_offset++) {
            if (isRealTimeMessage() && !isReservedStatusByte())
                continue;
            return true;
        }
        return false;
    }

    bool acceptCurrentSysex()
    {
        ASSERT(isSysex());
        for (m_offset++; !isEndOfData(); m_offset++) {
            if (isReservedStatusByte())
                return false;
            if (isRealTimeMessage())
                continue;
            if (isEndOfSysex()) {
                m_offset++;
                return true;
            }
            if (isStatusByte())
                return false;
        }
        return false;
    }

    bool acceptCurrentMessage()
    {
        ASSERT(isStatusByte());
        ASSERT(!isSysex());
        ASSERT(!isReservedStatusByte());
        ASSERT(!isRealTimeMessage());
        static const int channelMessageLength[7] = { 3, 3, 3, 3, 2, 2, 3 }; // for 0x8*, 0x9*, ..., 0xe*
        static const int systemMessageLength[7] = { 2, 3, 2, 0, 0, 1, 0 }; // for 0xf1, 0xf2, ..., 0xf7
        size_t length = isSystemMessage() ? systemMessageLength[m_data[m_offset] - 0xf1] : channelMessageLength[(m_data[m_offset] >> 4) - 8];
        size_t count = 1;
        for (m_offset++; !isEndOfData(); m_offset++) {
            if (isReservedStatusByte())
                return false;
            if (isRealTimeMessage())
                continue;
            if (isStatusByte())
                return false;
            if (++count == length) {
                m_offset++;
                return true;
            }
        }
        return false;
    }

    String getPositionString() { return "at index " + String::number(m_offset) + " (" + String::number(m_data[m_offset]) + ")."; }

    const unsigned char* m_data;
    const size_t m_length;
    size_t m_offset;
};

} // namespace

MIDIOutput* MIDIOutput::create(MIDIAccess* access, unsigned portIndex, const String& id, const String& manufacturer, const String& name, const String& version, PortState state)
{
    ASSERT(access);
    MIDIOutput* output = new MIDIOutput(access, portIndex, id, manufacturer, name, version, state);
    output->suspendIfNeeded();
    return output;
}

MIDIOutput::MIDIOutput(MIDIAccess* access, unsigned portIndex, const String& id, const String& manufacturer, const String& name, const String& version, PortState state)
    : MIDIPort(access, id, manufacturer, name, TypeOutput, version, state)
    , m_portIndex(portIndex)
{
}

MIDIOutput::~MIDIOutput()
{
}

void MIDIOutput::send(DOMUint8Array* array, double timestamp, ExceptionState& exceptionState)
{
    if (timestamp == 0.0)
        timestamp = now(executionContext());

    // Implicit open. It does nothing if the port is already opened.
    // This should be performed even if |array| is invalid.
    open();

    if (!array)
        return;

    if (MessageValidator::validate(array, exceptionState, midiAccess()->sysexEnabled()))
        midiAccess()->sendMIDIData(m_portIndex, array->data(), array->length(), timestamp);
}

void MIDIOutput::send(Vector<unsigned> unsignedData, double timestamp, ExceptionState& exceptionState)
{
    if (timestamp == 0.0)
        timestamp = now(executionContext());

    RefPtr<DOMUint8Array> array = DOMUint8Array::create(unsignedData.size());
    DOMUint8Array::ValueType* const arrayData = array->data();
    const uint32_t arrayLength = array->length();

    for (size_t i = 0; i < unsignedData.size(); ++i) {
        if (unsignedData[i] > 0xff) {
            exceptionState.throwTypeError("The value at index " + String::number(i) + " (" + String::number(unsignedData[i]) + ") is greater than 0xFF.");
            return;
        }
        if (i < arrayLength)
            arrayData[i] = unsignedData[i] & 0xff;
    }

    send(array.get(), timestamp, exceptionState);
}

void MIDIOutput::send(DOMUint8Array* data, ExceptionState& exceptionState)
{
    send(data, 0.0, exceptionState);
}

void MIDIOutput::send(Vector<unsigned> unsignedData, ExceptionState& exceptionState)
{
    send(unsignedData, 0.0, exceptionState);
}

DEFINE_TRACE(MIDIOutput)
{
    MIDIPort::trace(visitor);
}

} // namespace blink
