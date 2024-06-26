// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_DECRYPT_CONFIG_H_
#define MEDIA_BASE_DECRYPT_CONFIG_H_

#include <iosfwd>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/media_export.h"

namespace media {

// The Common Encryption spec provides for subsample encryption, where portions
// of a sample are set in cleartext. A SubsampleEntry specifies the number of
// clear and encrypted bytes in each subsample. For decryption, all of the
// encrypted bytes in a sample should be considered a single logical stream,
// regardless of how they are divided into subsamples, and the clear bytes
// should not be considered as part of decryption. This is logically equivalent
// to concatenating all 'cypher_bytes' portions of subsamples, decrypting that
// result, and then copying each byte from the decrypted block over the
// position of the corresponding encrypted byte.
struct SubsampleEntry {
    SubsampleEntry()
        : clear_bytes(0)
        , cypher_bytes(0)
    {
    }
    SubsampleEntry(uint32 clear_bytes, uint32 cypher_bytes)
        : clear_bytes(clear_bytes)
        , cypher_bytes(cypher_bytes)
    {
    }
    uint32 clear_bytes;
    uint32 cypher_bytes;
};

// Contains all information that a decryptor needs to decrypt a media sample.
class MEDIA_EXPORT DecryptConfig {
public:
    // Keys are always 128 bits.
    static const int kDecryptionKeySize = 16;

    // |key_id| is the ID that references the decryption key for this sample.
    // |iv| is the initialization vector defined by the encrypted format.
    //   Currently |iv| must be 16 bytes as defined by WebM and ISO. Or must be
    //   empty which signals an unencrypted frame.
    // |subsamples| defines the clear and encrypted portions of the sample as
    //   described above. A decrypted buffer will be equal in size to the sum
    //   of the subsample sizes.
    DecryptConfig(const std::string& key_id,
        const std::string& iv,
        const std::vector<SubsampleEntry>& subsamples);
    ~DecryptConfig();

    const std::string& key_id() const { return key_id_; }
    const std::string& iv() const { return iv_; }
    const std::vector<SubsampleEntry>& subsamples() const { return subsamples_; }

    // Returns true if all fields in |config| match this config.
    bool Matches(const DecryptConfig& config) const;

    // Prints to std::ostream.
    std::ostream& Print(std::ostream& os) const;

private:
    const std::string key_id_;

    // Initialization vector.
    const std::string iv_;

    // Subsample information. May be empty for some formats, meaning entire frame
    // (less data ignored by data_offset_) is encrypted.
    const std::vector<SubsampleEntry> subsamples_;

    DISALLOW_COPY_AND_ASSIGN(DecryptConfig);
};

} // namespace media

inline std::ostream& operator<<(std::ostream& os,
    const media::DecryptConfig& obj)
{
    return obj.Print(os);
}

#endif // MEDIA_BASE_DECRYPT_CONFIG_H_
