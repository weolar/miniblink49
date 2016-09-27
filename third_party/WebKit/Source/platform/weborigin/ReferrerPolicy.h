/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
 *
 */

#ifndef ReferrerPolicy_h
#define ReferrerPolicy_h

namespace blink {

enum ReferrerPolicy {
    // https://w3c.github.io/webappsec/specs/referrer-policy/#referrer-policy-state-unsafe-url
    ReferrerPolicyAlways,
    // The default policy, if no policy is explicitly set by the page.
    ReferrerPolicyDefault,
    // https://w3c.github.io/webappsec/specs/referrer-policy/#referrer-policy-state-no-referrer-when-downgrade
    ReferrerPolicyNoReferrerWhenDowngrade,
    // https://w3c.github.io/webappsec/specs/referrer-policy/#referrer-policy-state-no-referrer
    ReferrerPolicyNever,
    // https://w3c.github.io/webappsec/specs/referrer-policy/#referrer-policy-state-origin
    ReferrerPolicyOrigin,
    // https://w3c.github.io/webappsec/specs/referrer-policy/#referrer-policy-state-origin-when-cross-origin
    ReferrerPolicyOriginWhenCrossOrigin,
};

}

#endif // ReferrerPolicy_h
