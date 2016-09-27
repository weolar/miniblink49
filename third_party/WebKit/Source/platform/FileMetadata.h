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

#ifndef FileMetadata_h
#define FileMetadata_h

#include "platform/PlatformExport.h"
#include "platform/weborigin/KURL.h"
#include "wtf/MathExtras.h"
#include "wtf/text/WTFString.h"
#include <time.h>

namespace blink {

inline double invalidFileTime() { return std::numeric_limits<double>::quiet_NaN(); }
inline bool isValidFileTime(double time) { return std::isfinite(time); }

class FileMetadata {
public:
    FileMetadata()
        : modificationTime(invalidFileTime())
        , length(-1)
        , type(TypeUnknown)
    {
    }

    // The last modification time of the file, in milliseconds.
    // The value NaN means that the time is not known.
    double modificationTime;

    // The length of the file in bytes.
    // The value -1 means that the length is not set.
    long long length;

    enum Type {
        TypeUnknown = 0,
        TypeFile,
        TypeDirectory
    };

    Type type;
    String platformPath;
};

PLATFORM_EXPORT bool getFileSize(const String&, long long& result);
PLATFORM_EXPORT bool getFileModificationTime(const String&, double& result);
PLATFORM_EXPORT bool getFileMetadata(const String&, FileMetadata&);
PLATFORM_EXPORT String directoryName(const String&);
PLATFORM_EXPORT KURL filePathToURL(const String&);

} // namespace blink

#endif // FileMetadata_h
