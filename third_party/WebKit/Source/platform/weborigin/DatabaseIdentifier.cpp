/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/weborigin/DatabaseIdentifier.h"

#include "platform/weborigin/KURL.h"
#include "platform/weborigin/KnownPorts.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "platform/weborigin/SecurityOriginCache.h"
#include "platform/weborigin/SecurityPolicy.h"
#include "wtf/HexNumber.h"
#include "wtf/MainThread.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

const int maxAllowedPort = 65535;

static const char separatorCharacter = '_';

PassRefPtr<SecurityOrigin> createSecurityOriginFromDatabaseIdentifier(const String& databaseIdentifier)
{
    if (!databaseIdentifier.containsOnlyASCII())
        return SecurityOrigin::createUnique();

    // Make sure there's a first separator
    size_t separator1 = databaseIdentifier.find(separatorCharacter);
    if (separator1 == kNotFound)
        return SecurityOrigin::createUnique();

    // Make sure there's a second separator
    size_t separator2 = databaseIdentifier.reverseFind(separatorCharacter);
    if (separator2 == kNotFound)
        return SecurityOrigin::createUnique();

    // Ensure there were at least 2 separator characters. Some hostnames on intranets have
    // underscores in them, so we'll assume that any additional underscores are part of the host.
    if (separator1 == separator2)
        return SecurityOrigin::createUnique();

    // Make sure the port section is a valid port number or doesn't exist
    bool portOkay;
    int port = databaseIdentifier.right(databaseIdentifier.length() - separator2 - 1).toInt(&portOkay);
    bool portAbsent = (separator2 == databaseIdentifier.length() - 1);
    if (!(portOkay || portAbsent))
        return SecurityOrigin::createUnique();

    if (port < 0 || port > maxAllowedPort)
        return SecurityOrigin::createUnique();

    // Split out the 3 sections of data
    String protocol = databaseIdentifier.substring(0, separator1);
    String host = databaseIdentifier.substring(separator1 + 1, separator2 - separator1 - 1);

    // Make sure the components match their canonical representation so we are sure we're round tripping correctly.
    KURL url(KURL(), protocol + "://" + host + ":" + String::number(port) + "/");
    if (!url.isValid() || url.protocol() != protocol || url.host() != host)
        return SecurityOrigin::createUnique();

    return SecurityOrigin::create(url);
}

String createDatabaseIdentifierFromSecurityOrigin(const SecurityOrigin* securityOrigin)
{
    // Historically, we've used the following (somewhat non-sensical) string
    // for the databaseIdentifier of local files. We used to compute this
    // string because of a bug in how we handled the scheme for file URLs.
    // Now that we've fixed that bug, we still need to produce this string
    // to avoid breaking existing persistent state.
    if (securityOrigin->needsDatabaseIdentifierQuirkForFiles())
        return "file__0";

    String separatorString(&separatorCharacter, 1);

    return securityOrigin->protocol() + separatorString + securityOrigin->host() + separatorString + String::number(securityOrigin->port());
}

} // namespace blink
