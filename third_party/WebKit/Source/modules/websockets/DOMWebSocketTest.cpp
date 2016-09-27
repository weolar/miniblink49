// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/websockets/DOMWebSocket.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8BindingForTesting.h"
#include "core/dom/DOMTypedArray.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/SecurityContext.h"
#include "core/fileapi/Blob.h"
#include "core/frame/ConsoleTypes.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"
#include "wtf/testing/WTFTestHelpers.h"
#include "wtf/text/CString.h"
#include "wtf/text/WTFString.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <v8.h>

using testing::_;
using testing::AnyNumber;
using testing::InSequence;
using testing::Ref;
using testing::Return;

namespace blink {

namespace {

typedef testing::StrictMock<testing::MockFunction<void(int)>> Checkpoint;  // NOLINT

class MockWebSocketChannel : public WebSocketChannel {
public:
    static MockWebSocketChannel* create()
    {
        return new testing::StrictMock<MockWebSocketChannel>();
    }

    ~MockWebSocketChannel() override
    {
    }

    MOCK_METHOD2(connect, bool(const KURL&, const String&));
    MOCK_METHOD1(send, void(const CString&));
    MOCK_METHOD3(send, void(const DOMArrayBuffer&, unsigned, unsigned));
    MOCK_METHOD1(send, void(PassRefPtr<BlobDataHandle>));
    MOCK_METHOD1(sendTextAsCharVector, void(PassOwnPtr<Vector<char>>));
    MOCK_METHOD1(sendBinaryAsCharVector, void(PassOwnPtr<Vector<char>>));
    MOCK_CONST_METHOD0(bufferedAmount, unsigned());
    MOCK_METHOD2(close, void(int, const String&));
    MOCK_METHOD4(fail, void(const String&, MessageLevel, const String&, unsigned));
    MOCK_METHOD0(disconnect, void());

    MockWebSocketChannel()
    {
    }
};

class DOMWebSocketWithMockChannel final : public DOMWebSocket {
public:
    static DOMWebSocketWithMockChannel* create(ExecutionContext* context)
    {
        DOMWebSocketWithMockChannel* websocket = new DOMWebSocketWithMockChannel(context);
        websocket->suspendIfNeeded();
        return websocket;
    }

    MockWebSocketChannel* channel() { return m_channel.get(); }

    WebSocketChannel* createChannel(ExecutionContext*, WebSocketChannelClient*) override
    {
        ASSERT(!m_hasCreatedChannel);
        m_hasCreatedChannel = true;
        return m_channel.get();
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_channel);
        DOMWebSocket::trace(visitor);
    }

private:
    DOMWebSocketWithMockChannel(ExecutionContext* context)
        : DOMWebSocket(context)
        , m_channel(MockWebSocketChannel::create())
        , m_hasCreatedChannel(false) { }

    Member<MockWebSocketChannel> m_channel;
    bool m_hasCreatedChannel;
};

class DOMWebSocketTestBase {
public:
    DOMWebSocketTestBase()
        : m_pageHolder(DummyPageHolder::create())
        , m_websocket(DOMWebSocketWithMockChannel::create(&m_pageHolder->document()))
        , m_executionScope(v8::Isolate::GetCurrent())
        , m_exceptionState(ExceptionState::ConstructionContext, "property", "interface", m_executionScope.scriptState()->context()->Global(), m_executionScope.isolate())
    {
    }

    virtual ~DOMWebSocketTestBase()
    {
        if (!m_websocket)
            return;
        // These statements are needed to clear WebSocket::m_channel to
        // avoid ASSERTION failure on ~DOMWebSocket.
        ASSERT(m_websocket->channel());
        ::testing::Mock::VerifyAndClear(m_websocket->channel());
        EXPECT_CALL(channel(), disconnect()).Times(AnyNumber());

        m_websocket->didClose(WebSocketChannelClient::ClosingHandshakeIncomplete, 1006, "");
    }

    MockWebSocketChannel& channel() { return *m_websocket->channel(); }

    OwnPtr<DummyPageHolder> m_pageHolder;
    Persistent<DOMWebSocketWithMockChannel> m_websocket;
    V8TestingScope m_executionScope;
    ExceptionState m_exceptionState;
};

class DOMWebSocketTest : public DOMWebSocketTestBase, public ::testing::Test {
public:
};

TEST_F(DOMWebSocketTest, connectToBadURL)
{
    m_websocket->connect("xxx", Vector<String>(), m_exceptionState);


    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(SyntaxError, m_exceptionState.code());
    EXPECT_EQ("The URL 'xxx' is invalid.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, connectToNonWsURL)
{
    m_websocket->connect("http://example.com/", Vector<String>(), m_exceptionState);


    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(SyntaxError, m_exceptionState.code());
    EXPECT_EQ("The URL's scheme must be either 'ws' or 'wss'. 'http' is not allowed.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, connectToURLHavingFragmentIdentifier)
{
    m_websocket->connect("ws://example.com/#fragment", Vector<String>(), m_exceptionState);


    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(SyntaxError, m_exceptionState.code());
    EXPECT_EQ("The URL contains a fragment identifier ('fragment'). Fragment identifiers are not allowed in WebSocket URLs.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, invalidPort)
{
    m_websocket->connect("ws://example.com:7", Vector<String>(), m_exceptionState);


    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(SecurityError, m_exceptionState.code());
    EXPECT_EQ("The port 7 is not allowed.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

// FIXME: Add a test for Content Security Policy.

TEST_F(DOMWebSocketTest, invalidSubprotocols)
{
    Vector<String> subprotocols;
    subprotocols.append("@subprotocol-|'\"x\x01\x02\x03x");

    m_websocket->connect("ws://example.com/", subprotocols, m_exceptionState);

    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(SyntaxError, m_exceptionState.code());
    EXPECT_EQ("The subprotocol '@subprotocol-|'\"x\\u0001\\u0002\\u0003x' is invalid.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, insecureRequestsUpgrade)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "wss://example.com/endpoint"), String())).WillOnce(Return(true));
    }

    m_pageHolder->document().setInsecureRequestsPolicy(SecurityContext::InsecureRequestsUpgrade);
    m_websocket->connect("ws://example.com/endpoint", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());
    EXPECT_EQ(KURL(KURL(), "wss://example.com/endpoint"), m_websocket->url());
}

TEST_F(DOMWebSocketTest, insecureRequestsDoNotUpgrade)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/endpoint"), String())).WillOnce(Return(true));
    }

    m_pageHolder->document().setInsecureRequestsPolicy(SecurityContext::InsecureRequestsDoNotUpgrade);
    m_websocket->connect("ws://example.com/endpoint", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());
    EXPECT_EQ(KURL(KURL(), "ws://example.com/endpoint"), m_websocket->url());
}

TEST_F(DOMWebSocketTest, channelConnectSuccess)
{
    Vector<String> subprotocols;
    subprotocols.append("aa");
    subprotocols.append("bb");

    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/hoge"), String("aa, bb"))).WillOnce(Return(true));
    }

    m_websocket->connect("ws://example.com/hoge", Vector<String>(subprotocols), m_exceptionState);


    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());
    EXPECT_EQ(KURL(KURL(), "ws://example.com/hoge"), m_websocket->url());
}

TEST_F(DOMWebSocketTest, channelConnectFail)
{
    Vector<String> subprotocols;
    subprotocols.append("aa");
    subprotocols.append("bb");

    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String("aa, bb"))).WillOnce(Return(false));
        EXPECT_CALL(channel(), disconnect());
    }

    m_websocket->connect("ws://example.com/", Vector<String>(subprotocols), m_exceptionState);


    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(SecurityError, m_exceptionState.code());
    EXPECT_EQ("An insecure WebSocket connection may not be initiated from a page loaded over HTTPS.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, isValidSubprotocolString)
{
    EXPECT_TRUE(DOMWebSocket::isValidSubprotocolString("Helloworld!!"));
    EXPECT_FALSE(DOMWebSocket::isValidSubprotocolString("Hello, world!!"));
    EXPECT_FALSE(DOMWebSocket::isValidSubprotocolString(String()));
    EXPECT_FALSE(DOMWebSocket::isValidSubprotocolString(""));

    const char validCharacters[] = "!#$%&'*+-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ^_`abcdefghijklmnopqrstuvwxyz|~";
    size_t length = strlen(validCharacters);
    for (size_t i = 0; i < length; ++i) {
        String s;
        s.append(static_cast<UChar>(validCharacters[i]));
        EXPECT_TRUE(DOMWebSocket::isValidSubprotocolString(s));
    }
    for (size_t i = 0; i < 256; ++i) {
        if (std::find(validCharacters, validCharacters + length, static_cast<char>(i)) != validCharacters + length) {
            continue;
        }
        String s;
        s.append(static_cast<UChar>(i));
        EXPECT_FALSE(DOMWebSocket::isValidSubprotocolString(s));
    }
}

TEST_F(DOMWebSocketTest, connectSuccess)
{
    Vector<String> subprotocols;
    subprotocols.append("aa");
    subprotocols.append("bb");
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String("aa, bb"))).WillOnce(Return(true));
    }
    m_websocket->connect("ws://example.com/", subprotocols, m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->didConnect("bb", "cc");

    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
    EXPECT_EQ("bb", m_websocket->protocol());
    EXPECT_EQ("cc", m_websocket->extensions());
}

TEST_F(DOMWebSocketTest, didClose)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), disconnect());
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->didClose(WebSocketChannelClient::ClosingHandshakeIncomplete, 1006, "");

    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, maximumReasonSize)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), fail(_, _, _, _));
    }
    String reason;
    for (size_t i = 0; i < 123; ++i)
        reason.append("a");
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->close(1000, reason, m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, reasonSizeExceeding)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
    }
    String reason;
    for (size_t i = 0; i < 124; ++i)
        reason.append("a");
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->close(1000, reason, m_exceptionState);

    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(SyntaxError, m_exceptionState.code());
    EXPECT_EQ("The message must not be greater than 123 bytes.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, closeWhenConnecting)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), fail(String("WebSocket is closed before the connection is established."), WarningMessageLevel, String(), 0));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->close(1000, "bye", m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, close)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), close(3005, String("bye")));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->didConnect("", "");
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
    m_websocket->close(3005, "bye", m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, closeWithoutReason)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), close(3005, String()));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->didConnect("", "");
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
    m_websocket->close(3005, m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, closeWithoutCodeAndReason)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), close(-1, String()));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->didConnect("", "");
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
    m_websocket->close(m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, closeWhenClosing)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), close(-1, String()));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->didConnect("", "");
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
    m_websocket->close(m_exceptionState);
    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());

    m_websocket->close(m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, closeWhenClosed)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), close(-1, String()));
        EXPECT_CALL(channel(), disconnect());
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->didConnect("", "");
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
    m_websocket->close(m_exceptionState);
    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());

    m_websocket->didClose(WebSocketChannelClient::ClosingHandshakeComplete, 1000, String());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
    m_websocket->close(m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendStringWhenConnecting)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->send("hello", m_exceptionState);

    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(InvalidStateError, m_exceptionState.code());
    EXPECT_EQ("Still in CONNECTING state.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendStringWhenClosing)
{
    Checkpoint checkpoint;
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), fail(_, _, _, _));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->close(m_exceptionState);
    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->send("hello", m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendStringWhenClosed)
{
    Checkpoint checkpoint;
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), disconnect());
        EXPECT_CALL(checkpoint, Call(1));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->didClose(WebSocketChannelClient::ClosingHandshakeIncomplete, 1006, "");
    checkpoint.Call(1);

    m_websocket->send("hello", m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendStringSuccess)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), send(CString("hello")));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->didConnect("", "");
    m_websocket->send("hello", m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendNonLatin1String)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), send(CString("\xe7\x8b\x90\xe0\xa4\x94")));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->didConnect("", "");
    UChar nonLatin1String[] = {
        0x72d0,
        0x0914,
        0x0000
    };
    m_websocket->send(nonLatin1String, m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendArrayBufferWhenConnecting)
{
    RefPtr<DOMArrayBufferView> view = DOMUint8Array::create(8);
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->send(view->buffer().get(), m_exceptionState);

    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(InvalidStateError, m_exceptionState.code());
    EXPECT_EQ("Still in CONNECTING state.", m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendArrayBufferWhenClosing)
{
    RefPtr<DOMArrayBufferView> view = DOMUint8Array::create(8);
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), fail(_, _, _, _));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->close(m_exceptionState);
    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->send(view->buffer().get(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendArrayBufferWhenClosed)
{
    Checkpoint checkpoint;
    RefPtr<DOMArrayBufferView> view = DOMUint8Array::create(8);
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), disconnect());
        EXPECT_CALL(checkpoint, Call(1));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->didClose(WebSocketChannelClient::ClosingHandshakeIncomplete, 1006, "");
    checkpoint.Call(1);

    m_websocket->send(view->buffer().get(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSED, m_websocket->readyState());
}

TEST_F(DOMWebSocketTest, sendArrayBufferSuccess)
{
    RefPtr<DOMArrayBufferView> view = DOMUint8Array::create(8);
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), send(Ref(*view->buffer()), 0, 8));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());

    m_websocket->didConnect("", "");
    m_websocket->send(view->buffer().get(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::OPEN, m_websocket->readyState());
}

// FIXME: We should have Blob tests here.
// We can't create a Blob because the blob registration cannot be mocked yet.

// FIXME: We should add tests for bufferedAmount.

// FIXME: We should add tests for data receiving.

TEST_F(DOMWebSocketTest, binaryType)
{
    EXPECT_EQ("blob", m_websocket->binaryType());

    m_websocket->setBinaryType("arraybuffer");

    EXPECT_EQ("arraybuffer", m_websocket->binaryType());

    m_websocket->setBinaryType("blob");

    EXPECT_EQ("blob", m_websocket->binaryType());
}

// FIXME: We should add tests for suspend / resume.

class DOMWebSocketValidClosingTest : public DOMWebSocketTestBase, public ::testing::TestWithParam<unsigned short> {
public:
};

TEST_P(DOMWebSocketValidClosingTest, test)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
        EXPECT_CALL(channel(), fail(_, _, _, _));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->close(GetParam(), "bye", m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CLOSING, m_websocket->readyState());
}

INSTANTIATE_TEST_CASE_P(DOMWebSocketValidClosing, DOMWebSocketValidClosingTest, ::testing::Values(1000, 3000, 3001, 4998, 4999));

class DOMWebSocketInvalidClosingCodeTest : public DOMWebSocketTestBase, public ::testing::TestWithParam<unsigned short> {
public:
};

TEST_P(DOMWebSocketInvalidClosingCodeTest, test)
{
    {
        InSequence s;
        EXPECT_CALL(channel(), connect(KURL(KURL(), "ws://example.com/"), String())).WillOnce(Return(true));
    }
    m_websocket->connect("ws://example.com/", Vector<String>(), m_exceptionState);

    EXPECT_FALSE(m_exceptionState.hadException());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());

    m_websocket->close(GetParam(), "bye", m_exceptionState);

    EXPECT_TRUE(m_exceptionState.hadException());
    EXPECT_EQ(InvalidAccessError, m_exceptionState.code());
    EXPECT_EQ(String::format("The code must be either 1000, or between 3000 and 4999. %d is neither.", GetParam()), m_exceptionState.message());
    EXPECT_EQ(DOMWebSocket::CONNECTING, m_websocket->readyState());
}

INSTANTIATE_TEST_CASE_P(DOMWebSocketInvalidClosingCode, DOMWebSocketInvalidClosingCodeTest, ::testing::Values(0, 1, 998, 999, 1001, 2999, 5000, 9999, 65535));

} // namespace

} // namespace blink
