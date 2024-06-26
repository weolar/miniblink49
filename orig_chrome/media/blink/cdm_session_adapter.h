// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_CDM_SESSION_ADAPTER_H_
#define MEDIA_BLINK_CDM_SESSION_ADAPTER_H_

#include <map>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "media/base/media_keys.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModuleResult.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModuleSession.h"

namespace blink {
class WebURL;
}

namespace media {

struct CdmConfig;
class CdmFactory;
class WebContentDecryptionModuleSessionImpl;

// Owns the CDM instance and makes calls from session objects to the CDM.
// Forwards the session ID-based callbacks of the MediaKeys interface to the
// appropriate session object. Callers should hold references to this class
// as long as they need the CDM instance.
class CdmSessionAdapter : public base::RefCounted<CdmSessionAdapter> {
public:
    CdmSessionAdapter();

    // Creates the CDM for |key_system| using |cdm_factory| and returns the result
    // via |result|.
    void CreateCdm(CdmFactory* cdm_factory,
        const std::string& key_system,
        const blink::WebURL& security_origin,
        const CdmConfig& cdm_config,
        scoped_ptr<blink::WebContentDecryptionModuleResult> result);

    // Provides a server certificate to be used to encrypt messages to the
    // license server.
    void SetServerCertificate(const std::vector<uint8_t>& certificate,
        scoped_ptr<SimpleCdmPromise> promise);

    // Creates a new session and adds it to the internal map. The caller owns the
    // created session. RemoveSession() must be called when destroying it, if
    // RegisterSession() was called.
    WebContentDecryptionModuleSessionImpl* CreateSession();

    // Adds a session to the internal map. Called once the session is successfully
    // initialized. Returns true if the session was registered, false if there is
    // already an existing session with the same |session_id|.
    bool RegisterSession(
        const std::string& session_id,
        base::WeakPtr<WebContentDecryptionModuleSessionImpl> session);

    // Removes a session from the internal map.
    void UnregisterSession(const std::string& session_id);

    // Initializes a session with the |init_data_type|, |init_data| and
    // |session_type| provided.
    void InitializeNewSession(EmeInitDataType init_data_type,
        const std::vector<uint8_t>& init_data,
        MediaKeys::SessionType session_type,
        scoped_ptr<NewSessionCdmPromise> promise);

    // Loads the session specified by |session_id|.
    void LoadSession(MediaKeys::SessionType session_type,
        const std::string& session_id,
        scoped_ptr<NewSessionCdmPromise> promise);

    // Updates the session specified by |session_id| with |response|.
    void UpdateSession(const std::string& session_id,
        const std::vector<uint8_t>& response,
        scoped_ptr<SimpleCdmPromise> promise);

    // Closes the session specified by |session_id|.
    void CloseSession(const std::string& session_id,
        scoped_ptr<SimpleCdmPromise> promise);

    // Removes stored session data associated with the session specified by
    // |session_id|.
    void RemoveSession(const std::string& session_id,
        scoped_ptr<SimpleCdmPromise> promise);

    // Returns the CdmContext associated with |media_keys_|.
    // TODO(jrummell): Figure out lifetimes, as WMPI may still use the decryptor
    // after WebContentDecryptionModule is freed. http://crbug.com/330324
    CdmContext* GetCdmContext();

    // Returns the key system name.
    const std::string& GetKeySystem() const;

    // Returns a prefix to use for UMAs.
    const std::string& GetKeySystemUMAPrefix() const;

private:
    friend class base::RefCounted<CdmSessionAdapter>;

    // Session ID to WebContentDecryptionModuleSessionImpl mapping.
    typedef base::hash_map<std::string,
        base::WeakPtr<WebContentDecryptionModuleSessionImpl>>
        SessionMap;

    ~CdmSessionAdapter();

    // Callback for CreateCdm().
    void OnCdmCreated(const std::string& key_system,
        base::TimeTicks start_time,
        const scoped_refptr<MediaKeys>& cdm,
        const std::string& error_message);

    // Callbacks for firing session events.
    void OnSessionMessage(const std::string& session_id,
        MediaKeys::MessageType message_type,
        const std::vector<uint8_t>& message,
        const blink::WebURL& legacy_destination_url);
    void OnSessionKeysChange(const std::string& session_id,
        bool has_additional_usable_key,
        CdmKeysInfo keys_info);
    void OnSessionExpirationUpdate(const std::string& session_id,
        const base::Time& new_expiry_time);
    void OnSessionClosed(const std::string& session_id);
    void OnLegacySessionError(const std::string& session_id,
        MediaKeys::Exception exception_code,
        uint32_t system_code,
        const std::string& error_message);

    // Helper function of the callbacks.
    WebContentDecryptionModuleSessionImpl* GetSession(
        const std::string& session_id);

    void ReportTimeToCreateCdmUMA(base::TimeDelta cdm_creation_time) const;

    scoped_refptr<MediaKeys> cdm_;

    SessionMap sessions_;

    std::string key_system_;
    std::string key_system_uma_prefix_;

    // A unique ID to trace CdmSessionAdapter::CreateCdm() call and the matching
    // OnCdmCreated() call.
    uint32 trace_id_;

    scoped_ptr<blink::WebContentDecryptionModuleResult> cdm_created_result_;

    // NOTE: Weak pointers must be invalidated before all other member variables.
    base::WeakPtrFactory<CdmSessionAdapter> weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(CdmSessionAdapter);
};

} // namespace media

#endif // MEDIA_BLINK_CDM_SESSION_ADAPTER_H_
