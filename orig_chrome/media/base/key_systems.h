// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_KEY_SYSTEMS_H_
#define MEDIA_BASE_KEY_SYSTEMS_H_

#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "media/base/eme_constants.h"
#include "media/base/media_export.h"

namespace media {

// Provides an interface for querying registered key systems. The exposed API is
// only intended to support unprefixed EME.
//
// Many of the original static methods are still available, they should be
// migrated into this interface over time (or removed).
//
// TODO(sandersd): Provide GetKeySystem() so that it is not necessary to pass
// |key_system| to every method. http://crbug.com/457438
class MEDIA_EXPORT KeySystems {
public:
    static KeySystems* GetInstance();

    // Returns whether |key_system| is a supported key system.
    virtual bool IsSupportedKeySystem(const std::string& key_system) const = 0;

    // Returns whether |init_data_type| is supported by |key_system|.
    virtual bool IsSupportedInitDataType(
        const std::string& key_system,
        EmeInitDataType init_data_type) const = 0;

    // Returns the configuration rule for supporting a container and list of
    // codecs.
    virtual EmeConfigRule GetContentTypeConfigRule(
        const std::string& key_system,
        EmeMediaType media_type,
        const std::string& container_mime_type,
        const std::vector<std::string>& codecs) const = 0;

    // Returns the configuration rule for supporting a robustness requirement.
    virtual EmeConfigRule GetRobustnessConfigRule(
        const std::string& key_system,
        EmeMediaType media_type,
        const std::string& requested_robustness) const = 0;

    // Returns the support |key_system| provides for persistent-license sessions.
    virtual EmeSessionTypeSupport GetPersistentLicenseSessionSupport(
        const std::string& key_system) const = 0;

    // Returns the support |key_system| provides for persistent-release-message
    // sessions.
    virtual EmeSessionTypeSupport GetPersistentReleaseMessageSessionSupport(
        const std::string& key_system) const = 0;

    // Returns the support |key_system| provides for persistent state.
    virtual EmeFeatureSupport GetPersistentStateSupport(
        const std::string& key_system) const = 0;

    // Returns the support |key_system| provides for distinctive identifiers.
    virtual EmeFeatureSupport GetDistinctiveIdentifierSupport(
        const std::string& key_system) const = 0;

protected:
    virtual ~KeySystems() {};
};

// Prefixed EME API only supports prefixed (webkit-) key system name for
// certain key systems. But internally only unprefixed key systems are
// supported. The following two functions help convert between prefixed and
// unprefixed key system names.

// Gets the unprefixed key system name for |key_system|.
MEDIA_EXPORT std::string GetUnprefixedKeySystemName(
    const std::string& key_system);

// Gets the prefixed key system name for |key_system|.
MEDIA_EXPORT std::string GetPrefixedKeySystemName(
    const std::string& key_system);

// Use for prefixed EME only!
MEDIA_EXPORT bool IsSupportedKeySystemWithInitDataType(
    const std::string& key_system,
    EmeInitDataType init_data_type);

// Use for prefixed EME only!
// Returns whether |key_system| is a real supported key system that can be
// instantiated.
// Abstract parent |key_system| strings will return false.
MEDIA_EXPORT bool PrefixedIsSupportedConcreteKeySystem(
    const std::string& key_system);

// Use for prefixed EME only!
// Returns whether |key_system| supports the specified media type and codec(s).
// To be used with prefixed EME only as it generates UMAs based on the query.
MEDIA_EXPORT bool PrefixedIsSupportedKeySystemWithMediaMimeType(
    const std::string& mime_type,
    const std::vector<std::string>& codecs,
    const std::string& key_system);

// Returns a name for |key_system| suitable to UMA logging.
MEDIA_EXPORT std::string GetKeySystemNameForUMA(const std::string& key_system);

// Returns whether AesDecryptor can be used for the given |concrete_key_system|.
MEDIA_EXPORT bool CanUseAesDecryptor(const std::string& concrete_key_system);

#if defined(ENABLE_PEPPER_CDMS)
// Returns the Pepper MIME type for |concrete_key_system|.
// Returns empty string if |concrete_key_system| is unknown or not Pepper-based.
MEDIA_EXPORT std::string GetPepperType(
    const std::string& concrete_key_system);
#endif

#if defined(UNIT_TEST)
// Helper functions to add container/codec types for testing purposes.
MEDIA_EXPORT void AddContainerMask(const std::string& container, uint32 mask);
MEDIA_EXPORT void AddCodecMask(
    EmeMediaType media_type,
    const std::string& codec,
    uint32 mask);
#endif // defined(UNIT_TEST)

} // namespace media

#endif // MEDIA_BASE_KEY_SYSTEMS_H_
