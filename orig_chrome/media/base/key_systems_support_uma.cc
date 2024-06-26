// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/key_systems_support_uma.h"

#include "base/metrics/histogram.h"
#include "media/base/key_systems.h"

namespace media {

namespace {

    const char kKeySystemSupportUMAPrefix[] = "Media.EME.KeySystemSupport.";

    // These values are reported to UMA. Do not change the existing values!
    enum KeySystemSupportStatus {
        KEY_SYSTEM_QUERIED = 0,
        KEY_SYSTEM_SUPPORTED = 1,
        KEY_SYSTEM_WITH_TYPE_QUERIED = 2,
        KEY_SYSTEM_WITH_TYPE_SUPPORTED = 3,
        KEY_SYSTEM_SUPPORT_STATUS_COUNT
    };

    // Reports an event only once.
    class OneTimeReporter {
    public:
        OneTimeReporter(const std::string& key_system, KeySystemSupportStatus status);
        ~OneTimeReporter();

        void Report();

    private:
        bool is_reported_;
        const std::string key_system_;
        const KeySystemSupportStatus status_;
    };

    OneTimeReporter::OneTimeReporter(const std::string& key_system,
        KeySystemSupportStatus status)
        : is_reported_(false)
        , key_system_(key_system)
        , status_(status)
    {
    }

    OneTimeReporter::~OneTimeReporter() { }

    void OneTimeReporter::Report()
    {
        if (is_reported_)
            return;

        // Not using UMA_HISTOGRAM_ENUMERATION directly because UMA_* macros require
        // the names to be constant throughout the process' lifetime.
        //         base::LinearHistogram::FactoryGet(
        //             kKeySystemSupportUMAPrefix + GetKeySystemNameForUMA(key_system_), 1,
        //             KEY_SYSTEM_SUPPORT_STATUS_COUNT, KEY_SYSTEM_SUPPORT_STATUS_COUNT + 1,
        //             base::Histogram::kUmaTargetedHistogramFlag)
        //             ->Add(status_);

        is_reported_ = true;
    }

} // namespace

class KeySystemsSupportUMA::Reporter {
public:
    explicit Reporter(const std::string& key_system);
    ~Reporter();

    void Report(bool has_type, bool is_supported);

private:
    const std::string key_system_;

    OneTimeReporter call_reporter_;
    OneTimeReporter call_with_type_reporter_;
    OneTimeReporter support_reporter_;
    OneTimeReporter support_with_type_reporter_;
};

KeySystemsSupportUMA::Reporter::Reporter(const std::string& key_system)
    : key_system_(key_system)
    , call_reporter_(key_system, KEY_SYSTEM_QUERIED)
    , call_with_type_reporter_(key_system, KEY_SYSTEM_WITH_TYPE_QUERIED)
    , support_reporter_(key_system, KEY_SYSTEM_SUPPORTED)
    , support_with_type_reporter_(key_system, KEY_SYSTEM_WITH_TYPE_SUPPORTED)
{
}

KeySystemsSupportUMA::Reporter::~Reporter() { }

void KeySystemsSupportUMA::Reporter::Report(bool has_type, bool is_supported)
{
    call_reporter_.Report();
    if (has_type)
        call_with_type_reporter_.Report();

    if (!is_supported)
        return;

    support_reporter_.Report();
    if (has_type)
        support_with_type_reporter_.Report();
}

KeySystemsSupportUMA::KeySystemsSupportUMA() { }

KeySystemsSupportUMA::~KeySystemsSupportUMA() { }

void KeySystemsSupportUMA::AddKeySystemToReport(const std::string& key_system)
{
    DCHECK(!GetReporter(key_system));
    reporters_.set(key_system, scoped_ptr<Reporter>(new Reporter(key_system)));
}

void KeySystemsSupportUMA::ReportKeySystemQuery(const std::string& key_system,
    bool has_type)
{
    Reporter* reporter = GetReporter(key_system);
    if (!reporter)
        return;
    reporter->Report(has_type, false);
}

void KeySystemsSupportUMA::ReportKeySystemSupport(const std::string& key_system,
    bool has_type)
{
    Reporter* reporter = GetReporter(key_system);
    if (!reporter)
        return;
    reporter->Report(has_type, true);
}

KeySystemsSupportUMA::Reporter* KeySystemsSupportUMA::GetReporter(
    const std::string& key_system)
{
    Reporters::iterator reporter = reporters_.find(key_system);
    if (reporter == reporters_.end())
        return NULL;
    return reporter->second;
}

} // namespace media
