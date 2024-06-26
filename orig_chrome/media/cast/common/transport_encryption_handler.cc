// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/common/transport_encryption_handler.h"

#include "base/logging.h"
#include "crypto/encryptor.h"
#include "crypto/symmetric_key.h"
#include "media/cast/net/cast_transport_defines.h"

namespace {

// Crypto.
const size_t kAesBlockSize = 16;
const size_t kAesKeySize = 16;

std::string GetAesNonce(uint32 frame_id, const std::string& iv_mask)
{
    std::string aes_nonce(kAesBlockSize, 0);

    // Serializing frame_id in big-endian order (aes_nonce[8] is the most
    // significant byte of frame_id).
    aes_nonce[11] = frame_id & 0xff;
    aes_nonce[10] = (frame_id >> 8) & 0xff;
    aes_nonce[9] = (frame_id >> 16) & 0xff;
    aes_nonce[8] = (frame_id >> 24) & 0xff;

    for (size_t i = 0; i < kAesBlockSize; ++i) {
        aes_nonce[i] ^= iv_mask[i];
    }
    return aes_nonce;
}

} // namespace

namespace media {
namespace cast {

    TransportEncryptionHandler::TransportEncryptionHandler()
        : key_()
        , encryptor_()
        , iv_mask_()
        , is_activated_(false)
    {
    }

    TransportEncryptionHandler::~TransportEncryptionHandler() { }

    bool TransportEncryptionHandler::Initialize(const std::string& aes_key,
        const std::string& aes_iv_mask)
    {
        is_activated_ = false;
        if (aes_iv_mask.size() == kAesKeySize && aes_key.size() == kAesKeySize) {
            iv_mask_ = aes_iv_mask;
            key_.reset(
                crypto::SymmetricKey::Import(crypto::SymmetricKey::AES, aes_key));
            encryptor_.reset(new crypto::Encryptor());
            encryptor_->Init(key_.get(), crypto::Encryptor::CTR, std::string());
            is_activated_ = true;
        } else if (aes_iv_mask.size() != 0 || aes_key.size() != 0) {
            DCHECK_EQ(aes_iv_mask.size(), 0u)
                << "Invalid Crypto configuration: aes_iv_mask.size";
            DCHECK_EQ(aes_key.size(), 0u)
                << "Invalid Crypto configuration: aes_key.size";
            return false;
        }
        return true;
    }

    bool TransportEncryptionHandler::Encrypt(uint32 frame_id,
        const base::StringPiece& data,
        std::string* encrypted_data)
    {
        if (!is_activated_)
            return false;
        if (!encryptor_->SetCounter(GetAesNonce(frame_id, iv_mask_))) {
            NOTREACHED() << "Failed to set counter";
            return false;
        }
        if (!encryptor_->Encrypt(data, encrypted_data)) {
            NOTREACHED() << "Encrypt error";
            return false;
        }
        return true;
    }

    bool TransportEncryptionHandler::Decrypt(uint32 frame_id,
        const base::StringPiece& ciphertext,
        std::string* plaintext)
    {
        if (!is_activated_) {
            return false;
        }
        if (!encryptor_->SetCounter(GetAesNonce(frame_id, iv_mask_))) {
            NOTREACHED() << "Failed to set counter";
            return false;
        }
        if (!encryptor_->Decrypt(ciphertext, plaintext)) {
            VLOG(1) << "Decryption error";
            return false;
        }
        return true;
    }

} // namespace cast
} // namespace media
