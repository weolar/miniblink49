/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/loader/ProgressTracker.h"

#include "core/fetch/ResourceFetcher.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "platform/Logging.h"
#include "platform/network/ResourceResponse.h"
#include "wtf/CurrentTime.h"
#include "wtf/text/CString.h"

using namespace std;

namespace blink {

// Always start progress at initialProgressValue. This helps provide feedback as
// soon as a load starts.
static const double initialProgressValue = 0.1;

// Similarly, always leave space at the end. This helps show the user that we're not done
// until we're done.
static const double finalProgressValue = 0.9; // 1.0 - initialProgressValue

static const int progressItemDefaultEstimatedLength = 1024 * 16;

struct ProgressItem {
    WTF_MAKE_NONCOPYABLE(ProgressItem); WTF_MAKE_FAST_ALLOCATED(ProgressItem);
public:
    ProgressItem(long long length)
        : bytesReceived(0)
        , estimatedLength(length) { }

    long long bytesReceived;
    long long estimatedLength;
};

PassOwnPtrWillBeRawPtr<ProgressTracker> ProgressTracker::create(LocalFrame* frame)
{
    return adoptPtrWillBeNoop(new ProgressTracker(frame));
}

ProgressTracker::ProgressTracker(LocalFrame* frame)
    : m_frame(frame)
    , m_totalPageAndResourceBytesToLoad(0)
    , m_totalBytesReceived(0)
    , m_lastNotifiedProgressValue(0)
    , m_lastNotifiedProgressTime(0)
    , m_progressNotificationInterval(0.02)
    , m_progressNotificationTimeInterval(0.1)
    , m_finalProgressChangedSent(false)
    , m_progressValue(0)
{
}

ProgressTracker::~ProgressTracker()
{
}

DEFINE_TRACE(ProgressTracker)
{
    visitor->trace(m_frame);
}

void ProgressTracker::dispose()
{
    if (m_frame->isLoading())
        progressCompleted();
    ASSERT(!m_frame->isLoading());
}

double ProgressTracker::estimatedProgress() const
{
    return m_progressValue;
}

void ProgressTracker::reset()
{
    m_progressItems.clear();

    m_totalPageAndResourceBytesToLoad = 0;
    m_totalBytesReceived = 0;
    m_progressValue = 0;
    m_lastNotifiedProgressValue = 0;
    m_lastNotifiedProgressTime = 0;
    m_finalProgressChangedSent = false;
}

void ProgressTracker::progressStarted()
{
    if (!m_frame->isLoading()) {
        reset();
        m_progressValue = initialProgressValue;
        m_frame->loader().client()->didStartLoading(NavigationToDifferentDocument);
    }
    m_frame->setIsLoading(true);
    InspectorInstrumentation::frameStartedLoading(m_frame);
}

void ProgressTracker::progressCompleted()
{
    ASSERT(m_frame->isLoading());
    m_frame->setIsLoading(false);
    if (!m_finalProgressChangedSent) {
        m_progressValue = 1;
        m_frame->loader().client()->progressEstimateChanged(m_progressValue);
    }
    reset();
    m_frame->loader().client()->didStopLoading();
    InspectorInstrumentation::frameStoppedLoading(m_frame);
}

void ProgressTracker::incrementProgress(unsigned long identifier, const ResourceResponse& response)
{
    if (!m_frame->isLoading())
        return;

    long long estimatedLength = response.expectedContentLength();
    if (estimatedLength < 0)
        estimatedLength = progressItemDefaultEstimatedLength;

    m_totalPageAndResourceBytesToLoad += estimatedLength;

    if (ProgressItem* item = m_progressItems.get(identifier)) {
        item->bytesReceived = 0;
        item->estimatedLength = estimatedLength;
    } else {
        m_progressItems.set(identifier, adoptPtr(new ProgressItem(estimatedLength)));
    }
}

void ProgressTracker::incrementProgress(unsigned long identifier, int length)
{
    ProgressItem* item = m_progressItems.get(identifier);

    // FIXME: Can this ever happen?
    if (!item)
        return;

    unsigned bytesReceived = length;
    double increment, percentOfRemainingBytes;
    long long remainingBytes, estimatedBytesForPendingRequests;

    item->bytesReceived += bytesReceived;
    if (item->bytesReceived > item->estimatedLength) {
        m_totalPageAndResourceBytesToLoad += ((item->bytesReceived * 2) - item->estimatedLength);
        item->estimatedLength = item->bytesReceived * 2;
    }

    int numPendingOrLoadingRequests = m_frame->document()->fetcher()->requestCount();
    estimatedBytesForPendingRequests = progressItemDefaultEstimatedLength * numPendingOrLoadingRequests;
    remainingBytes = ((m_totalPageAndResourceBytesToLoad + estimatedBytesForPendingRequests) - m_totalBytesReceived);
    if (remainingBytes > 0) // Prevent divide by 0.
        percentOfRemainingBytes = (double)bytesReceived / (double)remainingBytes;
    else
        percentOfRemainingBytes = 1.0;

    // For documents that use WebCore's layout system, treat first layout as the half-way point.
    bool useClampedMaxProgress = !m_frame->view()->didFirstLayout();
    double maxProgressValue = useClampedMaxProgress ? 0.5 : finalProgressValue;
    increment = (maxProgressValue - m_progressValue) * percentOfRemainingBytes;
    m_progressValue += increment;
    m_progressValue = min(m_progressValue, maxProgressValue);
    ASSERT(m_progressValue >= initialProgressValue);

    m_totalBytesReceived += bytesReceived;

    double now = currentTime();
    double notifiedProgressTimeDelta = now - m_lastNotifiedProgressTime;

    double notificationProgressDelta = m_progressValue - m_lastNotifiedProgressValue;
    if (notificationProgressDelta >= m_progressNotificationInterval || notifiedProgressTimeDelta >= m_progressNotificationTimeInterval) {
        if (!m_finalProgressChangedSent) {
            if (m_progressValue == 1)
                m_finalProgressChangedSent = true;

            m_frame->loader().client()->progressEstimateChanged(m_progressValue);

            m_lastNotifiedProgressValue = m_progressValue;
            m_lastNotifiedProgressTime = now;
        }
    }
}

void ProgressTracker::completeProgress(unsigned long identifier)
{
    ProgressItem* item = m_progressItems.get(identifier);

    // This can happen if a load fails without receiving any response data.
    if (!item)
        return;

    // Adjust the total expected bytes to account for any overage/underage.
    long long delta = item->bytesReceived - item->estimatedLength;
    m_totalPageAndResourceBytesToLoad += delta;

    m_progressItems.remove(identifier);
}

}
