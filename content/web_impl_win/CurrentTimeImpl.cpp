/*
 * Copyright (C) 2006, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Google Inc. All rights reserved.
 * Copyright (C) 2007-2009 Torch Mobile, Inc.
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
#include "CurrentTimeImpl.h"

// Windows is first since we want to use hires timers, despite USE(CF)
// being defined.
// If defined, WIN32_LEAN_AND_MEAN disables timeBeginPeriod/timeEndPeriod.
#undef WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <sys/timeb.h>

namespace content {

const double msPerSecond = 1000.0; // weolar

static LARGE_INTEGER s_qpcFrequency = { 0 };
static bool s_syncedTime = false;

static double highResUpTime()
{
    // We use QPC, but only after sanity checking its result, due to bugs:
    // http://support.microsoft.com/kb/274323
    // http://support.microsoft.com/kb/895980
    // http://msdn.microsoft.com/en-us/library/ms644904.aspx ("...you can get different results on different processors due to bugs in the basic input/output system (BIOS) or the hardware abstraction layer (HAL)."

    static LARGE_INTEGER s_qpcLast = { 0 };
    static DWORD s_tickCountLast = 0;
    static bool s_inited = false;

    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    DWORD tickCount = GetTickCount();

    if (s_inited) {
        __int64 qpcElapsed = ((qpc.QuadPart - s_qpcLast.QuadPart) * 1000) / s_qpcFrequency.QuadPart;
        __int64 tickCountElapsed;
        if (tickCount >= s_tickCountLast)
            tickCountElapsed = (tickCount - s_tickCountLast);
        else {
#if COMPILER(MINGW)
            __int64 tickCountLarge = tickCount + 0x100000000ULL;
#else
            __int64 tickCountLarge = tickCount + 0x100000000I64;
#endif
            tickCountElapsed = tickCountLarge - s_tickCountLast;
        }

        // force a re-sync if QueryPerformanceCounter differs from GetTickCount by more than 500ms.
        // (500ms value is from http://support.microsoft.com/kb/274323)
        __int64 diff = tickCountElapsed - qpcElapsed;
        if (diff > 500 || diff < -500)
            s_syncedTime = false;
    } else
        s_inited = true;

    s_qpcLast = qpc;
    s_tickCountLast = tickCount;

    return (1000.0 * qpc.QuadPart) / static_cast<double>(s_qpcFrequency.QuadPart);
}

static double lowResUTCTime()
{
#if 1 // OS(WINCE)
    SYSTEMTIME systemTime;
    ::GetSystemTime(&systemTime);
    struct tm tmtime;
    tmtime.tm_year = systemTime.wYear - 1900;
    tmtime.tm_mon = systemTime.wMonth - 1;
    tmtime.tm_mday = systemTime.wDay;
    tmtime.tm_wday = systemTime.wDayOfWeek;
    tmtime.tm_hour = systemTime.wHour;
    tmtime.tm_min = systemTime.wMinute;
    tmtime.tm_sec = systemTime.wSecond;
    time_t timet = mktime(&tmtime);
    return timet * msPerSecond + systemTime.wMilliseconds;
#else
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    return timebuffer.time * msPerSecond + timebuffer.millitm;
#endif
}

static bool qpcAvailable()
{
    static bool s_available = false;
    static bool s_checked = false;

    if (s_checked)
        return s_available;

    s_available = QueryPerformanceFrequency(&s_qpcFrequency);
    s_checked = true;
    return s_available;
}

// 老版本代码有个问题，就是使用了本地时间，会被人篡改。
// 所以我们舍弃精度，直接使用高精度计时器
double currentTimeImpl()
{
    static double s_syncLowResUTCTime = 0;
    static double s_syncHighResUpTime = 0;
    static double s_lastUTCTime = 0;
    static bool s_isSyncedTime = false;

    double lowResTime = lowResUTCTime();

    if (!qpcAvailable())
        return lowResTime / 1000.0;

    double highResTime = highResUpTime();

    if (!s_isSyncedTime) {
        timeBeginPeriod(1); // increase time resolution around low-res time getter
        s_syncLowResUTCTime = lowResTime = lowResUTCTime();
        timeEndPeriod(1); // restore time resolution
        s_syncHighResUpTime = highResTime;
        s_isSyncedTime = true;
    }

    double highResElapsed = highResTime - s_syncHighResUpTime;
    double utc = s_syncLowResUTCTime + highResElapsed;

    if (utc < s_lastUTCTime)
        utc = s_lastUTCTime + 0.0001;

    s_lastUTCTime = utc;
    return utc / 1000.0;
}

double currentTimeImpl_unuse()
{
    // Use a combination of ftime and QueryPerformanceCounter.
    // ftime returns the information we want, but doesn't have sufficient resolution.
    // QueryPerformanceCounter has high resolution, but is only usable to measure time intervals.
    // To combine them, we call ftime and QueryPerformanceCounter initially. Later calls will use QueryPerformanceCounter
    // by itself, adding the delta to the saved ftime.  We periodically re-sync to correct for drift.
    static double s_syncLowResUTCTime;
    static double s_syncHighResUpTime;
    static double s_lastUTCTime;

    double lowResTime = lowResUTCTime();

    if (!qpcAvailable())
        return lowResTime / 1000.0;

    double highResTime = highResUpTime();

    if (!s_syncedTime) {
        timeBeginPeriod(1); // increase time resolution around low-res time getter
        s_syncLowResUTCTime = lowResTime = lowResUTCTime();
        timeEndPeriod(1); // restore time resolution
        s_syncHighResUpTime = highResTime;
        s_syncedTime = true;
    }

    double highResElapsed = highResTime - s_syncHighResUpTime;
    double utc = s_syncLowResUTCTime + highResElapsed;

    // force a clock re-sync if we've drifted
    double lowResElapsed = lowResTime - s_syncLowResUTCTime;
    const double maximumAllowedDriftMsec = 15.625 * 2.0; // 2x the typical low-res accuracy
    if (fabs(highResElapsed - lowResElapsed) > maximumAllowedDriftMsec)
        s_syncedTime = false;

    // make sure time doesn't run backwards (only correct if difference is < 2 seconds, since DST or clock changes could occur)
    const double backwardTimeLimit = 2000.0;
    if (utc < s_lastUTCTime && (s_lastUTCTime - utc) < backwardTimeLimit)
        return s_lastUTCTime / 1000.0;
    s_lastUTCTime = utc;
    return utc / 1000.0;
}

} // namespace WTF
