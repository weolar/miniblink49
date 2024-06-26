// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/webmediaplayer_util.h"

#include <math.h>
#include <string>

#include "base/metrics/histogram.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/media_client.h"
#include "media/base/media_keys.h"
#include "third_party/WebKit/public/platform/WebMediaPlayerEncryptedMediaClient.h"

namespace media {

// Compile asserts shared by all platforms.

#define STATIC_ASSERT_MATCHING_ENUM(name)                                      \
    static_assert(static_cast<int>(blink::WebMediaPlayerEncryptedMediaClient:: \
                          MediaKeyErrorCode##name)                             \
            == static_cast<int>(MediaKeys::k##name##Error),                    \
        "mismatching enum values: " #name)
STATIC_ASSERT_MATCHING_ENUM(Unknown);
STATIC_ASSERT_MATCHING_ENUM(Client);
#undef STATIC_ASSERT_MATCHING_ENUM

blink::WebTimeRanges ConvertToWebTimeRanges(
    const Ranges<base::TimeDelta>& ranges)
{
    blink::WebTimeRanges result(ranges.size());
    for (size_t i = 0; i < ranges.size(); ++i) {
        result[i].start = ranges.start(i).InSecondsF();
        result[i].end = ranges.end(i).InSecondsF();
    }
    return result;
}

blink::WebMediaPlayer::NetworkState PipelineErrorToNetworkState(
    PipelineStatus error)
{
    DCHECK_NE(error, PIPELINE_OK);

    switch (error) {
    case PIPELINE_ERROR_NETWORK:
    case PIPELINE_ERROR_READ:
        return blink::WebMediaPlayer::NetworkStateNetworkError;

    // TODO(vrk): Because OnPipelineInitialize() directly reports the
    // NetworkStateFormatError instead of calling OnPipelineError(), I believe
    // this block can be deleted. Should look into it! (crbug.com/126070)
    case PIPELINE_ERROR_INITIALIZATION_FAILED:
    case PIPELINE_ERROR_COULD_NOT_RENDER:
    case PIPELINE_ERROR_URL_NOT_FOUND:
    case DEMUXER_ERROR_COULD_NOT_OPEN:
    case DEMUXER_ERROR_COULD_NOT_PARSE:
    case DEMUXER_ERROR_NO_SUPPORTED_STREAMS:
    case DECODER_ERROR_NOT_SUPPORTED:
        return blink::WebMediaPlayer::NetworkStateFormatError;

    case PIPELINE_ERROR_DECODE:
    case PIPELINE_ERROR_ABORT:
    case PIPELINE_ERROR_OPERATION_PENDING:
    case PIPELINE_ERROR_INVALID_STATE:
        return blink::WebMediaPlayer::NetworkStateDecodeError;

    case PIPELINE_OK:
        NOTREACHED() << "Unexpected status! " << error;
    }
    return blink::WebMediaPlayer::NetworkStateFormatError;
}

namespace {

    // Helper enum for reporting scheme histograms.
    enum URLSchemeForHistogram {
        kUnknownURLScheme,
        kMissingURLScheme,
        kHttpURLScheme,
        kHttpsURLScheme,
        kFtpURLScheme,
        kChromeExtensionURLScheme,
        kJavascriptURLScheme,
        kFileURLScheme,
        kBlobURLScheme,
        kDataURLScheme,
        kFileSystemScheme,
        kMaxURLScheme = kFileSystemScheme // Must be equal to highest enum value.
    };

    //     URLSchemeForHistogram URLScheme(const GURL& url)
    //     {
    //         if (!url.has_scheme())
    //             return kMissingURLScheme;
    //         if (url.SchemeIs("http"))
    //             return kHttpURLScheme;
    //         if (url.SchemeIs("https"))
    //             return kHttpsURLScheme;
    //         if (url.SchemeIs("ftp"))
    //             return kFtpURLScheme;
    //         if (url.SchemeIs("chrome-extension"))
    //             return kChromeExtensionURLScheme;
    //         if (url.SchemeIs("javascript"))
    //             return kJavascriptURLScheme;
    //         if (url.SchemeIs("file"))
    //             return kFileURLScheme;
    //         if (url.SchemeIs("blob"))
    //             return kBlobURLScheme;
    //         if (url.SchemeIs("data"))
    //             return kDataURLScheme;
    //         if (url.SchemeIs("filesystem"))
    //             return kFileSystemScheme;
    //         return kUnknownURLScheme;
    //     }

    std::string LoadTypeToString(blink::WebMediaPlayer::LoadType load_type)
    {
        switch (load_type) {
        case blink::WebMediaPlayer::LoadTypeURL:
            return "SRC";
        case blink::WebMediaPlayer::LoadTypeMediaSource:
            return "MSE";
        case blink::WebMediaPlayer::LoadTypeMediaStream:
            return "MS";
        }

        NOTREACHED();
        return "Unknown";
    }

} // namespace

// TODO(xhwang): Call this from WebMediaPlayerMS to report metrics for
// MediaStream as well.
void ReportMetrics(blink::WebMediaPlayer::LoadType load_type,
    const blink::WebURL& url,
    const blink::WebURL& origin_url)
{
    //     // Report URL scheme, such as http, https, file, blob etc.
    //     UMA_HISTOGRAM_ENUMERATION("Media.URLScheme", URLScheme(url),
    //         kMaxURLScheme + 1);
    //
    //     // Report load type, such as URL, MediaSource or MediaStream.
    //     UMA_HISTOGRAM_ENUMERATION("Media.LoadType", load_type,
    //         blink::WebMediaPlayer::LoadTypeMax + 1);
    //
    //     // Report the origin from where the media player is created.
    //     if (GetMediaClient()) {
    //         GetMediaClient()->RecordRapporURL(
    //             "Media.OriginUrl." + LoadTypeToString(load_type), origin_url);
    //     }
    DebugBreak();
}

void RecordOriginOfHLSPlayback(const blink::WebURL& origin_url)
{
    //     if (media::GetMediaClient())
    //         GetMediaClient()->RecordRapporURL("Media.OriginUrl.HLS", origin_url);
    DebugBreak();
}

EmeInitDataType ConvertToEmeInitDataType(
    blink::WebEncryptedMediaInitDataType init_data_type)
{
    switch (init_data_type) {
    case blink::WebEncryptedMediaInitDataType::Webm:
        return EmeInitDataType::WEBM;
    case blink::WebEncryptedMediaInitDataType::Cenc:
        return EmeInitDataType::CENC;
    case blink::WebEncryptedMediaInitDataType::Keyids:
        return EmeInitDataType::KEYIDS;
    case blink::WebEncryptedMediaInitDataType::Unknown:
        return EmeInitDataType::UNKNOWN;
    }

    NOTREACHED();
    return EmeInitDataType::UNKNOWN;
}

blink::WebEncryptedMediaInitDataType ConvertToWebInitDataType(
    EmeInitDataType init_data_type)
{
    switch (init_data_type) {
    case EmeInitDataType::WEBM:
        return blink::WebEncryptedMediaInitDataType::Webm;
    case EmeInitDataType::CENC:
        return blink::WebEncryptedMediaInitDataType::Cenc;
    case EmeInitDataType::KEYIDS:
        return blink::WebEncryptedMediaInitDataType::Keyids;
    case EmeInitDataType::UNKNOWN:
        return blink::WebEncryptedMediaInitDataType::Unknown;
    }

    NOTREACHED();
    return blink::WebEncryptedMediaInitDataType::Unknown;
}

namespace {
    // This class wraps a scoped blink::WebSetSinkIdCallbacks pointer such that
    // copying objects of this class actually performs moving, thus
    // maintaining clear ownership of the blink::WebSetSinkIdCallbacks pointer.
    // The rationale for this class is that the SwichOutputDevice method
    // can make a copy of its base::Callback parameter, which implies
    // copying its bound parameters.
    // SwitchOutputDevice actually wants to move its base::Callback
    // parameter since only the final copy will be run, but base::Callback
    // does not support move semantics and there is no base::MovableCallback.
    // Since scoped pointers are not copyable, we cannot bind them directly
    // to a base::Callback in this case. Thus, we use this helper class,
    // whose copy constructor transfers ownership of the scoped pointer.

    class SetSinkIdCallback {
    public:
        explicit SetSinkIdCallback(blink::WebSetSinkIdCallbacks* web_callback)
            : web_callback_(web_callback)
        {
        }
        SetSinkIdCallback(const SetSinkIdCallback& other)
            : web_callback_(other.web_callback_.Pass())
        {
        }
        ~SetSinkIdCallback() { }
        friend void RunSetSinkIdCallback(const SetSinkIdCallback& callback,
            OutputDeviceStatus result);

    private:
        // Mutable is required so that Pass() can be called in the copy
        // constructor.
        mutable scoped_ptr<blink::WebSetSinkIdCallbacks> web_callback_;
    };

    void RunSetSinkIdCallback(const SetSinkIdCallback& callback,
        OutputDeviceStatus result)
    {
        DebugBreak();
        if (!callback.web_callback_)
            return;

        //         switch (result) {
        //         case OUTPUT_DEVICE_STATUS_OK:
        //             callback.web_callback_->onSuccess();
        //             break;
        //         case OUTPUT_DEVICE_STATUS_ERROR_NOT_FOUND:
        //             callback.web_callback_->onError(blink::WebSetSinkIdError::ErrorTypeNotFound);
        //             break;
        //         case OUTPUT_DEVICE_STATUS_ERROR_NOT_AUTHORIZED:
        //             callback.web_callback_->onError(blink::WebSetSinkIdError::NotAuthorized);
        //             break;
        //         case OUTPUT_DEVICE_STATUS_ERROR_INTERNAL:
        //             callback.web_callback_->onError(blink::WebSetSinkIdError::ErrorTypeAbort);
        //             break;
        //         default:
        //             NOTREACHED();
        //         }

        callback.web_callback_ = nullptr;
    }

} // namespace

SwitchOutputDeviceCB ConvertToSwitchOutputDeviceCB(
    blink::WebSetSinkIdCallbacks* web_callbacks)
{
    return media::BindToCurrentLoop(
        base::Bind(RunSetSinkIdCallback, SetSinkIdCallback(web_callbacks)));
}

} // namespace media
