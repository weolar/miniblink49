// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/BufferedResourceLoader.h"

#include "media/BufferedConstant.h"
#include "content/browser/PostTaskHelper.h"
#include "net/ActivatingObjCheck.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/public/web/WebURLLoaderOptions.h"

using blink::WebFrame;
using blink::WebString;
using blink::WebURLError;
using blink::WebURLLoader;
using blink::WebURLLoaderOptions;
using blink::WebURLRequest;
using blink::WebURLResponse;

namespace media {

// Computes the suggested backward and forward capacity for the buffer
// if one wants to play at |playback_rate| * the natural playback speed.
// Use a value of 0 for |bitrate| if it is unknown.
static void ComputeTargetBufferWindow(double playback_rate, int bitrate,
                                      int* out_backward_capacity,
                                      int* out_forward_capacity) {
    static const int kDefaultBitrate = 200 * 1024 * 8;  // 200 Kbps.
    static const int kMaxBitrate = 20 * kMegabyte * 8;  // 20 Mbps.
    static const double kMaxPlaybackRate = 25.0;
    static const int kTargetSecondsBufferedAhead = 10;
    static const int kTargetSecondsBufferedBehind = 2;

    // Use a default bit rate if unknown and clamp to prevent overflow.
    if (bitrate <= 0)
        bitrate = kDefaultBitrate;
    bitrate = std::min(bitrate, kMaxBitrate);

    // Only scale the buffer window for playback rates greater than 1.0 in
    // magnitude and clamp to prevent overflow.
    bool backward_playback = false;
    if (playback_rate < 0.0) {
        backward_playback = true;
        playback_rate *= -1.0;
    }

    playback_rate = std::max(playback_rate, 1.0);
    playback_rate = std::min(playback_rate, kMaxPlaybackRate);

    int bytes_per_second = (bitrate / 8.0) * playback_rate;

    // Clamp between kMinBufferCapacity and kMaxBufferCapacity.
    *out_forward_capacity = std::max(kTargetSecondsBufferedAhead * bytes_per_second, kMinBufferCapacity);
    *out_backward_capacity = std::max(kTargetSecondsBufferedBehind * bytes_per_second, kMinBufferCapacity);

    *out_forward_capacity = std::min(*out_forward_capacity, kMaxBufferCapacity);
    *out_backward_capacity = std::min(*out_backward_capacity, kMaxBufferCapacity);

    if (backward_playback)
        std::swap(*out_forward_capacity, *out_backward_capacity);
}

enum UncacheableReason {
    kNoData = 1 << 0,  // Not 200 or 206.
    kPre11PartialResponse = 1 << 1,  // 206 but HTTP version < 1.1.
    kNoStrongValidatorOnPartialResponse = 1 << 2,  // 206, no strong validator.
    kShortMaxAge = 1 << 3,  // Max age less than 1h (arbitrary value).
    kExpiresTooSoon = 1 << 4,  // Expires in less than 1h (arbitrary value).
    kHasMustRevalidate = 1 << 5,  // Response asks for revalidation.
    kNoCache = 1 << 6,  // Response included a no-cache header.
    kNoStore = 1 << 7,  // Response included a no-store header.
    kMaxReason  // Needs to be one more than max legitimate reason.
};

// bool HasStrongValidators(WebURLResponse::HTTPVersion version, const std::string& etag_header,
//                                    const std::string& last_modified_header,
//                                    const std::string& date_header) {
//     if (version < WebURLResponse::HTTP_1_1)
//         return false;
// 
//     if (!etag_header.empty()) {
//         size_t slash = etag_header.find('/');
//         if (slash == std::string::npos || slash == 0)
//             return true;
// 
//         std::string::const_iterator i = etag_header.begin();
//         std::string::const_iterator j = etag_header.begin() + slash;
//         TrimLWS(&i, &j);
//         if (!base::LowerCaseEqualsASCII(base::StringPiece(i, j), "w"))
//             return true;
//     }
// 
//     base::Time last_modified;
//     if (!base::Time::FromString(last_modified_header.c_str(), &last_modified))
//         return false;
// 
//     base::Time date;
//     if (!base::Time::FromString(date_header.c_str(), &date))
//         return false;
// 
//     return ((date - last_modified).InSeconds() >= 60);
// }

uint32 GetReasonsForUncacheability(const WebURLResponse& response) {
    uint32 reasons = 0;
    const int code = response.httpStatusCode();
    const int version = response.httpVersion();
//     const HttpVersion http_version =
//         version == WebURLResponse::HTTPVersion_2_0
//         ? HttpVersion(2, 0)
//         : version == WebURLResponse::HTTPVersion_1_1
//         ? HttpVersion(1, 1)
//         : version == WebURLResponse::HTTPVersion_1_0
//         ? HttpVersion(1, 0)
//         : version == WebURLResponse::HTTPVersion_0_9
//         ? HttpVersion(0, 9)
//         : HttpVersion();
    if (code != kHttpOK && code != kHttpPartialContent)
        reasons |= kNoData;
    if (version < WebURLResponse::HTTP_1_1 && code == kHttpPartialContent)
        reasons |= kPre11PartialResponse;
//     if (code == kHttpPartialContent &&
//         !net::HttpUtil::HasStrongValidators(
//             http_version,
//             response.httpHeaderField("etag").utf8(),
//             response.httpHeaderField("Last-Modified").utf8(),
//             response.httpHeaderField("Date").utf8())) {
//         reasons |= kNoStrongValidatorOnPartialResponse;
//     }

    WTF::String cacheControl = response.httpHeaderField("cache-control");
    cacheControl = cacheControl.lower();

    std::string cache_control_header((char*)cacheControl.characters8(), cacheControl.length());
    if (cache_control_header.find("no-cache") != std::string::npos)
        reasons |= kNoCache;
    if (cache_control_header.find("no-store") != std::string::npos)
        reasons |= kNoStore;
    if (cache_control_header.find("must-revalidate") != std::string::npos)
        reasons |= kHasMustRevalidate;

//     const TimeDelta kMinimumAgeForUsefulness = TimeDelta::FromSeconds(3600);  // Arbitrary value.
// 
//     const char kMaxAgePrefix[] = "max-age=";
//     const size_t kMaxAgePrefixLen = arraysize(kMaxAgePrefix) - 1;
//     if (cache_control_header.substr(0, kMaxAgePrefixLen) == kMaxAgePrefix) {
//         int64 max_age_seconds;
//         base::StringToInt64(base::StringPiece(cache_control_header.begin() + kMaxAgePrefixLen, cache_control_header.end()), &max_age_seconds);
//         if (TimeDelta::FromSeconds(max_age_seconds) < kMinimumAgeForUsefulness)
//             reasons |= kShortMaxAge;
//     }
// 
//     Time date;
//     Time expires;
//     if (Time::FromString(response.httpHeaderField("Date").utf8().data(), &date) &&
//         Time::FromString(response.httpHeaderField("Expires").utf8().data(), &expires) &&
//         date > Time() && expires > Time() && (expires - date) < kMinimumAgeForUsefulness) {
//         reasons |= kExpiresTooSoon;
//     }

    return reasons;
}

// Returns the integer i such as 2^i <= n < 2^(i+1)
inline int Log2Floor(uint32 n)
{
    if (n == 0)
        return -1;
    int log = 0;
    uint32 value = n;
    for (int i = 4; i >= 0; --i) {
        int shift = (1 << i);
        uint32 x = value >> shift;
        if (x != 0) {
            value = x;
            log += shift;
        }
    }
    //DCHECK_EQ(value, 1u);
    return log;
}

// Returns the integer i such as 2^(i-1) < n <= 2^i
inline int Log2Ceiling(uint32 n)
{
    if (n == 0) {
        return -1;
    } else {
        // Log2Floor returns -1 for 0, so the following works correctly for n=1.
        return 1 + Log2Floor(n - 1);
    }
}

BufferedResourceLoader::BufferedResourceLoader(
    const blink::KURL& url,
    CORSMode cors_mode,
    int64 first_byte_position,
    int64 last_byte_position,
    DeferStrategy strategy,
    int bitrate,
    double playback_rate,
    MediaLog* media_log)
    : m_buffer(kMinBufferCapacity, kMinBufferCapacity)
    , m_loaderFailed(false)
    , m_deferStrategy(strategy)
    , m_mightBeReusedFromCacheIinFuture(true)
    , m_rangeSsupported(false)
    , m_savedForwardCapacity(0)
    , m_url(url)
    , m_corsMmode(cors_mode)
    , m_firstBytePosition(first_byte_position)
    , m_lastBytePosition(last_byte_position)
    , m_singleOrigin(true)
    , m_offset(0)
    , m_contentLength(kPositionNotSpecified)
    , m_instanceSize(kPositionNotSpecified)
    , m_readPosition(0)
    , m_readSize(0)
    , m_readBuffer(NULL)
    , m_firstOffset(0)
    , m_lastOffset(0)
    , m_bitrate(bitrate)
    , m_playbackRate(playback_rate)
    //, media_log_(media_log)
    , m_cancelUponDeferral(false)
{
    m_id = net::ActivatingObjCheck::inst()->genId();
    net::ActivatingObjCheck::inst()->add(m_id);
    // Set the initial capacity of |m_buffer| based on |m_bitrate| and
    // |m_playbackRate|.
    UpdateBufferWindow();
}

BufferedResourceLoader::~BufferedResourceLoader() 
{
    net::ActivatingObjCheck::inst()->remove(m_id);
}

void BufferedResourceLoader::Start(StartCB&& start_cb, LoadingStateChangedCB&& loading_cb, ProgressCB&& progress_cb, WebFrame* frame)
{
    // Make sure we have not started.
//     ASSERT(m_startCb.is_null());
//     ASSERT(m_loadingCb.is_null());
//     ASSERT(m_progressCb.is_null());
//     ASSERT(!start_cb.is_null());
//     ASSERT(!loading_cb.is_null());
//     ASSERT(!progress_cb.is_null());
    ASSERT(frame);

    m_startCb.reset(new StartCB(std::move(start_cb)));
    m_loadingCb.reset(new LoadingStateChangedCB(std::move(loading_cb)));
    m_progressCb.reset(new ProgressCB(std::move(progress_cb)));

    if (m_firstBytePosition != kPositionNotSpecified) {
        // TODO(hclam): server may not support range request so |m_offset| may not
        // equal to |m_firstBytePosition|.
        m_offset = m_firstBytePosition;
    }

    // Prepare the request.
    WebURLRequest request(m_url);
    // TODO(mkwst): Split this into video/audio.
    request.setRequestContext(WebURLRequest::RequestContextVideo);

    if (IsRangeRequest()) {
        std::vector<char> httpByteRange;
        httpByteRange.resize(300);

        if (m_lastBytePosition == kPositionNotSpecified)
            sprintf(httpByteRange.data(), "bytes=%I64d-", m_firstBytePosition);
        else
            sprintf(httpByteRange.data(), "bytes=%I64d-%I64d", m_firstBytePosition, m_lastBytePosition);
//         OutputDebugStringA(httpByteRange.data());
//         OutputDebugStringA("\n");

        request.setHTTPHeaderField(WebString::fromUTF8("Range"), WebString::fromUTF8(httpByteRange.data()));
    }

    frame->setReferrerForRequest(request, blink::WebURL());

    // Disable compression, compression for audio/video doesn't make sense...
    request.setHTTPHeaderField(WebString::fromUTF8("Accept-Encoding"), WebString::fromUTF8("identity;q=1, *;q=0"));

    WebURLLoaderOptions options;
    if (m_corsMmode == kUnspecified) {
        options.allowCredentials = true;
        options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyAllow;
    } else {
        options.exposeAllResponseHeaders = true;
        // The author header set is empty, no preflight should go ahead.
        options.preflightPolicy = WebURLLoaderOptions::PreventPreflight;
        options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
        if (m_corsMmode == kUseCredentials)
            options.allowCredentials = true;
    }
    WebURLLoader* loader = (frame->createAssociatedURLLoader(options));

    // Start the resource loading.
    loader->loadAsynchronously(request, this);
    m_activeLoader.reset(new ActiveLoader(loader));
    (*m_loadingCb)(kLoading);
}

void BufferedResourceLoader::Stop()
{
    // Reset callbacks.
    m_startCb.reset();
    m_loadingCb.reset();
    m_progressCb.reset();
    m_readCb.reset();

    // Cancel and reset any active loaders.
    m_activeLoader.reset();
}

void BufferedResourceLoader::Read(
    int64 position,
    int read_size,
    uint8* buffer,
    ReadCB&& read_cb)
{
    ASSERT(!m_startCb.get());
    ASSERT(!m_readCb.get());
    //ASSERT(!read_cb.is_null());
    ASSERT(buffer);
    ASSERT(read_size > 0);

      // Save the parameter of reading.
    m_readCb.reset(new ReadCB(std::move(read_cb)));
    m_readPosition = position;
    m_readSize = read_size;
    m_readBuffer = buffer;

    //OutputDebugStringA("BufferedResourceLoader::Read\n");

    // Reads should immediately fail if the loader also failed.
    if (m_loaderFailed) {
        DoneRead(kFailed, 0);
        return;
    }

    // If we're attempting to read past the end of the file, return a zero
    // indicating EOF.
    //
    // This can happen with callees that read in fixed-sized amounts for parsing
    // or at the end of chunked 200 responses when we discover the actual length
    // of the file.
    if (m_instanceSize != kPositionNotSpecified &&
        m_instanceSize <= m_readPosition) {
        //DVLOG(1) << "Appear to have seeked beyond EOS; returning 0.";
        DoneRead(kOk, 0);
        return;
    }

    // Make sure |m_offset| and |m_readPosition| does not differ by a large
    // amount.
    if (m_readPosition > m_offset + kint32max ||
        m_readPosition < m_offset + kint32min) {
        DoneRead(kCacheMiss, 0);
        return;
    }

    // Make sure |m_readSize| is not too large for the buffer to ever be able to
    // fulfill the read request.
    if (m_readSize > kMaxBufferCapacity) {
        DoneRead(kFailed, 0);
        return;
    }

    // Prepare the parameters.
    m_firstOffset = m_readPosition - m_offset;
    m_lastOffset = m_firstOffset + m_readSize;

    // If we can serve the request now, do the actual read.
    if (CanFulfillRead()) {
        ReadInternal();
        UpdateDeferBehavior();
        return;
    }

    // If we expect the read request to be fulfilled later, expand capacity as
    // necessary and disable deferring.
    if (WillFulfillRead()) {
        // Advance offset as much as possible to create additional capacity.
        int advance = std::min(m_firstOffset, m_buffer.forward_bytes());
        bool ret = m_buffer.Seek(advance);
        ASSERT(ret);

        m_offset += advance;
        m_firstOffset -= advance;
        m_lastOffset -= advance;

        // Expand capacity to accomodate a read that extends past the normal
        // capacity.
        //
        // This can happen when reading in a large seek index or when the
        // first byte of a read request falls within kForwardWaitThreshold.
        if (m_lastOffset > m_buffer.forward_capacity()) {
            m_savedForwardCapacity = m_buffer.forward_capacity();
            m_buffer.set_forward_capacity(m_lastOffset);
        }

        // Make sure we stop deferring now that there's additional capacity.
        ASSERT(!ShouldDefer());// << "Capacity was not adjusted properly to prevent deferring.";
        UpdateDeferBehavior();

        return;
    }

    // Make a callback to report failure.
    DoneRead(kCacheMiss, 0);
}

int64 BufferedResourceLoader::content_length()
{
    return m_contentLength;
}

int64 BufferedResourceLoader::instance_size()
{
    return m_instanceSize;
}

bool BufferedResourceLoader::range_supported()
{
    return m_rangeSsupported;
}

/////////////////////////////////////////////////////////////////////////////
// blink::WebURLLoaderClient implementation.
void BufferedResourceLoader::willSendRequest(WebURLLoader* loader, WebURLRequest& newRequest, const WebURLResponse& redirectResponse)
{
    // The load may have been stopped and |start_cb| is destroyed.
    // In this case we shouldn't do anything.
    if (!m_startCb.get()) {
        // Set the url in the request to an invalid value (empty url).
        newRequest.setURL(blink::WebURL());
        return;
    }

    // Only allow |m_singleOrigin| if we haven't seen a different origin yet.
//     if (m_singleOrigin)
//         m_singleOrigin = m_url.GetOrigin() == blink::KURL(newRequest.url()).GetOrigin();

    m_url = newRequest.url();
}

void BufferedResourceLoader::didSendData(
    WebURLLoader* loader,
    unsigned long long bytes_sent,
    unsigned long long total_bytes_to_be_sent)
{

}

void BufferedResourceLoader::didReceiveResponse(WebURLLoader* loader, const WebURLResponse& response)
{
//     DVLOG(1) << "didReceiveResponse: HTTP/"
//         << (response.httpVersion() == WebURLResponse::HTTPVersion_0_9
//             ? "0.9"
//             : response.httpVersion() == WebURLResponse::HTTPVersion_1_0
//             ? "1.0"
//             : response.httpVersion() ==
//             WebURLResponse::HTTPVersion_1_1
//             ? "1.1"
//             : response.httpVersion() ==
//             WebURLResponse::HTTPVersion_2_0
//             ? "2.0"
//             : "Unknown")
//         << " " << response.httpStatusCode();
    ASSERT(m_activeLoader.get());
    m_responseOriginalUrl = response.wasFetchedViaServiceWorker()
        ? response.originalURLViaServiceWorker()
        : response.url();

    // The loader may have been stopped and |start_cb| is destroyed.
    // In this case we shouldn't do anything.
    if (!m_startCb.get())
        return;

    uint32 reasons = GetReasonsForUncacheability(response);
    m_mightBeReusedFromCacheIinFuture = reasons == 0;
    //UMA_HISTOGRAM_BOOLEAN("Media.CacheUseful", reasons == 0);
    int shift = 0;
    int max_enum = Log2Ceiling(kMaxReason);
    while (reasons) {
//         DCHECK_LT(shift, max_enum);  // Sanity check.
//         if (reasons & 0x1) {
//             UMA_HISTOGRAM_ENUMERATION("Media.UncacheableReason", shift, max_enum); // PRESUBMIT_IGNORE_UMA_MAX
//         }

        reasons >>= 1;
        ++shift;
    }

    // Expected content length can be |kPositionNotSpecified|, in that case
    // |m_contentLength| is not specified and this is a streaming response.
    m_contentLength = response.expectedContentLength();

    // We make a strong assumption that when we reach here we have either
    // received a response from HTTP/HTTPS protocol or the request was
    // successful (in particular range request). So we only verify the partial
    // response for HTTP and HTTPS protocol.
    if (m_url.protocolIsInHTTPFamily()) {
        bool partial_response = (response.httpStatusCode() == kHttpPartialContent);
        bool ok_response = (response.httpStatusCode() == kHttpOK);

        if (IsRangeRequest()) {
            // Check to see whether the server supports byte ranges.
            std::string accept_ranges = response.httpHeaderField("Accept-Ranges").utf8();
            m_rangeSsupported = (accept_ranges.find("bytes") != std::string::npos);

            // If we have verified the partial response and it is correct, we will
            // return kOk. It's also possible for a server to support range requests
            // without advertising "Accept-Ranges: bytes".
            if (partial_response && VerifyPartialResponse(response)) {
                m_rangeSsupported = true;
            } else if (ok_response && m_firstBytePosition == 0 && m_lastBytePosition == kPositionNotSpecified) {
                // We accept a 200 response for a Range:0- request, trusting the
                // Accept-Ranges header, because Apache thinks that's a reasonable thing
                // to return.
                m_instanceSize = m_contentLength;
            } else {
//                 MEDIA_LOG(ERROR, media_log_)m<< "Failed loading buffered resource using range request due to invalid server response. HTTP status code="
//                     << response.httpStatusCode();
                DoneStart(kFailed);
                return;
            }
        } else {
            m_instanceSize = m_contentLength;
            if (response.httpStatusCode() != kHttpOK) {
                // We didn't request a range but server didn't reply with "200 OK".
//                 MEDIA_LOG(ERROR, media_log_)<< "Failed loading buffered resource due to invalid server response. HTTP status code=" << response.httpStatusCode();
                DoneStart(kFailed);
                return;
            }
        }

    } else {
        //CHECK_EQ(m_instanceSize, kPositionNotSpecified);
        if (m_contentLength != kPositionNotSpecified) {
            if (m_firstBytePosition == kPositionNotSpecified)
                m_instanceSize = m_contentLength;
            else if (m_lastBytePosition == kPositionNotSpecified)
                m_instanceSize = m_contentLength + m_firstBytePosition;
        }
    }

    // Calls with a successful response.
    DoneStart(kOk);
}

// std::vector<char>* g_buffer = nullptr;
// 
// int getBufPos(const char* data, int data_length)
// {
// 	char* ptr = &g_buffer->at(0);
// 	for (int i = 0; i < (int)g_buffer->size(); ++i) {
// 		char* ptr3 = &g_buffer->at(i);
// 		if (0 == memcmp(ptr3, data, data_length)) {
//             return i;
// 		}
// 	}
//     return -1;
// }

void BufferedResourceLoader::didReceiveData(
    WebURLLoader* loader,
    const char* data,
    int data_length,
    int encoded_data_length)
{
//     if (!g_buffer) {
// 		g_buffer = new std::vector<char>();
// 
// 		HANDLE hFile = CreateFileW(L"E:\\test\\web_test\\icoc\\video.webm", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
// 		DWORD fileSizeHigh;
// 		const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);
// 
// 		DWORD numberOfBytesRead = 0;
// 		g_buffer->resize(bufferSize);
// 		BOOL b = ::ReadFile(hFile, &g_buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
// 		::CloseHandle(hFile);
//     }

//     int pos = getBufPos(data, data_length);
// 
// 	char* output = (char*)malloc(0x100);
// 	sprintf(output, "BufferedResourceLoader::didReceiveData: %d\n", pos);
// 	OutputDebugStringA(output);
// 	free(output);

//     DVLOG(1) << "didReceiveData: " << data_length << " bytes";
    ASSERT(m_activeLoader.get());
    ASSERT(data_length > 0);

    m_buffer.Append(reinterpret_cast<const uint8*>(data), data_length);

    // If there is an active read request, try to fulfill the request.
    if (HasPendingRead() && CanFulfillRead())
        ReadInternal();

    // At last see if the buffer is full and we need to defer the downloading.
    UpdateDeferBehavior();

    // Consume excess bytes from our in-memory buffer if necessary.
    if (m_buffer.forward_bytes() > m_buffer.forward_capacity()) {
        int excess = m_buffer.forward_bytes() - m_buffer.forward_capacity();
        bool success = m_buffer.Seek(excess);
        ASSERT(success);
        m_offset += m_firstOffset + excess;
    }

    // Notify latest progress and buffered offset.
    (*m_progressCb)(m_offset + m_buffer.forward_bytes() - 1);
    Log();
}

void BufferedResourceLoader::didDownloadData(
    blink::WebURLLoader* loader,
    int dataLength,
    int encoded_data_length)
{
    DebugBreak();
}

void BufferedResourceLoader::didReceiveCachedMetadata(
    WebURLLoader* loader,
    const char* data,
    int data_length)
{
    DebugBreak();
}

void BufferedResourceLoader::didFinishLoading(WebURLLoader* loader, double finishTime, int64_t total_encoded_data_length)
{
    ASSERT(m_activeLoader.get());

    // We're done with the loader.
    m_activeLoader.reset();
    (*m_loadingCb)(kLoadingFinished);

    // If we didn't know the |m_instanceSize| we do now.
    if (m_instanceSize == kPositionNotSpecified) {
        m_instanceSize = m_offset + m_buffer.forward_bytes();
    }

    // If there is a start callback, run it.
    if (!!m_startCb.get()) {
        ASSERT(!m_readCb.get());// << "Shouldn't have a read callback during start";
        DoneStart(kOk);
        return;
    }

    // Don't leave read callbacks hanging around.
    if (HasPendingRead()) {
        // Try to fulfill with what is in the buffer.
        if (CanFulfillRead())
            ReadInternal();
        else
            DoneRead(kCacheMiss, 0);
    }
}

void BufferedResourceLoader::didFail(WebURLLoader* loader, const WebURLError& error)
{
//     DVLOG(1) << "didFail: reason=" << error.reason
//         << ", isCancellation=" << error.isCancellation
//         << ", domain=" << error.domain.utf8().data()
//         << ", localizedDescription="
//         << error.localizedDescription.utf8().data();
    ASSERT(m_activeLoader.get());
//     MEDIA_LOG(ERROR, media_log_) << "Failed loading buffered resource. Error code=" << error.reason;

    // We don't need to continue loading after failure.
    //
    // Keep it alive until we exit this method so that |error| remains valid.
    std::unique_ptr<ActiveLoader> active_loader;
    active_loader.reset(m_activeLoader.release());

    m_loaderFailed = true;
    (*m_loadingCb)(kLoadingFailed);

    // Don't leave start callbacks hanging around.
    if (!!m_startCb.get()) {
        ASSERT(!m_readCb.get());// << "Shouldn't have a read callback during start";
        DoneStart(kFailed);
        return;
    }

    // Don't leave read callbacks hanging around.
    if (HasPendingRead()) {
        DoneRead(kFailed, 0);
    }
}

bool BufferedResourceLoader::HasSingleOrigin() const
{
    ASSERT(!m_startCb.get());// << "Start() must complete before calling HasSingleOrigin()";
    DebugBreak();
    return m_singleOrigin;
}

bool BufferedResourceLoader::DidPassCORSAccessCheck() const
{
    // Until Start() is done we don't know, assume no until we know.
    if (!!m_startCb.get())
        return false;
    return !m_loaderFailed && m_corsMmode != kUnspecified;
}

void BufferedResourceLoader::UpdateDeferStrategy(DeferStrategy strategy)
{
    if (!m_mightBeReusedFromCacheIinFuture && strategy == kNeverDefer)
        strategy = kCapacityDefer;
    m_deferStrategy = strategy;
    UpdateDeferBehavior();
}

void BufferedResourceLoader::SetPlaybackRate(double playback_rate)
{
    m_playbackRate = playback_rate;

    // This is a pause so don't bother updating the buffer window as we'll likely
    // get unpaused in the future.
    if (m_playbackRate == 0.0)
        return;

    // Abort any cancellations in progress if playback starts.
    if (m_playbackRate > 0 && m_cancelUponDeferral)
        m_cancelUponDeferral = false;

    UpdateBufferWindow();
}

void BufferedResourceLoader::SetBitrate(int bitrate)
{
    ASSERT(bitrate >= 0);
    m_bitrate = bitrate;
    UpdateBufferWindow();
}

/////////////////////////////////////////////////////////////////////////////
// Helper methods.

void BufferedResourceLoader::UpdateBufferWindow()
{
    int backward_capacity;
    int forward_capacity;
    ComputeTargetBufferWindow(m_playbackRate, m_bitrate, &backward_capacity, &forward_capacity);

    // This does not evict data from the buffer if the new capacities are less
    // than the current capacities; the new limits will be enforced after the
    // existing excess buffered data is consumed.
    m_buffer.set_backward_capacity(backward_capacity);
    m_buffer.set_forward_capacity(forward_capacity);
}

void BufferedResourceLoader::UpdateDeferBehavior()
{
    if (!m_activeLoader)
        return;

    SetDeferred(ShouldDefer());
}

void BufferedResourceLoader::SetDeferred(bool deferred)
{
    if (m_activeLoader->deferred() == deferred)
        return;

    m_activeLoader->SetDeferred(deferred);
    (*m_loadingCb)(deferred ? kLoadingDeferred : kLoading);

    if (deferred && m_cancelUponDeferral)
        CancelUponDeferral();
}

bool BufferedResourceLoader::ShouldDefer() const
{
    switch (m_deferStrategy) {
    case kNeverDefer:
        return false;

    case kReadThenDefer:
        ASSERT(!m_readCb.get() || m_lastOffset > m_buffer.forward_bytes());// << "We shouldn't stop deferring if we can fulfill the read";
        return !m_readCb.get();

    case kCapacityDefer:
        return m_buffer.forward_bytes() >= m_buffer.forward_capacity();
    }
    DebugBreak();
    return false;
}

bool BufferedResourceLoader::CanFulfillRead() const
{
    // If we are reading too far in the backward direction.
    if (m_firstOffset < 0 && (m_firstOffset + m_buffer.backward_bytes()) < 0)
        return false;

    // If the start offset is too far ahead.
    if (m_firstOffset >= m_buffer.forward_bytes())
        return false;

    // At the point, we verified that first byte requested is within the buffer.
    // If the request has completed, then just returns with what we have now.
    if (!m_activeLoader)
        return true;

    // If the resource request is still active, make sure the whole requested
    // range is covered.
    if (m_lastOffset > m_buffer.forward_bytes())
        return false;

    return true;
}

bool BufferedResourceLoader::WillFulfillRead() const
{
    // Trying to read too far behind.
    if (m_firstOffset < 0 && (m_firstOffset + m_buffer.backward_bytes()) < 0)
        return false;

    // Trying to read too far ahead.
    if ((m_firstOffset - m_buffer.forward_bytes()) >= kForwardWaitThreshold)
        return false;

    // The resource request has completed, there's no way we can fulfill the
    // read request.
    if (!m_activeLoader)
        return false;

    return true;
}

void BufferedResourceLoader::ReadInternal()
{
    // Seek to the first byte requested.
    bool ret = m_buffer.Seek(m_firstOffset);
    ASSERT(ret);

    // Then do the read.
    int read = m_buffer.Read(m_readBuffer, m_readSize);
    m_offset += m_firstOffset + read;

    // And report with what we have read.
    DoneRead(kOk, read);
}

int64 BufferedResourceLoader::first_byte_position() const
{
    return m_firstBytePosition;
}

bool stringToInt64(const std::string& input, int64* output)
{
    __int64 result = _atoi64(input.c_str());
    *output = result;
    return true;
}

// static
bool BufferedResourceLoader::ParseContentRange(
    const std::string& content_range_str, int64* first_byte_position,
    int64* last_byte_position, int64* instance_size)
{
    const std::string kUpThroughBytesUnit = "bytes ";
    if (content_range_str.find(kUpThroughBytesUnit) != 0)
        return false;
    std::string range_spec = content_range_str.substr(kUpThroughBytesUnit.length());
    size_t dash_offset = range_spec.find("-");
    size_t slash_offset = range_spec.find("/");

    if (dash_offset == std::string::npos || slash_offset == std::string::npos ||
        slash_offset < dash_offset || slash_offset + 1 == range_spec.length()) {
        return false;
    }
    if (!stringToInt64(range_spec.substr(0, dash_offset), first_byte_position) ||
        !stringToInt64(range_spec.substr(dash_offset + 1, slash_offset - dash_offset - 1),
                             last_byte_position)) {
        return false;
    }
    if (slash_offset == range_spec.length() - 2 && range_spec[slash_offset + 1] == '*') {
        *instance_size = kPositionNotSpecified;
    } else {
        if (!stringToInt64(range_spec.substr(slash_offset + 1), instance_size)) {
            return false;
        }
    }
    if (*last_byte_position < *first_byte_position || (*instance_size != kPositionNotSpecified && *last_byte_position >= *instance_size)) {
        return false;
    }

    return true;
}

void BufferedResourceLoader::CancelUponDeferral()
{
    m_cancelUponDeferral = true;
    if (m_activeLoader && m_activeLoader->deferred())
        m_activeLoader.reset();
}

int64_t BufferedResourceLoader::GetMemoryUsage() const
{
    return m_buffer.forward_bytes() + m_buffer.backward_bytes();
}

bool BufferedResourceLoader::VerifyPartialResponse(const WebURLResponse& response)
{
    int64 first_byte_position, last_byte_position, instance_size;
    if (!ParseContentRange(response.httpHeaderField("Content-Range").utf8(), &first_byte_position, &last_byte_position, &instance_size)) {
        return false;
    }

    if (instance_size != kPositionNotSpecified) {
        m_instanceSize = instance_size;
    }

    if (m_firstBytePosition != kPositionNotSpecified &&
        m_firstBytePosition != first_byte_position) {
        return false;
    }

    // TODO(hclam): I should also check |last_byte_position|, but since
    // we will never make such a request that it is ok to leave it unimplemented.
    return true;
}

void BufferedResourceLoader::DoneRead(Status status, int bytes_read)
{
    if (m_savedForwardCapacity) {
        m_buffer.set_forward_capacity(m_savedForwardCapacity);
        m_savedForwardCapacity = 0;
    }
    m_readPosition = 0;
    m_readSize = 0;
    m_readBuffer = NULL;
    m_firstOffset = 0;
    m_lastOffset = 0;
    Log();

    ReadCB* read_cb = m_readCb.release();
    (*read_cb)(status, bytes_read); // -> BufferedDataSource::ReadCallback, may be destroy myself

    content::postTaskToMainThread(FROM_HERE, [read_cb] {
        delete read_cb;
    });
}

void BufferedResourceLoader::DoneStart(Status status)
{
    StartCB* startCb = m_startCb.release();
    (*startCb)(status);
}

bool BufferedResourceLoader::IsRangeRequest() const
{
    return m_firstBytePosition != kPositionNotSpecified;
}

void BufferedResourceLoader::Log()
{
//     media_log_->AddEvent(
//         media_log_->CreateBufferedExtentsChangedEvent(
//             m_offset - m_buffer.backward_bytes(),
//             m_offset,
//             m_offset + m_buffer.forward_bytes()));
}

}  // namespace media
