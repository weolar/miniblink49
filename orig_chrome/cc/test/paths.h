// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_PATHS_H_
#define CC_TEST_PATHS_H_

namespace cc {

class CCPaths {
public:
    enum {
        PATH_START = 5000,

        // Valid only in development and testing environments.
        DIR_TEST_DATA,
        PATH_END
    };

    // Call once to register the provider for the path keys defined above.
    static void RegisterPathProvider();
};

} // namespace cc

#endif // CC_TEST_PATHS_H_
