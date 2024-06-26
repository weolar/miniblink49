// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/video_frame_metadata.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"

namespace media {

namespace {

    // Map enum key to internal std::string key used by base::DictionaryValue.
    inline std::string ToInternalKey(VideoFrameMetadata::Key key)
    {
        DCHECK_LT(key, VideoFrameMetadata::NUM_KEYS);
        return base::IntToString(static_cast<int>(key));
    }

} // namespace

VideoFrameMetadata::VideoFrameMetadata() { }

VideoFrameMetadata::~VideoFrameMetadata() { }

bool VideoFrameMetadata::HasKey(Key key) const
{
    return dictionary_.HasKey(ToInternalKey(key));
}

void VideoFrameMetadata::SetBoolean(Key key, bool value)
{
    dictionary_.SetBooleanWithoutPathExpansion(ToInternalKey(key), value);
}

void VideoFrameMetadata::SetInteger(Key key, int value)
{
    dictionary_.SetIntegerWithoutPathExpansion(ToInternalKey(key), value);
}

void VideoFrameMetadata::SetDouble(Key key, double value)
{
    dictionary_.SetDoubleWithoutPathExpansion(ToInternalKey(key), value);
}

void VideoFrameMetadata::SetString(Key key, const std::string& value)
{
    dictionary_.SetWithoutPathExpansion(
        ToInternalKey(key),
        // Using BinaryValue since we don't want the |value| interpreted as having
        // any particular character encoding (e.g., UTF-8) by
        // base::DictionaryValue.
        base::BinaryValue::CreateWithCopiedBuffer(value.data(), value.size()));
}

namespace {
    template <class TimeType>
    void SetTimeValue(VideoFrameMetadata::Key key,
        const TimeType& value,
        base::DictionaryValue* dictionary)
    {
        const int64 internal_value = value.ToInternalValue();
        dictionary->SetWithoutPathExpansion(
            ToInternalKey(key),
            base::BinaryValue::CreateWithCopiedBuffer(
                reinterpret_cast<const char*>(&internal_value),
                sizeof(internal_value)));
    }
} // namespace

void VideoFrameMetadata::SetTimeDelta(Key key, const base::TimeDelta& value)
{
    SetTimeValue(key, value, &dictionary_);
}

void VideoFrameMetadata::SetTimeTicks(Key key, const base::TimeTicks& value)
{
    SetTimeValue(key, value, &dictionary_);
}

void VideoFrameMetadata::SetValue(Key key, scoped_ptr<base::Value> value)
{
    //dictionary_.SetWithoutPathExpansion(ToInternalKey(key), value.Pass());
    DebugBreak();
}

bool VideoFrameMetadata::GetBoolean(Key key, bool* value) const
{
    DCHECK(value);
    return dictionary_.GetBooleanWithoutPathExpansion(ToInternalKey(key), value);
}

bool VideoFrameMetadata::GetInteger(Key key, int* value) const
{
    DCHECK(value);
    return dictionary_.GetIntegerWithoutPathExpansion(ToInternalKey(key), value);
}

bool VideoFrameMetadata::GetDouble(Key key, double* value) const
{
    DCHECK(value);
    return dictionary_.GetDoubleWithoutPathExpansion(ToInternalKey(key), value);
}

bool VideoFrameMetadata::GetString(Key key, std::string* value) const
{
    DCHECK(value);
    const base::BinaryValue* const binary_value = GetBinaryValue(key);
    if (binary_value)
        value->assign(binary_value->GetBuffer(), binary_value->GetSize());
    return !!binary_value;
}

namespace {
    template <class TimeType>
    bool ToTimeValue(const base::BinaryValue& binary_value, TimeType* value)
    {
        DCHECK(value);
        int64 internal_value;
        if (binary_value.GetSize() != sizeof(internal_value))
            return false;
        memcpy(&internal_value, binary_value.GetBuffer(), sizeof(internal_value));
        *value = TimeType::FromInternalValue(internal_value);
        return true;
    }
} // namespace

bool VideoFrameMetadata::GetTimeDelta(Key key, base::TimeDelta* value) const
{
    const base::BinaryValue* const binary_value = GetBinaryValue(key);
    return binary_value && ToTimeValue(*binary_value, value);
}

bool VideoFrameMetadata::GetTimeTicks(Key key, base::TimeTicks* value) const
{
    const base::BinaryValue* const binary_value = GetBinaryValue(key);
    return binary_value && ToTimeValue(*binary_value, value);
}

const base::Value* VideoFrameMetadata::GetValue(Key key) const
{
    const base::Value* result = nullptr;
    if (!dictionary_.GetWithoutPathExpansion(ToInternalKey(key), &result))
        return nullptr;
    return result;
}

bool VideoFrameMetadata::IsTrue(Key key) const
{
    bool value = false;
    return GetBoolean(key, &value) && value;
}

void VideoFrameMetadata::MergeInternalValuesInto(
    base::DictionaryValue* out) const
{
    out->MergeDictionary(&dictionary_);
}

void VideoFrameMetadata::MergeInternalValuesFrom(
    const base::DictionaryValue& in)
{
    dictionary_.MergeDictionary(&in);
}

const base::BinaryValue* VideoFrameMetadata::GetBinaryValue(Key key) const
{
    const base::Value* internal_value = nullptr;
    if (dictionary_.GetWithoutPathExpansion(ToInternalKey(key),
            &internal_value)
        && internal_value->GetType() == base::Value::TYPE_BINARY) {
        return static_cast<const base::BinaryValue*>(internal_value);
    }
    return nullptr;
}

} // namespace media
