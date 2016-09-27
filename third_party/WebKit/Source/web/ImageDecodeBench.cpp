// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef WTF

Provides a minimal wrapping of the Blink image decoders. Used to perform
a non-threaded, memory-to-memory image decode using micro second accuracy
clocks to measure image decode time. Optionally applies color correction
during image decoding on supported platforms (default off). Usage:

  % ninja -C /out/Release image_decode_bench &&
     ./out/Release/image_decode_bench file [iterations]

FIXME: Consider adding md5 checksum support to WTF. Use it to compute the
decoded image frame md5 and output that value.

FIXME: Consider adding an input data packetization option. Break the input
into network-sized packets, pump the packets into the image decoder until
the input data is complete (allDataReceived). This to include any internal
SharedBuffer costs, such as buffer coalescing, in the reported results.

FIXME: Consider integrating this tool in Chrome telemetry for realz, using
the image corpii used to assess Blink image decode performance. Refer to
http://crbug.com/398235#c103 and http://crbug.com/258324#c5

#endif

#include "config.h"

#include "platform/SharedBuffer.h"
#include "platform/image-decoders/ImageDecoder.h"
#include "public/platform/Platform.h"
#include "public/web/WebKit.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"

#if defined(_WIN32)
#if defined(WIN32_LEAN_AND_MEAN)
#error Fix: WIN32_LEAN_AND_MEAN disables timeBeginPeriod/TimeEndPeriod.
#endif
#include <mmsystem.h>
#include <sys/stat.h>
#include <time.h>
#define stat(x,y) _stat(x,y)
typedef struct _stat sttype;
#else
#include <sys/stat.h>
#include <sys/time.h>
typedef struct stat sttype;
#endif

using namespace blink;

#if defined(_WIN32)

// There is no real platform support herein, so adopt the WIN32 performance counter from
// WTF http://trac.webkit.org/browser/trunk/Source/WTF/wtf/CurrentTime.cpp?rev=152438

static double lowResUTCTime()
{
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);

    // As per Windows documentation for FILETIME, copy the resulting FILETIME structure to a
    // ULARGE_INTEGER structure using memcpy (using memcpy instead of direct assignment can
    // prevent alignment faults on 64-bit Windows).
    ULARGE_INTEGER dateTime;
    memcpy(&dateTime, &fileTime, sizeof(dateTime));

    // Number of 100 nanosecond between January 1, 1601 and January 1, 1970.
    static const ULONGLONG epochBias = 116444736000000000ULL;
    // Windows file times are in 100s of nanoseconds.
    static const double hundredsOfNanosecondsPerMillisecond = 10000;
    return (dateTime.QuadPart - epochBias) / hundredsOfNanosecondsPerMillisecond;
}

static LARGE_INTEGER qpcFrequency;
static bool syncedTime;

static double highResUpTime()
{
    // We use QPC, but only after sanity checking its result, due to bugs:
    // http://support.microsoft.com/kb/274323 http://support.microsoft.com/kb/895980
    // http://msdn.microsoft.com/en-us/library/ms644904.aspx ("you can get different results
    // on different processors due to bugs in the basic input/output system (BIOS) or the
    // hardware abstraction layer (HAL).").

    static LARGE_INTEGER qpcLast;
    static DWORD tickCountLast;
    static bool inited;

    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    DWORD tickCount = GetTickCount();

    if (inited) {
        __int64 qpcElapsed = ((qpc.QuadPart - qpcLast.QuadPart) * 1000) / qpcFrequency.QuadPart;
        __int64 tickCountElapsed;
        if (tickCount >= tickCountLast) {
            tickCountElapsed = (tickCount - tickCountLast);
        } else {
            __int64 tickCountLarge = tickCount + 0x100000000I64;
            tickCountElapsed = tickCountLarge - tickCountLast;
        }

        // Force a re-sync if QueryPerformanceCounter differs from GetTickCount() by more than
        // 500ms. (The 500ms value is from http://support.microsoft.com/kb/274323).
        __int64 diff = tickCountElapsed - qpcElapsed;
        if (diff > 500 || diff < -500)
            syncedTime = false;
    } else {
        inited = true;
    }

    qpcLast = qpc;
    tickCountLast = tickCount;

    return (1000.0 * qpc.QuadPart) / static_cast<double>(qpcFrequency.QuadPart);
}

static bool qpcAvailable()
{
    static bool available;
    static bool checked;

    if (checked)
        return available;

    available = QueryPerformanceFrequency(&qpcFrequency);
    checked = true;
    return available;
}

static double getCurrentTime()
{
    // Use a combination of ftime and QueryPerformanceCounter.
    // ftime returns the information we want, but doesn't have sufficient resolution.
    // QueryPerformanceCounter has high resolution, but is only usable to measure time intervals.
    // To combine them, we call ftime and QueryPerformanceCounter initially. Later calls will
    // use QueryPerformanceCounter by itself, adding the delta to the saved ftime.
    // We periodically re-sync to correct for drift.
    static double syncLowResUTCTime;
    static double syncHighResUpTime;
    static double lastUTCTime;

    double lowResTime = lowResUTCTime();
    if (!qpcAvailable())
        return lowResTime * (1.0 / 1000.0);

    double highResTime = highResUpTime();
    if (!syncedTime) {
        timeBeginPeriod(1); // increase time resolution around low-res time getter
        syncLowResUTCTime = lowResTime = lowResUTCTime();
        timeEndPeriod(1); // restore time resolution
        syncHighResUpTime = highResTime;
        syncedTime = true;
    }

    double highResElapsed = highResTime - syncHighResUpTime;
    double utc = syncLowResUTCTime + highResElapsed;

    // Force a clock re-sync if we've drifted.
    double lowResElapsed = lowResTime - syncLowResUTCTime;
    const double maximumAllowedDriftMsec = 15.625 * 2.0; // 2x the typical low-res accuracy
    if (fabs(highResElapsed - lowResElapsed) > maximumAllowedDriftMsec)
        syncedTime = false;

    // Make sure time doesn't run backwards (only correct if the difference is < 2 seconds,
    // since DST or clock changes could occur).
    const double backwardTimeLimit = 2000.0;
    if (utc < lastUTCTime && (lastUTCTime - utc) < backwardTimeLimit)
        return lastUTCTime * (1.0 / 1000.0);

    lastUTCTime = utc;
    return utc * (1.0 / 1000.0);
}

#else

static double getCurrentTime()
{
    struct timeval now;
    gettimeofday(&now, 0);
    return now.tv_sec + now.tv_usec * (1.0 / 1000000.0);
}

#endif

void getScreenColorProfile(WebVector<char>* profile)
{
    static unsigned char profileData[] = {
        0x00,0x00,0x01,0xea,0x54,0x45,0x53,0x54,0x00,0x00,0x00,0x00,
        0x6d,0x6e,0x74,0x72,0x52,0x47,0x42,0x20,0x58,0x59,0x5a,0x20,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x61,0x63,0x73,0x70,0x74,0x65,0x73,0x74,0x00,0x00,0x00,0x00,
        0x74,0x65,0x73,0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf6,0xd6,
        0x00,0x01,0x00,0x00,0x00,0x00,0xd3,0x2d,0x74,0x65,0x73,0x74,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,
        0x63,0x70,0x72,0x74,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x0d,
        0x64,0x65,0x73,0x63,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x8c,
        0x77,0x74,0x70,0x74,0x00,0x00,0x01,0x8c,0x00,0x00,0x00,0x14,
        0x72,0x58,0x59,0x5a,0x00,0x00,0x01,0xa0,0x00,0x00,0x00,0x14,
        0x67,0x58,0x59,0x5a,0x00,0x00,0x01,0xb4,0x00,0x00,0x00,0x14,
        0x62,0x58,0x59,0x5a,0x00,0x00,0x01,0xc8,0x00,0x00,0x00,0x14,
        0x72,0x54,0x52,0x43,0x00,0x00,0x01,0xdc,0x00,0x00,0x00,0x0e,
        0x67,0x54,0x52,0x43,0x00,0x00,0x01,0xdc,0x00,0x00,0x00,0x0e,
        0x62,0x54,0x52,0x43,0x00,0x00,0x01,0xdc,0x00,0x00,0x00,0x0e,
        0x74,0x65,0x78,0x74,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x64,0x65,0x73,0x63,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x10,0x77,0x68,0x61,0x63,0x6b,0x65,0x64,0x2e,
        0x69,0x63,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x58,0x59,0x5a,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0xf3,0x52,
        0x00,0x01,0x00,0x00,0x00,0x01,0x16,0xcc,0x58,0x59,0x5a,0x20,
        0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x8d,0x00,0x00,0xa0,0x2c,
        0x00,0x00,0x0f,0x95,0x58,0x59,0x5a,0x20,0x00,0x00,0x00,0x00,
        0x00,0x00,0x26,0x31,0x00,0x00,0x10,0x2f,0x00,0x00,0xbe,0x9b,
        0x58,0x59,0x5a,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x9c,0x18,
        0x00,0x00,0x4f,0xa5,0x00,0x00,0x04,0xfc,0x63,0x75,0x72,0x76,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x33
    };

    static struct WhackedColorProfile {

        char* data() { return reinterpret_cast<char*>(profileData); }

        const size_t profileSize = 490u;

        size_t size() { return profileSize; }

    } screenProfile;

    profile->assign(screenProfile.data(), screenProfile.size());
}

PassRefPtr<SharedBuffer> readFile(const char* fileName)
{
    FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        fprintf(stderr, "Can't open file %s\n", fileName);
        exit(2);
    }

    sttype s;
    stat(fileName, &s);
    size_t fileSize = s.st_size;
    if (s.st_size <= 0)
        return SharedBuffer::create();

    OwnPtr<unsigned char[]> buffer = adoptArrayPtr(new unsigned char[fileSize]);
    if (fileSize != fread(buffer.get(), 1, fileSize, fp)) {
        fprintf(stderr, "Error reading file %s\n", fileName);
        exit(2);
    }

    fclose(fp);
    return SharedBuffer::create(buffer.get(), fileSize);
}

bool decodeImageData(SharedBuffer* data, bool colorCorrection)
{
    OwnPtr<ImageDecoder> decoder = ImageDecoder::create(*data,
        ImageSource::AlphaPremultiplied, colorCorrection ?
            ImageSource::GammaAndColorProfileApplied : ImageSource::GammaAndColorProfileIgnored);

    bool allDataReceived = true;
    decoder->setData(data, allDataReceived);

    int frameCount = decoder->frameCount();
    for (int i = 0; i < frameCount; ++i) {
        if (!decoder->frameBufferAtIndex(i))
            return false;
    }

    return !decoder->failed();
}

int main(int argc, char* argv[])
{
    char* name = argv[0];

    // If the platform supports color correction, allow it to be controlled.

    bool applyColorCorrection = false;

#if USE(QCMSLIB)
    if (argc >= 2 && strcmp(argv[1], "--color-correct") == 0)
        applyColorCorrection = (--argc, ++argv, true);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [--color-correct] file [iterations]\n", name);
        exit(1);
    }
#else
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file [iterations]\n", name);
        exit(1);
    }
#endif

    // Control decode bench iterations.

    size_t iterations = 1;
    if (argc >= 3) {
        char* end = 0;
        iterations = strtol(argv[2], &end, 10);
        if (*end != '\0' || !iterations) {
            fprintf(stderr, "Second argument should be number of iterations. "
                "The default is 1. You supplied %s\n", argv[2]);
            exit(1);
        }
    }

    // Create a web platform without V8.

    class WebPlatform : public blink::Platform {
    public:
        const unsigned char* getTraceCategoryEnabledFlag(const char*) override
        {
            return reinterpret_cast<const unsigned char *>("nope-none-nada");
        }

        void cryptographicallyRandomValues(unsigned char*, size_t) override
        {
            // Do nothing: make blink::Platform use the default crypto-randoms.
        }

        void screenColorProfile(WebVector<char>* profile) override
        {
            getScreenColorProfile(profile); // Returns a whacked color profile.
        }
    };

    blink::initializeWithoutV8(new WebPlatform());

    // Set image decoding Platform options.

#if USE(QCMSLIB)
    ImageDecoder::qcmsOutputDeviceProfile(); // Initialize screen colorProfile.
#endif

    // Read entire file content to data.

    RefPtr<SharedBuffer> data = readFile(argv[1]);
    if (!data.get() || !data->size()) {
        fprintf(stderr, "Error reading image data from [%s]\n", argv[1]);
        exit(2);
    }

    // Image decode bench for iterations.

    double totalTime = 0.0;

    for (size_t i = 0; i < iterations; ++i) {
        double startTime = getCurrentTime();
        bool decoded = decodeImageData(data.get(), applyColorCorrection);
        double elapsedTime = getCurrentTime() - startTime;
        totalTime += elapsedTime;
        if (!decoded) {
            fprintf(stderr, "Image decode failed [%s]\n", argv[1]);
            exit(3);
        }
    }

    // Results to stdout.

    double averageTime = totalTime / static_cast<double>(iterations);
    printf("%f %f\n", totalTime, averageTime);
    return 0;
}
