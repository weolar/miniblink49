// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef protocol_Profiler_h
#define protocol_Profiler_h

#include "platform/inspector_protocol/Platform.h"
#include "platform/inspector_protocol/Array.h"
#include "platform/inspector_protocol/BackendCallback.h"
#include "platform/inspector_protocol/DispatcherBase.h"
#include "platform/inspector_protocol/ErrorSupport.h"
#include "platform/inspector_protocol/FrontendChannel.h"
#include "platform/inspector_protocol/Maybe.h"
#include "platform/inspector_protocol/Object.h"
#include "platform/inspector_protocol/Platform.h"
#include "platform/inspector_protocol/String16.h"
#include "platform/inspector_protocol/Values.h"
#include "platform/inspector_protocol/ValueConversions.h"
// For each imported domain we generate a ValueConversions struct instead of a full domain definition
// and include Domain::API version from there.
#include "platform\v8_inspector\protocol/Runtime.h"
#include "platform\v8_inspector\protocol/Debugger.h"

namespace blink {
namespace protocol {
namespace Profiler {

// ------------- Forward and enum declarations.
// CPU Profile node. Holds callsite information, execution statistics and child nodes.
class CPUProfileNode;
// Profile.
class CPUProfile;
// Specifies a number of samples attributed to a certain source position.
class PositionTickInfo;

// ------------- Type and builder declarations.

// CPU Profile node. Holds callsite information, execution statistics and child nodes.
class PLATFORM_EXPORT CPUProfileNode {
    PROTOCOL_DISALLOW_COPY(CPUProfileNode);
public:
    static std::unique_ptr<CPUProfileNode> parse(protocol::Value* value, ErrorSupport* errors);

    ~CPUProfileNode() { }

    protocol::Runtime::CallFrame* getCallFrame() { return m_callFrame.get(); }
    void setCallFrame(std::unique_ptr<protocol::Runtime::CallFrame> value) { m_callFrame = std::move(value); }

    int getHitCount() { return m_hitCount; }
    void setHitCount(int value) { m_hitCount = value; }

    protocol::Array<protocol::Profiler::CPUProfileNode>* getChildren() { return m_children.get(); }
    void setChildren(std::unique_ptr<protocol::Array<protocol::Profiler::CPUProfileNode>> value) { m_children = std::move(value); }

    String16 getDeoptReason() { return m_deoptReason; }
    void setDeoptReason(const String16& value) { m_deoptReason = value; }

    int getId() { return m_id; }
    void setId(int value) { m_id = value; }

    protocol::Array<protocol::Profiler::PositionTickInfo>* getPositionTicks() { return m_positionTicks.get(); }
    void setPositionTicks(std::unique_ptr<protocol::Array<protocol::Profiler::PositionTickInfo>> value) { m_positionTicks = std::move(value); }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<CPUProfileNode> clone() const;

    template<int STATE>
    class CPUProfileNodeBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          CallFrameSet = 1 << 1,
          HitCountSet = 1 << 2,
          ChildrenSet = 1 << 3,
          DeoptReasonSet = 1 << 4,
          IdSet = 1 << 5,
          PositionTicksSet = 1 << 6,
            AllFieldsSet = (CallFrameSet | HitCountSet | ChildrenSet | DeoptReasonSet | IdSet | PositionTicksSet | 0)};


        CPUProfileNodeBuilder<STATE | CallFrameSet>& setCallFrame(std::unique_ptr<protocol::Runtime::CallFrame> value)
        {
            static_assert(!(STATE & CallFrameSet), "property callFrame should not be set yet");
            m_result->setCallFrame(std::move(value));
            return castState<CallFrameSet>();
        }

        CPUProfileNodeBuilder<STATE | HitCountSet>& setHitCount(int value)
        {
            static_assert(!(STATE & HitCountSet), "property hitCount should not be set yet");
            m_result->setHitCount(value);
            return castState<HitCountSet>();
        }

        CPUProfileNodeBuilder<STATE | ChildrenSet>& setChildren(std::unique_ptr<protocol::Array<protocol::Profiler::CPUProfileNode>> value)
        {
            static_assert(!(STATE & ChildrenSet), "property children should not be set yet");
            m_result->setChildren(std::move(value));
            return castState<ChildrenSet>();
        }

        CPUProfileNodeBuilder<STATE | DeoptReasonSet>& setDeoptReason(const String16& value)
        {
            static_assert(!(STATE & DeoptReasonSet), "property deoptReason should not be set yet");
            m_result->setDeoptReason(value);
            return castState<DeoptReasonSet>();
        }

        CPUProfileNodeBuilder<STATE | IdSet>& setId(int value)
        {
            static_assert(!(STATE & IdSet), "property id should not be set yet");
            m_result->setId(value);
            return castState<IdSet>();
        }

        CPUProfileNodeBuilder<STATE | PositionTicksSet>& setPositionTicks(std::unique_ptr<protocol::Array<protocol::Profiler::PositionTickInfo>> value)
        {
            static_assert(!(STATE & PositionTicksSet), "property positionTicks should not be set yet");
            m_result->setPositionTicks(std::move(value));
            return castState<PositionTicksSet>();
        }

        std::unique_ptr<CPUProfileNode> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class CPUProfileNode;
        CPUProfileNodeBuilder() : m_result(new CPUProfileNode()) { }

        template<int STEP> CPUProfileNodeBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<CPUProfileNodeBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Profiler::CPUProfileNode> m_result;
    };

    static CPUProfileNodeBuilder<0> create()
    {
        return CPUProfileNodeBuilder<0>();
    }

private:
    CPUProfileNode()
    {
          m_hitCount = 0;
          m_id = 0;
    }

    std::unique_ptr<protocol::Runtime::CallFrame> m_callFrame;
    int m_hitCount;
    std::unique_ptr<protocol::Array<protocol::Profiler::CPUProfileNode>> m_children;
    String16 m_deoptReason;
    int m_id;
    std::unique_ptr<protocol::Array<protocol::Profiler::PositionTickInfo>> m_positionTicks;
};


// Profile.
class PLATFORM_EXPORT CPUProfile {
    PROTOCOL_DISALLOW_COPY(CPUProfile);
public:
    static std::unique_ptr<CPUProfile> parse(protocol::Value* value, ErrorSupport* errors);

    ~CPUProfile() { }

    protocol::Profiler::CPUProfileNode* getHead() { return m_head.get(); }
    void setHead(std::unique_ptr<protocol::Profiler::CPUProfileNode> value) { m_head = std::move(value); }

    double getStartTime() { return m_startTime; }
    void setStartTime(double value) { m_startTime = value; }

    double getEndTime() { return m_endTime; }
    void setEndTime(double value) { m_endTime = value; }

    bool hasSamples() { return m_samples.isJust(); }
    protocol::Array<int>* getSamples(protocol::Array<int>* defaultValue) { return m_samples.isJust() ? m_samples.fromJust() : defaultValue; }
    void setSamples(std::unique_ptr<protocol::Array<int>> value) { m_samples = std::move(value); }

    bool hasTimestamps() { return m_timestamps.isJust(); }
    protocol::Array<double>* getTimestamps(protocol::Array<double>* defaultValue) { return m_timestamps.isJust() ? m_timestamps.fromJust() : defaultValue; }
    void setTimestamps(std::unique_ptr<protocol::Array<double>> value) { m_timestamps = std::move(value); }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<CPUProfile> clone() const;

    template<int STATE>
    class CPUProfileBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          HeadSet = 1 << 1,
          StartTimeSet = 1 << 2,
          EndTimeSet = 1 << 3,
            AllFieldsSet = (HeadSet | StartTimeSet | EndTimeSet | 0)};


        CPUProfileBuilder<STATE | HeadSet>& setHead(std::unique_ptr<protocol::Profiler::CPUProfileNode> value)
        {
            static_assert(!(STATE & HeadSet), "property head should not be set yet");
            m_result->setHead(std::move(value));
            return castState<HeadSet>();
        }

        CPUProfileBuilder<STATE | StartTimeSet>& setStartTime(double value)
        {
            static_assert(!(STATE & StartTimeSet), "property startTime should not be set yet");
            m_result->setStartTime(value);
            return castState<StartTimeSet>();
        }

        CPUProfileBuilder<STATE | EndTimeSet>& setEndTime(double value)
        {
            static_assert(!(STATE & EndTimeSet), "property endTime should not be set yet");
            m_result->setEndTime(value);
            return castState<EndTimeSet>();
        }

        CPUProfileBuilder<STATE>& setSamples(std::unique_ptr<protocol::Array<int>> value)
        {
            m_result->setSamples(std::move(value));
            return *this;
        }

        CPUProfileBuilder<STATE>& setTimestamps(std::unique_ptr<protocol::Array<double>> value)
        {
            m_result->setTimestamps(std::move(value));
            return *this;
        }

        std::unique_ptr<CPUProfile> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class CPUProfile;
        CPUProfileBuilder() : m_result(new CPUProfile()) { }

        template<int STEP> CPUProfileBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<CPUProfileBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Profiler::CPUProfile> m_result;
    };

    static CPUProfileBuilder<0> create()
    {
        return CPUProfileBuilder<0>();
    }

private:
    CPUProfile()
    {
          m_startTime = 0;
          m_endTime = 0;
    }

    std::unique_ptr<protocol::Profiler::CPUProfileNode> m_head;
    double m_startTime;
    double m_endTime;
    Maybe<protocol::Array<int>> m_samples;
    Maybe<protocol::Array<double>> m_timestamps;
};


// Specifies a number of samples attributed to a certain source position.
class PLATFORM_EXPORT PositionTickInfo {
    PROTOCOL_DISALLOW_COPY(PositionTickInfo);
public:
    static std::unique_ptr<PositionTickInfo> parse(protocol::Value* value, ErrorSupport* errors);

    ~PositionTickInfo() { }

    int getLine() { return m_line; }
    void setLine(int value) { m_line = value; }

    int getTicks() { return m_ticks; }
    void setTicks(int value) { m_ticks = value; }

    std::unique_ptr<protocol::DictionaryValue> serialize() const;
    std::unique_ptr<PositionTickInfo> clone() const;

    template<int STATE>
    class PositionTickInfoBuilder {
    public:
        enum {
            NoFieldsSet = 0,
          LineSet = 1 << 1,
          TicksSet = 1 << 2,
            AllFieldsSet = (LineSet | TicksSet | 0)};


        PositionTickInfoBuilder<STATE | LineSet>& setLine(int value)
        {
            static_assert(!(STATE & LineSet), "property line should not be set yet");
            m_result->setLine(value);
            return castState<LineSet>();
        }

        PositionTickInfoBuilder<STATE | TicksSet>& setTicks(int value)
        {
            static_assert(!(STATE & TicksSet), "property ticks should not be set yet");
            m_result->setTicks(value);
            return castState<TicksSet>();
        }

        std::unique_ptr<PositionTickInfo> build()
        {
            static_assert(STATE == AllFieldsSet, "state should be AllFieldsSet");
            return std::move(m_result);
        }

    private:
        friend class PositionTickInfo;
        PositionTickInfoBuilder() : m_result(new PositionTickInfo()) { }

        template<int STEP> PositionTickInfoBuilder<STATE | STEP>& castState()
        {
            return *reinterpret_cast<PositionTickInfoBuilder<STATE | STEP>*>(this);
        }

        std::unique_ptr<protocol::Profiler::PositionTickInfo> m_result;
    };

    static PositionTickInfoBuilder<0> create()
    {
        return PositionTickInfoBuilder<0>();
    }

private:
    PositionTickInfo()
    {
          m_line = 0;
          m_ticks = 0;
    }

    int m_line;
    int m_ticks;
};


// ------------- Backend interface.

class PLATFORM_EXPORT Backend {
public:
    virtual void enable(ErrorString*) = 0;
    virtual void disable(ErrorString*) = 0;
    virtual void setSamplingInterval(ErrorString*, int in_interval) = 0;
    virtual void start(ErrorString*) = 0;
    virtual void stop(ErrorString*, std::unique_ptr<protocol::Profiler::CPUProfile>* out_profile) = 0;


protected:
    virtual ~Backend() { }
};

// ------------- Frontend interface.

class PLATFORM_EXPORT Frontend {
public:
    Frontend(FrontendChannel* frontendChannel) : m_frontendChannel(frontendChannel) { }
    void consoleProfileStarted(const String16& id, std::unique_ptr<protocol::Debugger::Location> location, const Maybe<String16>& title = Maybe<String16>());
    void consoleProfileFinished(const String16& id, std::unique_ptr<protocol::Debugger::Location> location, std::unique_ptr<protocol::Profiler::CPUProfile> profile, const Maybe<String16>& title = Maybe<String16>());

    void flush() { m_frontendChannel->flushProtocolNotifications(); }
private:
    FrontendChannel* m_frontendChannel;
};

// ------------- Dispatcher.

class PLATFORM_EXPORT Dispatcher {
public:
    static void wire(UberDispatcher*, blink::protocol::Profiler::Backend*);

private:
    Dispatcher() { }
};

// ------------- Metainfo.

class PLATFORM_EXPORT Metainfo {
public:
    using BackendClass = Backend;
    using FrontendClass = Frontend;
    using DispatcherClass = Dispatcher;
    static const char domainName[];
    static const char commandPrefix[];
};

} // namespace Profiler
} // namespace protocol
} // namespace blink

#endif // !defined(protocol_Profiler_h)
