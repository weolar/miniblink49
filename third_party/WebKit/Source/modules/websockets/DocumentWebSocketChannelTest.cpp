// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/websockets/WebSocketChannel.h"

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/Document.h"
#include "core/fileapi/Blob.h"
#include "core/frame/ConsoleTypes.h"
#include "core/testing/DummyPageHolder.h"
#include "modules/websockets/DocumentWebSocketChannel.h"
#include "modules/websockets/WebSocketChannelClient.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/WebSerializedOrigin.h"
#include "public/platform/WebSocketHandle.h"
#include "public/platform/WebSocketHandleClient.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebVector.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"
#include <stdint.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::InSequence;
using testing::PrintToString;
using testing::AnyNumber;


namespace blink {

namespace {

typedef testing::StrictMock< testing::MockFunction<void(int)>> Checkpoint;

class MockWebSocketChannelClient : public GarbageCollectedFinalized<MockWebSocketChannelClient>, public WebSocketChannelClient {
    USING_GARBAGE_COLLECTED_MIXIN(MockWebSocketChannelClient);
public:
    static MockWebSocketChannelClient* create()
    {
        return new testing::StrictMock<MockWebSocketChannelClient>();
    }

    MockWebSocketChannelClient() { }

    ~MockWebSocketChannelClient() override { }

    MOCK_METHOD2(didConnect, void(const String&, const String&));
    MOCK_METHOD1(didReceiveTextMessage, void(const String&));
    void didReceiveBinaryMessage(PassOwnPtr<Vector<char>> payload) override
    {
        didReceiveBinaryMessageMock(*payload);
    }
    MOCK_METHOD1(didReceiveBinaryMessageMock, void(const Vector<char>&));
    MOCK_METHOD0(didError, void());
    MOCK_METHOD1(didConsumeBufferedAmount, void(uint64_t));
    MOCK_METHOD0(didStartClosingHandshake, void());
    MOCK_METHOD3(didClose, void(ClosingHandshakeCompletionStatus, unsigned short, const String&));

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        WebSocketChannelClient::trace(visitor);
    }

};

class MockWebSocketHandle : public WebSocketHandle {
public:
    static MockWebSocketHandle* create()
    {
        return new testing::StrictMock<MockWebSocketHandle>();
    }

    MockWebSocketHandle() { }

    ~MockWebSocketHandle() override { }

    MOCK_METHOD4(connect, void(const WebURL&, const WebVector<WebString>&, const WebSerializedOrigin&, WebSocketHandleClient*));
    MOCK_METHOD4(send, void(bool, WebSocketHandle::MessageType, const char*, size_t));
    MOCK_METHOD1(flowControl, void(int64_t));
    MOCK_METHOD2(close, void(unsigned short, const WebString&));
};

class DocumentWebSocketChannelTest : public ::testing::Test {
public:
    DocumentWebSocketChannelTest()
        : m_pageHolder(DummyPageHolder::create())
        , m_channelClient(MockWebSocketChannelClient::create())
        , m_handle(MockWebSocketHandle::create())
        , m_channel(DocumentWebSocketChannel::create(&m_pageHolder->document(), m_channelClient.get(), String(), 0, handle()))
        , m_sumOfConsumedBufferedAmount(0)
    {
        ON_CALL(*channelClient(), didConsumeBufferedAmount(_)).WillByDefault(Invoke(this, &DocumentWebSocketChannelTest::didConsumeBufferedAmount));
    }

    ~DocumentWebSocketChannelTest()
    {
        channel()->disconnect();
    }

    MockWebSocketChannelClient* channelClient()
    {
        return m_channelClient.get();
    }

    WebSocketChannel* channel()
    {
        return static_cast<WebSocketChannel*>(m_channel.get());
    }

    WebSocketHandleClient* handleClient()
    {
        return static_cast<WebSocketHandleClient*>(m_channel.get());
    }

    MockWebSocketHandle* handle()
    {
        return m_handle;
    }

    void didConsumeBufferedAmount(unsigned long a)
    {
        m_sumOfConsumedBufferedAmount += a;
    }

    void connect()
    {
        {
            InSequence s;
            EXPECT_CALL(*handle(), connect(WebURL(KURL(KURL(), "ws://localhost/")), _, _, handleClient()));
            EXPECT_CALL(*handle(), flowControl(65536));
            EXPECT_CALL(*channelClient(), didConnect(String("a"), String("b")));
        }
        EXPECT_TRUE(channel()->connect(KURL(KURL(), "ws://localhost/"), "x"));
        handleClient()->didConnect(handle(), WebString("a"), WebString("b"));
        ::testing::Mock::VerifyAndClearExpectations(this);
    }

    OwnPtr<DummyPageHolder> m_pageHolder;
    Persistent<MockWebSocketChannelClient> m_channelClient;
    MockWebSocketHandle* m_handle;
    Persistent<DocumentWebSocketChannel> m_channel;
    unsigned long m_sumOfConsumedBufferedAmount;
};

MATCHER_P2(MemEq, p, len,
    std::string("pointing to memory")
    + (negation ? " not" : "")
    + " equal to \""
    + std::string(p, len) + "\" (length=" + PrintToString(len) + ")"
)
{
    return memcmp(arg, p, len) == 0;
}

TEST_F(DocumentWebSocketChannelTest, connectSuccess)
{
    Checkpoint checkpoint;
    {
        InSequence s;
        EXPECT_CALL(*handle(), connect(WebURL(KURL(KURL(), "ws://localhost/")), _, _, handleClient()));
        EXPECT_CALL(*handle(), flowControl(65536));
        EXPECT_CALL(checkpoint, Call(1));
        EXPECT_CALL(*channelClient(), didConnect(String("a"), String("b")));
    }

    EXPECT_TRUE(channel()->connect(KURL(KURL(), "ws://localhost/"), "x"));
    checkpoint.Call(1);
    handleClient()->didConnect(handle(), WebString("a"), WebString("b"));
}

TEST_F(DocumentWebSocketChannelTest, sendText)
{
    connect();
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeText, MemEq("foo", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeText, MemEq("bar", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeText, MemEq("baz", 3), 3));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    channel()->send("foo");
    channel()->send("bar");
    channel()->send("baz");

    EXPECT_EQ(9ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendTextContinuation)
{
    connect();
    Checkpoint checkpoint;
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(false, WebSocketHandle::MessageTypeText, MemEq("0123456789abcdef", 16), 16));
        EXPECT_CALL(checkpoint, Call(1));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeContinuation, MemEq("g", 1), 1));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeText, MemEq("hijk", 4), 4));
        EXPECT_CALL(*handle(), send(false, WebSocketHandle::MessageTypeText, MemEq("lmnopqrstuv", 11), 11));
        EXPECT_CALL(checkpoint, Call(2));
        EXPECT_CALL(*handle(), send(false, WebSocketHandle::MessageTypeContinuation, MemEq("wxyzABCDEFGHIJKL", 16), 16));
        EXPECT_CALL(checkpoint, Call(3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeContinuation, MemEq("MNOPQRSTUVWXYZ", 14), 14));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    channel()->send("0123456789abcdefg");
    channel()->send("hijk");
    channel()->send("lmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    checkpoint.Call(1);
    handleClient()->didReceiveFlowControl(handle(), 16);
    checkpoint.Call(2);
    handleClient()->didReceiveFlowControl(handle(), 16);
    checkpoint.Call(3);
    handleClient()->didReceiveFlowControl(handle(), 16);

    EXPECT_EQ(62ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInVector)
{
    connect();
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("foo", 3), 3));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    Vector<char> fooVector;
    fooVector.append("foo", 3);
    channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(fooVector)));

    EXPECT_EQ(3ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInVectorWithNullBytes)
{
    connect();
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\0ar", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("b\0z", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("qu\0", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\0\0\0", 3), 3));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    {
        Vector<char> v;
        v.append("\0ar", 3);
        channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(v)));
    }
    {
        Vector<char> v;
        v.append("b\0z", 3);
        channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(v)));
    }
    {
        Vector<char> v;
        v.append("qu\0", 3);
        channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(v)));
    }
    {
        Vector<char> v;
        v.append("\0\0\0", 3);
        channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(v)));
    }

    EXPECT_EQ(12ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInVectorNonLatin1UTF8)
{
    connect();
    EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\xe7\x8b\x90", 3), 3));

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    Vector<char> v;
    v.append("\xe7\x8b\x90", 3);
    channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(v)));

    EXPECT_EQ(3ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInVectorNonUTF8)
{
    connect();
    EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\x80\xff\xe7", 3), 3));

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    Vector<char> v;
    v.append("\x80\xff\xe7", 3);
    channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(v)));

    EXPECT_EQ(3ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInVectorNonLatin1UTF8Continuation)
{
    connect();
    Checkpoint checkpoint;
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(false, WebSocketHandle::MessageTypeBinary, MemEq("\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7", 16), 16));
        EXPECT_CALL(checkpoint, Call(1));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeContinuation, MemEq("\x8b\x90", 2), 2));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    Vector<char> v;
    v.append("\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90", 18);
    channel()->sendBinaryAsCharVector(adoptPtr(new Vector<char>(v)));
    checkpoint.Call(1);

    handleClient()->didReceiveFlowControl(handle(), 16);

    EXPECT_EQ(18ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInArrayBuffer)
{
    connect();
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("foo", 3), 3));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    RefPtr<DOMArrayBuffer> fooBuffer = DOMArrayBuffer::create("foo", 3);
    channel()->send(*fooBuffer, 0, 3);

    EXPECT_EQ(3ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInArrayBufferPartial)
{
    connect();
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("foo", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("bar", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("baz", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("a", 1), 1));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    RefPtr<DOMArrayBuffer> foobarBuffer = DOMArrayBuffer::create("foobar", 6);
    RefPtr<DOMArrayBuffer> qbazuxBuffer = DOMArrayBuffer::create("qbazux", 6);
    channel()->send(*foobarBuffer, 0, 3);
    channel()->send(*foobarBuffer, 3, 3);
    channel()->send(*qbazuxBuffer, 1, 3);
    channel()->send(*qbazuxBuffer, 2, 1);

    EXPECT_EQ(10ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInArrayBufferWithNullBytes)
{
    connect();
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\0ar", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("b\0z", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("qu\0", 3), 3));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\0\0\0", 3), 3));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    {
        RefPtr<DOMArrayBuffer> b = DOMArrayBuffer::create("\0ar", 3);
        channel()->send(*b, 0, 3);
    }
    {
        RefPtr<DOMArrayBuffer> b = DOMArrayBuffer::create("b\0z", 3);
        channel()->send(*b, 0, 3);
    }
    {
        RefPtr<DOMArrayBuffer> b = DOMArrayBuffer::create("qu\0", 3);
        channel()->send(*b, 0, 3);
    }
    {
        RefPtr<DOMArrayBuffer> b = DOMArrayBuffer::create("\0\0\0", 3);
        channel()->send(*b, 0, 3);
    }

    EXPECT_EQ(12ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInArrayBufferNonLatin1UTF8)
{
    connect();
    EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\xe7\x8b\x90", 3), 3));

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    RefPtr<DOMArrayBuffer> b = DOMArrayBuffer::create("\xe7\x8b\x90", 3);
    channel()->send(*b, 0, 3);

    EXPECT_EQ(3ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInArrayBufferNonUTF8)
{
    connect();
    EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeBinary, MemEq("\x80\xff\xe7", 3), 3));

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    RefPtr<DOMArrayBuffer> b = DOMArrayBuffer::create("\x80\xff\xe7", 3);
    channel()->send(*b, 0, 3);

    EXPECT_EQ(3ul, m_sumOfConsumedBufferedAmount);
}

TEST_F(DocumentWebSocketChannelTest, sendBinaryInArrayBufferNonLatin1UTF8Continuation)
{
    connect();
    Checkpoint checkpoint;
    {
        InSequence s;
        EXPECT_CALL(*handle(), send(false, WebSocketHandle::MessageTypeBinary, MemEq("\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7", 16), 16));
        EXPECT_CALL(checkpoint, Call(1));
        EXPECT_CALL(*handle(), send(true, WebSocketHandle::MessageTypeContinuation, MemEq("\x8b\x90", 2), 2));
    }

    handleClient()->didReceiveFlowControl(handle(), 16);
    EXPECT_CALL(*channelClient(), didConsumeBufferedAmount(_)).Times(AnyNumber());

    RefPtr<DOMArrayBuffer> b = DOMArrayBuffer::create("\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90\xe7\x8b\x90", 18);
    channel()->send(*b, 0, 18);
    checkpoint.Call(1);

    handleClient()->didReceiveFlowControl(handle(), 16);

    EXPECT_EQ(18ul, m_sumOfConsumedBufferedAmount);
}

// FIXME: Add tests for WebSocketChannel::send(PassRefPtr<BlobDataHandle>)

TEST_F(DocumentWebSocketChannelTest, receiveText)
{
    connect();
    {
        InSequence s;
        EXPECT_CALL(*channelClient(), didReceiveTextMessage(String("FOO")));
        EXPECT_CALL(*channelClient(), didReceiveTextMessage(String("BAR")));
    }

    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeText, "FOOX", 3);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeText, "BARX", 3);
}

TEST_F(DocumentWebSocketChannelTest, receiveTextContinuation)
{
    connect();
    EXPECT_CALL(*channelClient(), didReceiveTextMessage(String("BAZ")));

    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeText, "BX", 1);
    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeContinuation, "AX", 1);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeContinuation, "ZX", 1);
}

TEST_F(DocumentWebSocketChannelTest, receiveTextNonLatin1)
{
    connect();
    UChar nonLatin1String[] = {
        0x72d0,
        0x0914,
        0x0000
    };
    EXPECT_CALL(*channelClient(), didReceiveTextMessage(String(nonLatin1String)));

    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeText, "\xe7\x8b\x90\xe0\xa4\x94", 6);
}

TEST_F(DocumentWebSocketChannelTest, receiveTextNonLatin1Continuation)
{
    connect();
    UChar nonLatin1String[] = {
        0x72d0,
        0x0914,
        0x0000
    };
    EXPECT_CALL(*channelClient(), didReceiveTextMessage(String(nonLatin1String)));

    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeText, "\xe7\x8b", 2);
    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeContinuation, "\x90\xe0", 2);
    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeContinuation, "\xa4", 1);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeContinuation, "\x94", 1);
}

TEST_F(DocumentWebSocketChannelTest, receiveBinary)
{
    connect();
    Vector<char> fooVector;
    fooVector.append("FOO", 3);
    EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(fooVector));

    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeBinary, "FOOx", 3);
}

TEST_F(DocumentWebSocketChannelTest, receiveBinaryContinuation)
{
    connect();
    Vector<char> bazVector;
    bazVector.append("BAZ", 3);
    EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(bazVector));

    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeBinary, "Bx", 1);
    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeContinuation, "Ax", 1);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeContinuation, "Zx", 1);
}

TEST_F(DocumentWebSocketChannelTest, receiveBinaryWithNullBytes)
{
    connect();
    {
        InSequence s;
        {
            Vector<char> v;
            v.append("\0AR", 3);
            EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(v));
        }
        {
            Vector<char> v;
            v.append("B\0Z", 3);
            EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(v));
        }
        {
            Vector<char> v;
            v.append("QU\0", 3);
            EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(v));
        }
        {
            Vector<char> v;
            v.append("\0\0\0", 3);
            EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(v));
        }
    }

    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeBinary, "\0AR", 3);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeBinary, "B\0Z", 3);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeBinary, "QU\0", 3);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeBinary, "\0\0\0", 3);
}

TEST_F(DocumentWebSocketChannelTest, receiveBinaryNonLatin1UTF8)
{
    connect();
    Vector<char> v;
    v.append("\xe7\x8b\x90\xe0\xa4\x94", 6);
    EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(v));

    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeBinary, "\xe7\x8b\x90\xe0\xa4\x94", 6);
}

TEST_F(DocumentWebSocketChannelTest, receiveBinaryNonLatin1UTF8Continuation)
{
    connect();
    Vector<char> v;
    v.append("\xe7\x8b\x90\xe0\xa4\x94", 6);
    EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(v));

    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeBinary, "\xe7\x8b", 2);
    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeContinuation, "\x90\xe0", 2);
    handleClient()->didReceiveData(handle(), false, WebSocketHandle::MessageTypeContinuation, "\xa4", 1);
    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeContinuation, "\x94", 1);
}

TEST_F(DocumentWebSocketChannelTest, receiveBinaryNonUTF8)
{
    connect();
    Vector<char> v;
    v.append("\x80\xff", 2);
    EXPECT_CALL(*channelClient(), didReceiveBinaryMessageMock(v));

    handleClient()->didReceiveData(handle(), true, WebSocketHandle::MessageTypeBinary, "\x80\xff", 2);
}

TEST_F(DocumentWebSocketChannelTest, closeFromBrowser)
{
    connect();
    Checkpoint checkpoint;
    {
        InSequence s;

        EXPECT_CALL(*channelClient(), didStartClosingHandshake());
        EXPECT_CALL(checkpoint, Call(1));

        EXPECT_CALL(*handle(), close(WebSocketChannel::CloseEventCodeNormalClosure, WebString("close reason")));
        EXPECT_CALL(checkpoint, Call(2));

        EXPECT_CALL(*channelClient(), didClose(WebSocketChannelClient::ClosingHandshakeComplete, WebSocketChannel::CloseEventCodeNormalClosure, String("close reason")));
        EXPECT_CALL(checkpoint, Call(3));
    }

    handleClient()->didStartClosingHandshake(handle());
    checkpoint.Call(1);

    channel()->close(WebSocketChannel::CloseEventCodeNormalClosure, String("close reason"));
    checkpoint.Call(2);

    handleClient()->didClose(handle(), true, WebSocketChannel::CloseEventCodeNormalClosure, String("close reason"));
    checkpoint.Call(3);

    channel()->disconnect();
}

TEST_F(DocumentWebSocketChannelTest, closeFromWebSocket)
{
    connect();
    Checkpoint checkpoint;
    {
        InSequence s;

        EXPECT_CALL(*handle(), close(WebSocketChannel::CloseEventCodeNormalClosure, WebString("close reason")));
        EXPECT_CALL(checkpoint, Call(1));

        EXPECT_CALL(*channelClient(), didClose(WebSocketChannelClient::ClosingHandshakeComplete, WebSocketChannel::CloseEventCodeNormalClosure, String("close reason")));
        EXPECT_CALL(checkpoint, Call(2));
    }

    channel()->close(WebSocketChannel::CloseEventCodeNormalClosure, String("close reason"));
    checkpoint.Call(1);

    handleClient()->didClose(handle(), true, WebSocketChannel::CloseEventCodeNormalClosure, String("close reason"));
    checkpoint.Call(2);

    channel()->disconnect();
}

TEST_F(DocumentWebSocketChannelTest, failFromBrowser)
{
    connect();
    {
        InSequence s;

        EXPECT_CALL(*channelClient(), didError());
        EXPECT_CALL(*channelClient(), didClose(WebSocketChannelClient::ClosingHandshakeIncomplete, WebSocketChannel::CloseEventCodeAbnormalClosure, String()));
    }

    handleClient()->didFail(handle(), "fail message");
}

TEST_F(DocumentWebSocketChannelTest, failFromWebSocket)
{
    connect();
    {
        InSequence s;

        EXPECT_CALL(*channelClient(), didError());
        EXPECT_CALL(*channelClient(), didClose(WebSocketChannelClient::ClosingHandshakeIncomplete, WebSocketChannel::CloseEventCodeAbnormalClosure, String()));
    }

    channel()->fail("fail message from WebSocket", ErrorMessageLevel, "sourceURL", 1234);
}

} // namespace

} // namespace blink
