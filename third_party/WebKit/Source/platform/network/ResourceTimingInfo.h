/*
 * Copyright (C) 2013 Intel Inc. All rights reserved.
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

#ifndef ResourceTimingInfo_h
#define ResourceTimingInfo_h

#include "platform/CrossThreadCopier.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "wtf/text/AtomicString.h"

namespace blink {

struct CrossThreadResourceTimingInfoData;

class PLATFORM_EXPORT ResourceTimingInfo {
public:
    static PassOwnPtr<ResourceTimingInfo> create(const AtomicString& type, const double time, bool isMainResource)
    {
        return adoptPtr(new ResourceTimingInfo(type, time, isMainResource));
    }
    static PassOwnPtr<ResourceTimingInfo> adopt(PassOwnPtr<CrossThreadResourceTimingInfoData>);

    // Gets a copy of the data suitable for passing to another thread.
    PassOwnPtr<CrossThreadResourceTimingInfoData> copyData() const;

    double initialTime() const { return m_initialTime; }
    bool isMainResource() const { return m_isMainResource; }

    void setInitiatorType(const AtomicString& type) { m_type = type; }
    const AtomicString& initiatorType() const { return m_type; }

    void setOriginalTimingAllowOrigin(const AtomicString& originalTimingAllowOrigin) { m_originalTimingAllowOrigin = originalTimingAllowOrigin; }
    const AtomicString& originalTimingAllowOrigin() const { return m_originalTimingAllowOrigin; }

    void setLoadFinishTime(double time) { m_loadFinishTime = time; }
    double loadFinishTime() const { return m_loadFinishTime; }

    void setInitialRequest(const ResourceRequest& request) { m_initialRequest = request; }
    const ResourceRequest& initialRequest() const { return m_initialRequest; }

    void setFinalResponse(const ResourceResponse& response) { m_finalResponse = response; }
    const ResourceResponse& finalResponse() const { return m_finalResponse; }

    void addRedirect(const ResourceResponse& redirectResponse) { m_redirectChain.append(redirectResponse); }
    const Vector<ResourceResponse>& redirectChain() const { return m_redirectChain; }

    void clearLoadTimings()
    {
        m_finalResponse.setResourceLoadTiming(nullptr);
        for (ResourceResponse& redirect : m_redirectChain)
            redirect.setResourceLoadTiming(nullptr);
    }

private:
    ResourceTimingInfo(const AtomicString& type, const double time, bool isMainResource)
        : m_type(type)
        , m_initialTime(time)
        , m_isMainResource(isMainResource)
    {
    }

    AtomicString m_type;
    AtomicString m_originalTimingAllowOrigin;
    double m_initialTime;
    double m_loadFinishTime;
    ResourceRequest m_initialRequest;
    ResourceResponse m_finalResponse;
    Vector<ResourceResponse> m_redirectChain;
    bool m_isMainResource;
};

struct CrossThreadResourceTimingInfoData {
    WTF_MAKE_NONCOPYABLE(CrossThreadResourceTimingInfoData);
    WTF_MAKE_FAST_ALLOCATED(CrossThreadResourceTimingInfoData);
public:
    CrossThreadResourceTimingInfoData() {}

    String m_type;
    String m_originalTimingAllowOrigin;
    double m_initialTime;
    double m_loadFinishTime;
    OwnPtr<CrossThreadResourceRequestData> m_initialRequest;
    OwnPtr<CrossThreadResourceResponseData> m_finalResponse;
    Vector<OwnPtr<CrossThreadResourceResponseData>> m_redirectChain;
    bool m_isMainResource;
};

template<> struct CrossThreadCopierBase<false, false, false, ResourceTimingInfo> {
    typedef PassOwnPtr<CrossThreadResourceTimingInfoData> Type;
    static Type copy(const ResourceTimingInfo& info) { return info.copyData(); }
};

} // namespace blink

#endif
