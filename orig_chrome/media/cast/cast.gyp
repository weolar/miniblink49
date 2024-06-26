# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'include_tests%': 0,
    'chromium_code': 1,
  },
  'conditions': [
  ],
  'targets': [
    {
      # GN version: //media/cast:common
      'target_name': 'cast_base',
      'type': 'static_library',
      'include_dirs': [
        '<(DEPTH)/',
      ],
      'dependencies': [
        'cast_logging_proto',
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/crypto/crypto.gyp:crypto',
        '<(DEPTH)/net/net.gyp:net',
      ],
      'export_dependent_settings': [
        'cast_logging_proto',
      ],
      'sources': [
        'cast_config.cc',
        'cast_config.h',
        'cast_defines.h',
        'cast_environment.cc',
        'cast_environment.h',
        'common/clock_drift_smoother.cc',
        'common/clock_drift_smoother.h',
        'common/transport_encryption_handler.cc',
        'common/transport_encryption_handler.h',
        'logging/encoding_event_subscriber.cc',
        'logging/encoding_event_subscriber.h',
        'logging/log_deserializer.cc',
        'logging/log_deserializer.h',
        'logging/log_event_dispatcher.cc',
        'logging/log_event_dispatcher.h',
        'logging/log_serializer.cc',
        'logging/log_serializer.h',
        'logging/logging_defines.cc',
        'logging/logging_defines.h',
        'logging/raw_event_subscriber.h',
        'logging/raw_event_subscriber_bundle.cc',
        'logging/raw_event_subscriber_bundle.h',
        'logging/receiver_time_offset_estimator.h',
        'logging/receiver_time_offset_estimator_impl.cc',
        'logging/receiver_time_offset_estimator_impl.h',
        'logging/simple_event_subscriber.cc',
        'logging/simple_event_subscriber.h',
        'logging/stats_event_subscriber.cc',
        'logging/stats_event_subscriber.h',
      ], # source
    },
    {
      # GN version: //media/cast/logging/proto
      'target_name': 'cast_logging_proto',
      'type': 'static_library',
      'include_dirs': [
        '<(DEPTH)/',
      ],
      'sources': [
        'logging/proto/proto_utils.cc',
        'logging/proto/raw_events.proto',
      ],
      'variables': {
        'proto_in_dir': 'logging/proto',
        'proto_out_dir': 'media/cast/logging/proto',
      },
      'includes': ['../../build/protoc.gypi'],
    },
    {
      # GN version: //media/cast:receiver
      'target_name': 'cast_receiver',
      'type': 'static_library',
      'include_dirs': [
        '<(DEPTH)/',
      ],
      'dependencies': [
        'cast_base',
        'cast_net',
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/media/media.gyp:media',
        '<(DEPTH)/media/media.gyp:shared_memory_support',
        '<(DEPTH)/third_party/opus/opus.gyp:opus',
        '<(DEPTH)/third_party/libvpx_new/libvpx.gyp:libvpx_new',
        '<(DEPTH)/third_party/libyuv/libyuv.gyp:libyuv',
        '<(DEPTH)/ui/gfx/gfx.gyp:gfx_geometry',
      ],
      'sources': [
        'cast_receiver.h',
        'net/rtp/cast_message_builder.cc',
        'net/rtp/cast_message_builder.h',
        'net/rtp/frame_buffer.cc',
        'net/rtp/frame_buffer.h',
        'net/rtp/framer.cc',
        'net/rtp/framer.h',
        'net/rtp/receiver_stats.cc',
        'net/rtp/receiver_stats.h',
        'receiver/audio_decoder.cc',
        'receiver/audio_decoder.h',
        'receiver/cast_receiver_impl.cc',
        'receiver/cast_receiver_impl.h',
        'receiver/frame_receiver.cc',
        'receiver/frame_receiver.h',
        'receiver/video_decoder.cc',
        'receiver/video_decoder.h',
      ], # source
      'conditions': [
        # use a restricted subset of media and no software codecs on iOS
        ['OS=="ios"', {
          'dependencies': [ '<(DEPTH)/media/media.gyp:media_for_cast_ios' ],
          'dependencies!': [
            '<(DEPTH)/media/media.gyp:media',
            '<(DEPTH)/third_party/opus/opus.gyp:opus',
            '<(DEPTH)/third_party/libvpx_new/libvpx.gyp:libvpx_new',
          ],
        }], # OS=="ios"
      ], # conditions
    },
    {
      # GN version: //media/cast:sender
      'target_name': 'cast_sender',
      'type': 'static_library',
      'include_dirs': [
        '<(DEPTH)/',
      ],
      'dependencies': [
        'cast_base',
        'cast_net',
        '<(DEPTH)/media/media.gyp:media',
        '<(DEPTH)/media/media.gyp:shared_memory_support',
        '<(DEPTH)/third_party/opus/opus.gyp:opus',
        '<(DEPTH)/third_party/libvpx_new/libvpx.gyp:libvpx_new',
        '<(DEPTH)/ui/gfx/gfx.gyp:gfx_geometry',
      ], # dependencies
      'sources': [
        'cast_sender.h',
        'cast_sender_impl.cc',
        'cast_sender_impl.h',
        'sender/audio_encoder.cc',
        'sender/audio_encoder.h',
        'sender/audio_sender.cc',
        'sender/audio_sender.h',
        'sender/congestion_control.cc',
        'sender/congestion_control.h',
        'sender/external_video_encoder.cc',
        'sender/external_video_encoder.h',
        'sender/fake_software_video_encoder.cc',
        'sender/fake_software_video_encoder.h',
        'sender/frame_sender.cc',
        'sender/frame_sender.h',
        'sender/performance_metrics_overlay.cc',
        'sender/performance_metrics_overlay.h',
        'sender/sender_encoded_frame.cc',
        'sender/sender_encoded_frame.h',
        'sender/size_adaptable_video_encoder_base.cc',
        'sender/size_adaptable_video_encoder_base.h',
        'sender/software_video_encoder.h',
        'sender/video_encoder.cc',
        'sender/video_encoder.h',
        'sender/video_encoder_impl.cc',
        'sender/video_encoder_impl.h',
        'sender/video_frame_factory.h',
        'sender/video_sender.cc',
        'sender/video_sender.h',
        'sender/vp8_encoder.cc',
        'sender/vp8_encoder.h',
        'sender/vp8_quantizer_parser.h',
        'sender/vp8_quantizer_parser.cc',
      ], # source
      'conditions': [
        # use a restricted subset of media and no software codecs on iOS
        ['OS=="ios"', {
          'dependencies': [ '<(DEPTH)/media/media.gyp:media_for_cast_ios' ],
          'dependencies!': [
            '<(DEPTH)/media/media.gyp:media',
            '<(DEPTH)/third_party/opus/opus.gyp:opus',
            '<(DEPTH)/third_party/libvpx_new/libvpx.gyp:libvpx_new',
          ],
          'sources!': [
            'sender/external_video_encoder.cc',
            'sender/external_video_encoder.h',
            'sender/video_encoder_impl.cc',
            'sender/video_encoder_impl.h',
            'sender/vp8_encoder.cc',
            'sender/vp8_encoder.h',
            'sender/vp8_quantizer_parser.cc',
	    'sender/vp8_quantizer_parser.h',
          ],
        }], # OS=="ios"
        # iOS and OS X encoders
        ['OS=="ios" or OS=="mac"', {
          'sources': [
            'sender/h264_vt_encoder.cc',
            'sender/h264_vt_encoder.h',
          ],
          'link_settings': {
            'libraries': [
              '$(SDKROOT)/System/Library/Frameworks/AudioToolbox.framework',
              '$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework',
            ],
          },
        }],  # OS=="ios" or OS=="mac"
      ], # conditions
    },
    {
      # GN version: //media/cast:net
      'target_name': 'cast_net',
      'type': 'static_library',
      'include_dirs': [
        '<(DEPTH)/',
      ],
      'dependencies': [
        'cast_base',
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/net/net.gyp:net',
      ],
      'sources': [
        'net/cast_transport_config.cc',
        'net/cast_transport_config.h',
        'net/cast_transport_defines.h',
        'net/cast_transport_sender.h',
        'net/cast_transport_sender_impl.cc',
        'net/cast_transport_sender_impl.h',
        'net/pacing/paced_sender.cc',
        'net/pacing/paced_sender.h',
        'net/rtcp/receiver_rtcp_event_subscriber.cc',
        'net/rtcp/rtcp.cc',
        'net/rtcp/rtcp.h',
        'net/rtcp/rtcp_builder.cc',
        'net/rtcp/rtcp_builder.h',
        'net/rtcp/rtcp_defines.cc',
        'net/rtcp/rtcp_defines.h',
        'net/rtcp/rtcp_utility.cc',
        'net/rtcp/rtcp_utility.h',
        'net/rtp/packet_storage.cc',
        'net/rtp/packet_storage.h',
        'net/rtp/rtp_defines.cc',
        'net/rtp/rtp_defines.h',
        'net/rtp/rtp_packetizer.cc',
        'net/rtp/rtp_packetizer.h',
        'net/rtp/rtp_parser.cc',
        'net/rtp/rtp_parser.h',
        'net/rtp/rtp_sender.cc',
        'net/rtp/rtp_sender.h',
        'net/udp_transport.cc',
        'net/udp_transport.h',
      ], # source
    },
  ],
}
