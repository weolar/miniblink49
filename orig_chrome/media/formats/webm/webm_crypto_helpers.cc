// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/webm/webm_crypto_helpers.h"

#include "base/logging.h"
//#include "base/sys_byteorder.h"
#include "media/base/decrypt_config.h"
#include "media/formats/webm/webm_constants.h"

namespace media {
namespace {

    // Generates a 16 byte CTR counter block. The CTR counter block format is a
    // CTR IV appended with a CTR block counter. |iv| is an 8 byte CTR IV.
    // |iv_size| is the size of |iv| in btyes. Returns a string of
    // kDecryptionKeySize bytes.
    std::string GenerateWebMCounterBlock(const uint8* iv, int iv_size)
    {
        std::string counter_block(reinterpret_cast<const char*>(iv), iv_size);
        counter_block.append(DecryptConfig::kDecryptionKeySize - iv_size, 0);
        return counter_block;
    }

} // namespace anonymous

bool WebMCreateDecryptConfig(const uint8* data, int data_size,
    const uint8* key_id, int key_id_size,
    scoped_ptr<DecryptConfig>* decrypt_config,
    int* data_offset)
{
    if (data_size < kWebMSignalByteSize) {
        DVLOG(1) << "Got a block from an encrypted stream with no data.";
        return false;
    }

    uint8 signal_byte = data[0];
    int frame_offset = sizeof(signal_byte);

    // Setting the DecryptConfig object of the buffer while leaving the
    // initialization vector empty will tell the decryptor that the frame is
    // unencrypted.
    std::string counter_block;

    if (signal_byte & kWebMFlagEncryptedFrame) {
        if (data_size < kWebMSignalByteSize + kWebMIvSize) {
            DVLOG(1) << "Got an encrypted block with not enough data " << data_size;
            return false;
        }
        counter_block = GenerateWebMCounterBlock(data + frame_offset, kWebMIvSize);
        frame_offset += kWebMIvSize;
    }

    decrypt_config->reset(new DecryptConfig(
        std::string(reinterpret_cast<const char*>(key_id), key_id_size),
        counter_block,
        std::vector<SubsampleEntry>()));
    *data_offset = frame_offset;

    return true;
}

} // namespace media
