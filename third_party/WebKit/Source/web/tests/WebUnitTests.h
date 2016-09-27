/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef WebUnitTests_h
#define WebUnitTests_h

#include "public/platform/WebCommon.h"

namespace blink {

// In a chromium multi-dll build, blink unittest code is compiled into
// blink_web.dll, since some of the tests cover unexported methods.
//
// While gtest does support running tests in libraries, the chromium gtest is
// not built with shared library support. As a result, if the test runner tries
// to instantiate the test suite outside of blink_web.dll, it won't correctly
// register the tests. In order to get around that, blink_web.dll exports this
// helper function to instantiate and run the test suite in the right module.
//
// Unfortunately, to make things more complicated, blink tests require some test
// support initialization in the content layer. Since the content layer depends
// on blink_web.dll, runWebTests() can't initialize it directly since that
// introduces a dependency cycle.
//
// To get around that, runWebTests() allows the caller to supply hooks to
// execute code before and after running tests.
BLINK_EXPORT int runWebTests(int argc, char** argv, void (*preTestHook)(void), void (*postTestHook)(void));

} // namespace blink

#endif
