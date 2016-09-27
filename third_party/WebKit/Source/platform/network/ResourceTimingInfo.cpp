// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/network/ResourceTimingInfo.h"

#include "platform/CrossThreadCopier.h"

namespace blink {

PassOwnPtr<ResourceTimingInfo> ResourceTimingInfo::adopt(PassOwnPtr<CrossThreadResourceTimingInfoData> data)
{
    OwnPtr<ResourceTimingInfo> info = ResourceTimingInfo::create(AtomicString(data->m_type), data->m_initialTime, data->m_isMainResource);
    info->m_originalTimingAllowOrigin = AtomicString(data->m_originalTimingAllowOrigin);
    info->m_loadFinishTime = data->m_loadFinishTime;
    info->m_initialRequest = *ResourceRequest::adopt(data->m_initialRequest.release());
    info->m_finalResponse = *ResourceResponse::adopt(data->m_finalResponse.release());
    for (auto& responseData : data->m_redirectChain)
        info->m_redirectChain.append(*ResourceResponse::adopt(responseData.release()));
    return info.release();
}

PassOwnPtr<CrossThreadResourceTimingInfoData> ResourceTimingInfo::copyData() const
{
    OwnPtr<CrossThreadResourceTimingInfoData> data = adoptPtr(new CrossThreadResourceTimingInfoData);
    data->m_type = m_type.string().isolatedCopy();
    data->m_originalTimingAllowOrigin = m_originalTimingAllowOrigin.string().isolatedCopy();
    data->m_initialTime = m_initialTime;
    data->m_loadFinishTime = m_loadFinishTime;
    data->m_initialRequest = m_initialRequest.copyData();
    data->m_finalResponse = m_finalResponse.copyData();
    for (const auto& response : m_redirectChain)
        data->m_redirectChain.append(response.copyData());
    data->m_isMainResource = m_isMainResource;
    return data.release();
}

} // namespace blink
