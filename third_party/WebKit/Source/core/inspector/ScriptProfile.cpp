/*
 * Copyright (c) 2010, Google Inc. All rights reserved.
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
#include "core/inspector/ScriptProfile.h"

#include "bindings/core/v8/V8Binding.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include <v8.h>

namespace blink {

ScriptProfile::~ScriptProfile()
{
    m_profile->Delete();
}

String ScriptProfile::title() const
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());
    return toCoreString(m_profile->GetTitle());
}

double ScriptProfile::idleTime() const
{
    return m_idleTime;
}

double ScriptProfile::startTime() const
{
    return static_cast<double>(m_profile->GetStartTime()) / 1000000;
}

double ScriptProfile::endTime() const
{
    return static_cast<double>(m_profile->GetEndTime()) / 1000000;
}

static RefPtr<TypeBuilder::Array<TypeBuilder::Profiler::PositionTickInfo> > buildInspectorObjectForPositionTicks(const v8::CpuProfileNode* node)
{
    RefPtr<TypeBuilder::Array<TypeBuilder::Profiler::PositionTickInfo> > array = TypeBuilder::Array<TypeBuilder::Profiler::PositionTickInfo>::create();
    unsigned lineCount = node->GetHitLineCount();
    if (!lineCount)
        return array;

    Vector<v8::CpuProfileNode::LineTick> entries(lineCount);
    if (node->GetLineTicks(&entries[0], lineCount)) {
        for (unsigned i = 0; i < lineCount; i++) {
            RefPtr<TypeBuilder::Profiler::PositionTickInfo> line = TypeBuilder::Profiler::PositionTickInfo::create()
                .setLine(entries[i].line)
                .setTicks(entries[i].hit_count);
            array->addItem(line);
        }
    }

    return array;
}

static PassRefPtr<TypeBuilder::Profiler::CPUProfileNode> buildInspectorObjectFor(const v8::CpuProfileNode* node)
{
    v8::HandleScope handleScope(v8::Isolate::GetCurrent());

    RefPtr<TypeBuilder::Array<TypeBuilder::Profiler::CPUProfileNode> > children = TypeBuilder::Array<TypeBuilder::Profiler::CPUProfileNode>::create();
    const int childrenCount = node->GetChildrenCount();
    for (int i = 0; i < childrenCount; i++) {
        const v8::CpuProfileNode* child = node->GetChild(i);
        children->addItem(buildInspectorObjectFor(child));
    }

    RefPtr<TypeBuilder::Array<TypeBuilder::Profiler::PositionTickInfo> > positionTicks = buildInspectorObjectForPositionTicks(node);

    RefPtr<TypeBuilder::Profiler::CPUProfileNode> result = TypeBuilder::Profiler::CPUProfileNode::create()
        .setFunctionName(toCoreString(node->GetFunctionName()))
        .setScriptId(String::number(node->GetScriptId()))
        .setUrl(toCoreString(node->GetScriptResourceName()))
        .setLineNumber(node->GetLineNumber())
        .setColumnNumber(node->GetColumnNumber())
        .setHitCount(node->GetHitCount())
        .setCallUID(node->GetCallUid())
        .setChildren(children.release())
        .setPositionTicks(positionTicks.release())
        .setDeoptReason(node->GetBailoutReason())
        .setId(node->GetNodeId());
    return result.release();
}

PassRefPtr<TypeBuilder::Profiler::CPUProfileNode> ScriptProfile::buildInspectorObjectForHead() const
{
    return buildInspectorObjectFor(m_profile->GetTopDownRoot());
}

PassRefPtr<TypeBuilder::Array<int> > ScriptProfile::buildInspectorObjectForSamples() const
{
    RefPtr<TypeBuilder::Array<int> > array = TypeBuilder::Array<int>::create();
    int count = m_profile->GetSamplesCount();
    for (int i = 0; i < count; i++)
        array->addItem(m_profile->GetSample(i)->GetNodeId());
    return array.release();
}

PassRefPtr<TypeBuilder::Array<double> > ScriptProfile::buildInspectorObjectForTimestamps() const
{
    RefPtr<TypeBuilder::Array<double> > array = TypeBuilder::Array<double>::create();
    int count = m_profile->GetSamplesCount();
    for (int i = 0; i < count; i++)
        array->addItem(m_profile->GetSampleTimestamp(i));
    return array.release();
}

} // namespace blink
