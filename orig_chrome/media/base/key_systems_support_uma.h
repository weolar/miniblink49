// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_KEY_SYSTEMS_SUPPORT_UMA_H_
#define MEDIA_BASE_KEY_SYSTEMS_SUPPORT_UMA_H_

#include <string>

#include "base/containers/scoped_ptr_hash_map.h"

namespace media {

// Key system support UMA statistics for queried key systems.
// 1. The key system is queried (with or without a MIME type).
// 2. The key system is queried with a MIME type.
// 3. The queried key system is supported (with or without a MIME type). This is
//    reported when the key system is supported when queried, regardless of
//    whether a MIME type is specified.
// 4. The queried key system is supported with a MIME type. This is reported
//    when the key system is supported when queried without a MIME type
//    specified.
// Note: All 4 stats are only reported once per renderer process per key system.
class KeySystemsSupportUMA {
public:
    KeySystemsSupportUMA();
    ~KeySystemsSupportUMA();

    // Adds a |key_system| for which query/support statistics are reported.
    // If you use this function to add key system to report, make sure to update
    // AddKeySystemSupportActions() in tools/metrics/actions/extract_actions.py.
    void AddKeySystemToReport(const std::string& key_system);

    // Reports that the |key_system| is queried. When |has_type|, also reports
    // that the |key_system| with a MIME type is queried.
    void ReportKeySystemQuery(const std::string& key_system, bool has_type);

    // Reports that the queried |key_system| is supported. When |has_type| (a
    // a MIME type is specified in the query), also reports that the queried
    // |key_system| is supported with that MIME type.
    void ReportKeySystemSupport(const std::string& key_system, bool has_type);

private:
    class Reporter;

    // Returns the Reporter for |key_system|. Returns NULL if |key_system| was not
    // added for UMA reporting.
    Reporter* GetReporter(const std::string& key_system);

    // Key system <-> Reporter map.
    typedef base::ScopedPtrHashMap<std::string, scoped_ptr<Reporter>> Reporters;
    Reporters reporters_;
};

} // namespace media

#endif // MEDIA_BASE_KEY_SYSTEMS_SUPPORT_UMA_H_
