// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_LOGGING_RAW_EVENT_SUBSCRIBER_BUNDLE_H_
#define MEDIA_CAST_LOGGING_RAW_EVENT_SUBSCRIBER_BUNDLE_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "media/cast/logging/encoding_event_subscriber.h"
#include "media/cast/logging/stats_event_subscriber.h"

namespace media {
namespace cast {

    class CastEnvironment;
    class ReceiverTimeOffsetEstimator;

    // Allow 9MB for serialized video / audio event logs.
    const int kMaxSerializedBytes = 9000000;

    // Assume serialized log data for each frame will take up to 150 bytes.
    const int kMaxVideoEventEntries = kMaxSerializedBytes / 150;

    // Assume serialized log data for each frame will take up to 75 bytes.
    const int kMaxAudioEventEntries = kMaxSerializedBytes / 75;

    // A bundle for raw event subscribers for a single stream.
    // It contains an EncodingEventSubscriber and a StatsSubscriber.
    class RawEventSubscriberBundleForStream {
    public:
        RawEventSubscriberBundleForStream(
            const scoped_refptr<CastEnvironment>& cast_environment,
            bool is_audio,
            ReceiverTimeOffsetEstimator* offset_estimator);
        ~RawEventSubscriberBundleForStream();

        EncodingEventSubscriber* GetEncodingEventSubscriber();
        StatsEventSubscriber* GetStatsEventSubscriber();

    private:
        const scoped_refptr<CastEnvironment> cast_environment_;
        EncodingEventSubscriber event_subscriber_;
        StatsEventSubscriber stats_subscriber_;

        DISALLOW_COPY_AND_ASSIGN(RawEventSubscriberBundleForStream);
    };

    // A bundle of subscribers for all streams. An instance of this object
    // is associated with a CastEnvironment.
    // This class can be used for managing event subscribers
    // in a session where they could be multiple streams (i.e. CastSessionDelegate).
    // It also contains a ReceiverTimeOffsetEstimator that is shared by subscribers
    // of different streams.
    class RawEventSubscriberBundle {
    public:
        explicit RawEventSubscriberBundle(
            const scoped_refptr<CastEnvironment>& cast_environment);
        ~RawEventSubscriberBundle();

        void AddEventSubscribers(bool is_audio);
        void RemoveEventSubscribers(bool is_audio);
        EncodingEventSubscriber* GetEncodingEventSubscriber(
            bool is_audio);
        StatsEventSubscriber* GetStatsEventSubscriber(bool is_audio);

    private:
        // Map from (is_audio) -> RawEventSubscriberBundleForStream.
        // TODO(imcheng): This works because we only have 1 audio and 1 video stream.
        // This needs to scale better.
        typedef std::map<bool, linked_ptr<RawEventSubscriberBundleForStream>>
            SubscribersMapByStream;
        const scoped_refptr<CastEnvironment> cast_environment_;
        SubscribersMapByStream subscribers_;
        scoped_ptr<ReceiverTimeOffsetEstimator> receiver_offset_estimator_;

        DISALLOW_COPY_AND_ASSIGN(RawEventSubscriberBundle);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_LOGGING_RAW_EVENT_SUBSCRIBER_BUNDLE_H_
