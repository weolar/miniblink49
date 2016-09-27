/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/inspector/InspectorProfilerAgent.h"

#include "bindings/core/v8/ScriptCallStackFactory.h"
#include "bindings/core/v8/ScriptProfiler.h"
#include "core/frame/ConsoleTypes.h"
#include "core/frame/UseCounter.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/InspectorOverlay.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/inspector/ScriptProfile.h"
#include "wtf/CurrentTime.h"
#include "wtf/text/StringConcatenate.h"

namespace blink {

namespace ProfilerAgentState {
static const char samplingInterval[] = "samplingInterval";
static const char userInitiatedProfiling[] = "userInitiatedProfiling";
static const char profilerEnabled[] = "profilerEnabled";
static const char nextProfileId[] = "nextProfileId";
}

static PassRefPtr<TypeBuilder::Profiler::CPUProfile> createCPUProfile(const ScriptProfile& scriptProfile)
{
    RefPtr<TypeBuilder::Profiler::CPUProfile> profile = TypeBuilder::Profiler::CPUProfile::create()
        .setHead(scriptProfile.buildInspectorObjectForHead())
        .setStartTime(scriptProfile.startTime())
        .setEndTime(scriptProfile.endTime());
    profile->setSamples(scriptProfile.buildInspectorObjectForSamples());
    profile->setTimestamps(scriptProfile.buildInspectorObjectForTimestamps());
    return profile.release();
}

static PassRefPtr<TypeBuilder::Debugger::Location> currentDebugLocation()
{
    RefPtrWillBeRawPtr<ScriptCallStack> callStack(createScriptCallStack(1));
    const ScriptCallFrame& lastCaller = callStack->at(0);
    RefPtr<TypeBuilder::Debugger::Location> location = TypeBuilder::Debugger::Location::create()
        .setScriptId(lastCaller.scriptId())
        .setLineNumber(lastCaller.lineNumber());
    location->setColumnNumber(lastCaller.columnNumber());
    return location.release();
}

class InspectorProfilerAgent::ProfileDescriptor {
public:
    ProfileDescriptor(const String& id, const String& title)
        : m_id(id)
        , m_title(title) { }
    String m_id;
    String m_title;
};

PassOwnPtrWillBeRawPtr<InspectorProfilerAgent> InspectorProfilerAgent::create(InjectedScriptManager* injectedScriptManager, InspectorOverlay* overlay)
{
    return adoptPtrWillBeNoop(new InspectorProfilerAgent(injectedScriptManager, overlay));
}

InspectorProfilerAgent::InspectorProfilerAgent(InjectedScriptManager* injectedScriptManager, InspectorOverlay* overlay)
    : InspectorBaseAgent<InspectorProfilerAgent, InspectorFrontend::Profiler>("Profiler")
    , m_injectedScriptManager(injectedScriptManager)
    , m_recordingCPUProfile(false)
    , m_profileNameIdleTimeMap(ScriptProfiler::currentProfileNameIdleTimeMap())
    , m_idleStartTime(0.0)
    , m_overlay(overlay)
{
}

InspectorProfilerAgent::~InspectorProfilerAgent()
{
}

void InspectorProfilerAgent::consoleProfile(ExecutionContext* context, const String& title)
{
    UseCounter::count(context, UseCounter::DevToolsConsoleProfile);
    ASSERT(frontend() && enabled());
    String id = nextProfileId();
    m_startedProfiles.append(ProfileDescriptor(id, title));
    ScriptProfiler::start(id);
    frontend()->consoleProfileStarted(id, currentDebugLocation(), title.isNull() ? 0 : &title);
}

void InspectorProfilerAgent::consoleProfileEnd(const String& title)
{
    ASSERT(frontend() && enabled());
    String id;
    String resolvedTitle;
    // Take last started profile if no title was passed.
    if (title.isNull()) {
        if (m_startedProfiles.isEmpty())
            return;
        id = m_startedProfiles.last().m_id;
        resolvedTitle = m_startedProfiles.last().m_title;
        m_startedProfiles.removeLast();
    } else {
        for (size_t i = 0; i < m_startedProfiles.size(); i++) {
            if (m_startedProfiles[i].m_title == title) {
                resolvedTitle = title;
                id = m_startedProfiles[i].m_id;
                m_startedProfiles.remove(i);
                break;
            }
        }
        if (id.isEmpty())
            return;
    }
    RefPtrWillBeRawPtr<ScriptProfile> profile = ScriptProfiler::stop(id);
    if (!profile)
        return;
    RefPtr<TypeBuilder::Debugger::Location> location = currentDebugLocation();
    frontend()->consoleProfileFinished(id, location, createCPUProfile(*profile), resolvedTitle.isNull() ? 0 : &resolvedTitle);
}

void InspectorProfilerAgent::enable(ErrorString*)
{
    m_state->setBoolean(ProfilerAgentState::profilerEnabled, true);
    doEnable();
}

void InspectorProfilerAgent::doEnable()
{
    m_instrumentingAgents->setInspectorProfilerAgent(this);
}

void InspectorProfilerAgent::disable(ErrorString*)
{
    for (Vector<ProfileDescriptor>::reverse_iterator it = m_startedProfiles.rbegin(); it != m_startedProfiles.rend(); ++it)
        ScriptProfiler::stop(it->m_id);
    m_startedProfiles.clear();
    stop(0, 0);
    m_instrumentingAgents->setInspectorProfilerAgent(nullptr);
    m_state->setBoolean(ProfilerAgentState::profilerEnabled, false);
}

bool InspectorProfilerAgent::enabled()
{
    return m_state->getBoolean(ProfilerAgentState::profilerEnabled);
}

void InspectorProfilerAgent::setSamplingInterval(ErrorString* error, int interval)
{
    if (m_recordingCPUProfile) {
        *error = "Cannot change sampling interval when profiling.";
        return;
    }
    m_state->setLong(ProfilerAgentState::samplingInterval, interval);
    ScriptProfiler::setSamplingInterval(interval);
}

void InspectorProfilerAgent::restore()
{
    if (m_state->getBoolean(ProfilerAgentState::profilerEnabled))
        doEnable();
    if (long interval = m_state->getLong(ProfilerAgentState::samplingInterval, 0))
        ScriptProfiler::setSamplingInterval(interval);
    if (m_state->getBoolean(ProfilerAgentState::userInitiatedProfiling)) {
        ErrorString error;
        start(&error);
    }
}

void InspectorProfilerAgent::start(ErrorString* error)
{
    if (m_recordingCPUProfile)
        return;
    if (!enabled()) {
        *error = "Profiler is not enabled";
        return;
    }
    m_recordingCPUProfile = true;
    if (m_overlay)
        m_overlay->suspendUpdates();
    m_frontendInitiatedProfileId = nextProfileId();
    ScriptProfiler::start(m_frontendInitiatedProfileId);
    m_state->setBoolean(ProfilerAgentState::userInitiatedProfiling, true);
}

void InspectorProfilerAgent::stop(ErrorString* errorString, RefPtr<TypeBuilder::Profiler::CPUProfile>& profile)
{
    stop(errorString, &profile);
}

void InspectorProfilerAgent::stop(ErrorString* errorString, RefPtr<TypeBuilder::Profiler::CPUProfile>* profile)
{
    if (!m_recordingCPUProfile) {
        if (errorString)
            *errorString = "No recording profiles found";
        return;
    }
    m_recordingCPUProfile = false;
    if (m_overlay)
        m_overlay->resumeUpdates();
    RefPtrWillBeRawPtr<ScriptProfile> scriptProfile = ScriptProfiler::stop(m_frontendInitiatedProfileId);
    m_frontendInitiatedProfileId = String();
    if (scriptProfile && profile)
        *profile = createCPUProfile(*scriptProfile);
    else if (errorString)
        *errorString = "Profile wasn't found";
    m_state->setBoolean(ProfilerAgentState::userInitiatedProfiling, false);
}

String InspectorProfilerAgent::nextProfileId()
{
    long nextId = m_state->getLong(ProfilerAgentState::nextProfileId, 1);
    m_state->setLong(ProfilerAgentState::nextProfileId, nextId + 1);
    return String::number(nextId);
}

void InspectorProfilerAgent::idleFinished()
{
    if (!m_profileNameIdleTimeMap || !m_profileNameIdleTimeMap->size())
        return;
    ScriptProfiler::setIdle(false);
    if (!m_idleStartTime)
        return;

    double idleTime = WTF::monotonicallyIncreasingTime() - m_idleStartTime;
    m_idleStartTime = 0.0;
    for (auto& map : *m_profileNameIdleTimeMap)
        map.value += idleTime;
}

void InspectorProfilerAgent::idleStarted()
{
    if (!m_profileNameIdleTimeMap || !m_profileNameIdleTimeMap->size())
        return;
    m_idleStartTime = WTF::monotonicallyIncreasingTime();
    ScriptProfiler::setIdle(true);
}

void InspectorProfilerAgent::willProcessTask()
{
    idleFinished();
}

void InspectorProfilerAgent::didProcessTask()
{
    idleStarted();
}

void InspectorProfilerAgent::willEnterNestedRunLoop()
{
    idleStarted();
}

void InspectorProfilerAgent::didLeaveNestedRunLoop()
{
    idleFinished();
}

DEFINE_TRACE(InspectorProfilerAgent)
{
    visitor->trace(m_injectedScriptManager);
    visitor->trace(m_overlay);
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink
