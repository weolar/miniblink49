// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CRYPTO_AES_DECRYPTOR_H_
#define MEDIA_CRYPTO_AES_DECRYPTOR_H_

#include <set>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "media/base/cdm_context.h"
#include "media/base/decryptor.h"
#include "media/base/media_export.h"
#include "media/base/media_keys.h"

class GURL;

namespace crypto {
class SymmetricKey;
}

namespace media {

// Decrypts an AES encrypted buffer into an unencrypted buffer. The AES
// encryption must be CTR with a key size of 128bits.
class MEDIA_EXPORT AesDecryptor : public MediaKeys,
                                  public CdmContext,
                                  public Decryptor {
public:
    AesDecryptor(const GURL& security_origin,
        const SessionMessageCB& session_message_cb,
        const SessionClosedCB& session_closed_cb,
        const SessionKeysChangeCB& session_keys_change_cb);

    // MediaKeys implementation.
    void SetServerCertificate(const std::vector<uint8_t>& certificate,
        scoped_ptr<SimpleCdmPromise> promise) override;
    void CreateSessionAndGenerateRequest(
        SessionType session_type,
        EmeInitDataType init_data_type,
        const std::vector<uint8_t>& init_data,
        scoped_ptr<NewSessionCdmPromise> promise) override;
    void LoadSession(SessionType session_type,
        const std::string& session_id,
        scoped_ptr<NewSessionCdmPromise> promise) override;
    void UpdateSession(const std::string& session_id,
        const std::vector<uint8_t>& response,
        scoped_ptr<SimpleCdmPromise> promise) override;
    void CloseSession(const std::string& session_id,
        scoped_ptr<SimpleCdmPromise> promise) override;
    void RemoveSession(const std::string& session_id,
        scoped_ptr<SimpleCdmPromise> promise) override;
    CdmContext* GetCdmContext() override;

    // CdmContext implementation.
    Decryptor* GetDecryptor() override;
    int GetCdmId() const override;

    // Decryptor implementation.
    void RegisterNewKeyCB(StreamType stream_type,
        const NewKeyCB& key_added_cb) override;
    void Decrypt(StreamType stream_type,
        const scoped_refptr<DecoderBuffer>& encrypted,
        const DecryptCB& decrypt_cb) override;
    void CancelDecrypt(StreamType stream_type) override;
    void InitializeAudioDecoder(const AudioDecoderConfig& config,
        const DecoderInitCB& init_cb) override;
    void InitializeVideoDecoder(const VideoDecoderConfig& config,
        const DecoderInitCB& init_cb) override;
    void DecryptAndDecodeAudio(const scoped_refptr<DecoderBuffer>& encrypted,
        const AudioDecodeCB& audio_decode_cb) override;
    void DecryptAndDecodeVideo(const scoped_refptr<DecoderBuffer>& encrypted,
        const VideoDecodeCB& video_decode_cb) override;
    void ResetDecoder(StreamType stream_type) override;
    void DeinitializeDecoder(StreamType stream_type) override;

private:
    // TODO(fgalligan): Remove this and change KeyMap to use crypto::SymmetricKey
    // as there are no decryptors that are performing an integrity check.
    // Helper class that manages the decryption key.
    class DecryptionKey {
    public:
        explicit DecryptionKey(const std::string& secret);
        ~DecryptionKey();

        // Creates the encryption key.
        bool Init();

        crypto::SymmetricKey* decryption_key() { return decryption_key_.get(); }

    private:
        // The base secret that is used to create the decryption key.
        const std::string secret_;

        // The key used to decrypt the data.
        scoped_ptr<crypto::SymmetricKey> decryption_key_;

        DISALLOW_COPY_AND_ASSIGN(DecryptionKey);
    };

    // Keep track of the keys for a key ID. If multiple sessions specify keys
    // for the same key ID, then the last key inserted is used. The structure is
    // optimized so that Decrypt() has fast access, at the cost of slow deletion
    // of keys when a session is released.
    class SessionIdDecryptionKeyMap;

    // Key ID <-> SessionIdDecryptionKeyMap map.
    typedef base::ScopedPtrHashMap<std::string,
        scoped_ptr<SessionIdDecryptionKeyMap>>
        KeyIdToSessionKeysMap;

    ~AesDecryptor() override;

    // Creates a DecryptionKey using |key_string| and associates it with |key_id|.
    // Returns true if successful.
    bool AddDecryptionKey(const std::string& session_id,
        const std::string& key_id,
        const std::string& key_string);

    // Gets a DecryptionKey associated with |key_id|. The AesDecryptor still owns
    // the key. Returns NULL if no key is associated with |key_id|.
    DecryptionKey* GetKey_Locked(const std::string& key_id) const;

    // Determines if |key_id| is already specified for |session_id|.
    bool HasKey(const std::string& session_id, const std::string& key_id);

    // Deletes all keys associated with |session_id|.
    void DeleteKeysForSession(const std::string& session_id);

    // Callbacks for firing session events.
    SessionMessageCB session_message_cb_;
    SessionClosedCB session_closed_cb_;
    SessionKeysChangeCB session_keys_change_cb_;

    // Since only Decrypt() is called off the renderer thread, we only need to
    // protect |key_map_|, the only member variable that is shared between
    // Decrypt() and other methods.
    KeyIdToSessionKeysMap key_map_; // Protected by |key_map_lock_|.
    mutable base::Lock key_map_lock_; // Protects the |key_map_|.

    // Keeps track of current valid sessions.
    std::set<std::string> valid_sessions_;

    // Make session ID unique per renderer by making it static. Session
    // IDs seen by the app will be "1", "2", etc.
    static uint32_t next_session_id_;

    NewKeyCB new_audio_key_cb_;
    NewKeyCB new_video_key_cb_;

    // Protect |new_audio_key_cb_| and |new_video_key_cb_| as they are set on the
    // main thread but called on the media thread.
    mutable base::Lock new_key_cb_lock_;

    DISALLOW_COPY_AND_ASSIGN(AesDecryptor);
};

} // namespace media

#endif // MEDIA_CRYPTO_AES_DECRYPTOR_H_
