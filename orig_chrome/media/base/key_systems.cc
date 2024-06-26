// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/key_systems.h"

#include "base/containers/hash_tables.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "media/base/key_system_info.h"
#include "media/base/key_systems_support_uma.h"
#include "media/base/media_client.h"
#include "media/cdm/key_system_names.h"
//#include "third_party/widevine/cdm/widevine_cdm_common.h"

// "alpha" is a temporary name until a convention is defined.
const char kWidevineKeySystem[] = "com.widevine.alpha";

namespace media {

const char kClearKeyKeySystem[] = "org.w3.clearkey";
const char kPrefixedClearKeyKeySystem[] = "webkit-org.w3.clearkey";
const char kUnsupportedClearKeyKeySystem[] = "unsupported-org.w3.clearkey";

// These names are used by UMA. Do not change them!
const char kClearKeyKeySystemNameForUMA[] = "ClearKey";
const char kUnknownKeySystemNameForUMA[] = "Unknown";

struct NamedCodec {
    const char* name;
    EmeCodec type;
};

// Mapping between containers and their codecs.
// Only audio codec can belong to a "audio/*" container. Both audio and video
// codecs can belong to a "video/*" container.
// TODO(sandersd): This definition only makes sense for prefixed EME. Change it
// when prefixed EME is removed. http://crbug.com/249976
static NamedCodec kContainerToCodecMasks[] = {
    { "audio/webm", EME_CODEC_WEBM_AUDIO_ALL },
    { "video/webm", EME_CODEC_WEBM_ALL },
#if defined(USE_PROPRIETARY_CODECS)
    { "audio/mp4", EME_CODEC_MP4_AUDIO_ALL },
    { "video/mp4", EME_CODEC_MP4_ALL }
#endif // defined(USE_PROPRIETARY_CODECS)
};

// Mapping between codec names and enum values.
static NamedCodec kCodecStrings[] = {
    { "opus", EME_CODEC_WEBM_OPUS },
    { "vorbis", EME_CODEC_WEBM_VORBIS },
    { "vp8", EME_CODEC_WEBM_VP8 },
    { "vp8.0", EME_CODEC_WEBM_VP8 },
    { "vp9", EME_CODEC_WEBM_VP9 },
    { "vp9.0", EME_CODEC_WEBM_VP9 },
#if defined(USE_PROPRIETARY_CODECS)
    { "mp4a", EME_CODEC_MP4_AAC },
    { "avc1", EME_CODEC_MP4_AVC1 },
    { "avc3", EME_CODEC_MP4_AVC1 }
#endif // defined(USE_PROPRIETARY_CODECS)
};

static EmeRobustness ConvertRobustness(const std::string& robustness)
{
    if (robustness.empty())
        return EmeRobustness::EMPTY;
    if (robustness == "SW_SECURE_CRYPTO")
        return EmeRobustness::SW_SECURE_CRYPTO;
    if (robustness == "SW_SECURE_DECODE")
        return EmeRobustness::SW_SECURE_DECODE;
    if (robustness == "HW_SECURE_CRYPTO")
        return EmeRobustness::HW_SECURE_CRYPTO;
    if (robustness == "HW_SECURE_DECODE")
        return EmeRobustness::HW_SECURE_DECODE;
    if (robustness == "HW_SECURE_ALL")
        return EmeRobustness::HW_SECURE_ALL;
    return EmeRobustness::INVALID;
}

static void AddClearKey(std::vector<KeySystemInfo>* concrete_key_systems)
{
    KeySystemInfo info;
    info.key_system = kClearKeyKeySystem;

    // On Android, Vorbis, VP8, AAC and AVC1 are supported in MediaCodec:
    // http://developer.android.com/guide/appendix/media-formats.html
    // VP9 support is device dependent.

    info.supported_init_data_types = kInitDataTypeMaskWebM | kInitDataTypeMaskKeyIds;
    info.supported_codecs = EME_CODEC_WEBM_ALL;

#if defined(OS_ANDROID)
    // Temporarily disable VP9 support for Android.
    // TODO(xhwang): Use mime_util.h to query VP9 support on Android.
    info.supported_codecs &= ~EME_CODEC_WEBM_VP9;

    // Opus is not supported on Android yet. http://crbug.com/318436.
    // TODO(sandersd): Check for platform support to set this bit.
    info.supported_codecs &= ~EME_CODEC_WEBM_OPUS;
#endif // defined(OS_ANDROID)

#if defined(USE_PROPRIETARY_CODECS)
    info.supported_init_data_types |= kInitDataTypeMaskCenc;
    info.supported_codecs |= EME_CODEC_MP4_ALL;
#endif // defined(USE_PROPRIETARY_CODECS)

    info.max_audio_robustness = EmeRobustness::EMPTY;
    info.max_video_robustness = EmeRobustness::EMPTY;
    info.persistent_license_support = EmeSessionTypeSupport::NOT_SUPPORTED;
    info.persistent_release_message_support = EmeSessionTypeSupport::NOT_SUPPORTED;
    info.persistent_state_support = EmeFeatureSupport::NOT_SUPPORTED;
    info.distinctive_identifier_support = EmeFeatureSupport::NOT_SUPPORTED;

    info.use_aes_decryptor = true;

    concrete_key_systems->push_back(info);
}

// Returns whether the |key_system| is known to Chromium and is thus likely to
// be implemented in an interoperable way.
// True is always returned for a |key_system| that begins with "x-".
//
// As with other web platform features, advertising support for a key system
// implies that it adheres to a defined and interoperable specification.
//
// To ensure interoperability, implementations of a specific |key_system| string
// must conform to a specification for that identifier that defines
// key system-specific behaviors not fully defined by the EME specification.
// That specification should be provided by the owner of the domain that is the
// reverse of the |key_system| string.
// This involves more than calling a library, SDK, or platform API.
// KeySystemsImpl must be populated appropriately, and there will likely be glue
// code to adapt to the API of the library, SDK, or platform API.
//
// Chromium mainline contains this data and glue code for specific key systems,
// which should help ensure interoperability with other implementations using
// these key systems.
//
// If you need to add support for other key systems, ensure that you have
// obtained the specification for how to integrate it with EME, implemented the
// appropriate glue/adapter code, and added all the appropriate data to
// KeySystemsImpl. Only then should you change this function.
static bool IsPotentiallySupportedKeySystem(const std::string& key_system)
{
    // Known and supported key systems.
    if (key_system == kWidevineKeySystem)
        return true;
    if (key_system == kClearKey)
        return true;

    // External Clear Key is known and supports suffixes for testing.
    if (IsExternalClearKey(key_system))
        return true;

    // Chromecast defines behaviors for Cast clients within its reverse domain.
    const char kChromecastRoot[] = "com.chromecast";
    if (IsParentKeySystemOf(kChromecastRoot, key_system))
        return true;

    // Implementations that do not have a specification or appropriate glue code
    // can use the "x-" prefix to avoid conflicting with and advertising support
    // for real key system names. Use is discouraged.
    const char kExcludedPrefix[] = "x-";
    if (key_system.find(kExcludedPrefix, 0, arraysize(kExcludedPrefix) - 1) == 0)
        return true;

    return false;
}

class KeySystemsImpl : public KeySystems {
public:
    static KeySystemsImpl* GetInstance();

    void UpdateIfNeeded();

    bool IsConcreteSupportedKeySystem(const std::string& key_system) const;

    bool PrefixedIsSupportedKeySystemWithMediaMimeType(
        const std::string& mime_type,
        const std::vector<std::string>& codecs,
        const std::string& key_system);

    std::string GetKeySystemNameForUMA(const std::string& key_system) const;

    bool UseAesDecryptor(const std::string& concrete_key_system) const;

#if defined(ENABLE_PEPPER_CDMS)
    std::string GetPepperType(const std::string& concrete_key_system) const;
#endif

    void AddContainerMask(const std::string& container, uint32 mask);
    void AddCodecMask(
        EmeMediaType media_type,
        const std::string& codec,
        uint32 mask);

    // Implementation of KeySystems interface.
    bool IsSupportedKeySystem(const std::string& key_system) const override;

    bool IsSupportedInitDataType(const std::string& key_system,
        EmeInitDataType init_data_type) const override;

    EmeConfigRule GetContentTypeConfigRule(
        const std::string& key_system,
        EmeMediaType media_type,
        const std::string& container_mime_type,
        const std::vector<std::string>& codecs) const override;

    EmeConfigRule GetRobustnessConfigRule(
        const std::string& key_system,
        EmeMediaType media_type,
        const std::string& requested_robustness) const override;

    EmeSessionTypeSupport GetPersistentLicenseSessionSupport(
        const std::string& key_system) const override;

    EmeSessionTypeSupport GetPersistentReleaseMessageSessionSupport(
        const std::string& key_system) const override;

    EmeFeatureSupport GetPersistentStateSupport(
        const std::string& key_system) const override;

    EmeFeatureSupport GetDistinctiveIdentifierSupport(
        const std::string& key_system) const override;

private:
    KeySystemsImpl();
    ~KeySystemsImpl() override;

    void InitializeUMAInfo();

    void UpdateSupportedKeySystems();

    void AddConcreteSupportedKeySystems(
        const std::vector<KeySystemInfo>& concrete_key_systems);

    friend struct base::DefaultLazyInstanceTraits<KeySystemsImpl>;

    typedef base::hash_map<std::string, KeySystemInfo> KeySystemInfoMap;
    typedef base::hash_map<std::string, std::string> ParentKeySystemMap;
    typedef base::hash_map<std::string, SupportedCodecs> ContainerCodecsMap;
    typedef base::hash_map<std::string, EmeCodec> CodecsMap;
    typedef base::hash_map<std::string, EmeInitDataType> InitDataTypesMap;
    typedef base::hash_map<std::string, std::string> KeySystemNameForUMAMap;

    // TODO(sandersd): Separate container enum from codec mask value.
    // http://crbug.com/417440
    SupportedCodecs GetCodecMaskForContainer(
        const std::string& container) const;
    EmeCodec GetCodecForString(const std::string& codec) const;

    const std::string& PrefixedGetConcreteKeySystemNameFor(
        const std::string& key_system) const;

    // Returns whether a |container| type is supported by checking
    // |key_system_supported_codecs|.
    // TODO(xhwang): Update this to actually check initDataType support.
    bool IsSupportedContainer(const std::string& container,
        SupportedCodecs key_system_supported_codecs) const;

    // Returns true if all |codecs| are supported in |container| by checking
    // |key_system_supported_codecs|.
    bool IsSupportedContainerAndCodecs(
        const std::string& container,
        const std::vector<std::string>& codecs,
        SupportedCodecs key_system_supported_codecs) const;

    // Map from key system string to capabilities.
    KeySystemInfoMap concrete_key_system_map_;

    // Map from parent key system to the concrete key system that should be used
    // to represent its capabilities.
    ParentKeySystemMap parent_key_system_map_;

    KeySystemsSupportUMA key_systems_support_uma_;

    ContainerCodecsMap container_to_codec_mask_map_;
    CodecsMap codec_string_map_;
    KeySystemNameForUMAMap key_system_name_for_uma_map_;

    SupportedCodecs audio_codec_mask_;
    SupportedCodecs video_codec_mask_;

    // Makes sure all methods are called from the same thread.
    base::ThreadChecker thread_checker_;

    DISALLOW_COPY_AND_ASSIGN(KeySystemsImpl);
};

static base::LazyInstance<KeySystemsImpl>::Leaky g_key_systems = LAZY_INSTANCE_INITIALIZER;

KeySystemsImpl* KeySystemsImpl::GetInstance()
{
    KeySystemsImpl* key_systems = g_key_systems.Pointer();
    key_systems->UpdateIfNeeded();
    return key_systems;
}

// Because we use a LazyInstance, the key systems info must be populated when
// the instance is lazily initiated.
KeySystemsImpl::KeySystemsImpl()
    : audio_codec_mask_(EME_CODEC_AUDIO_ALL)
    , video_codec_mask_(EME_CODEC_VIDEO_ALL)
{
    for (size_t i = 0; i < arraysize(kContainerToCodecMasks); ++i) {
        const std::string& name = kContainerToCodecMasks[i].name;
        DCHECK(!container_to_codec_mask_map_.count(name));
        container_to_codec_mask_map_[name] = kContainerToCodecMasks[i].type;
    }
    for (size_t i = 0; i < arraysize(kCodecStrings); ++i) {
        const std::string& name = kCodecStrings[i].name;
        DCHECK(!codec_string_map_.count(name));
        codec_string_map_[name] = kCodecStrings[i].type;
    }

    InitializeUMAInfo();

    // Always update supported key systems during construction.
    UpdateSupportedKeySystems();
}

KeySystemsImpl::~KeySystemsImpl()
{
}

SupportedCodecs KeySystemsImpl::GetCodecMaskForContainer(
    const std::string& container) const
{
    ContainerCodecsMap::const_iterator iter = container_to_codec_mask_map_.find(container);
    if (iter != container_to_codec_mask_map_.end())
        return iter->second;
    return EME_CODEC_NONE;
}

EmeCodec KeySystemsImpl::GetCodecForString(const std::string& codec) const
{
    CodecsMap::const_iterator iter = codec_string_map_.find(codec);
    if (iter != codec_string_map_.end())
        return iter->second;
    return EME_CODEC_NONE;
}

const std::string& KeySystemsImpl::PrefixedGetConcreteKeySystemNameFor(
    const std::string& key_system) const
{
    ParentKeySystemMap::const_iterator iter = parent_key_system_map_.find(key_system);
    if (iter != parent_key_system_map_.end())
        return iter->second;
    return key_system;
}

void KeySystemsImpl::InitializeUMAInfo()
{
    //     DCHECK(thread_checker_.CalledOnValidThread());
    //     DCHECK(key_system_name_for_uma_map_.empty());
    //
    //     std::vector<KeySystemInfoForUMA> key_systems_info_for_uma;
    //     if (GetMediaClient())
    //         GetMediaClient()->AddKeySystemsInfoForUMA(&key_systems_info_for_uma);
    //
    //     for (const KeySystemInfoForUMA& info : key_systems_info_for_uma) {
    //         key_system_name_for_uma_map_[info.key_system] = info.key_system_name_for_uma;
    //         if (info.reports_key_system_support_to_uma)
    //             key_systems_support_uma_.AddKeySystemToReport(info.key_system);
    //     }
    //
    //     // Clear Key is always supported.
    //     key_system_name_for_uma_map_[kClearKeyKeySystem] = kClearKeyKeySystemNameForUMA;
    DebugBreak();
}

void KeySystemsImpl::UpdateIfNeeded()
{
    //     if (GetMediaClient() && GetMediaClient()->IsKeySystemsUpdateNeeded())
    //         UpdateSupportedKeySystems();
    DebugBreak();
}

void KeySystemsImpl::UpdateSupportedKeySystems()
{
    DebugBreak();
    //     DCHECK(thread_checker_.CalledOnValidThread());
    //     concrete_key_system_map_.clear();
    //     parent_key_system_map_.clear();
    //
    //     // Build KeySystemInfo.
    //     std::vector<KeySystemInfo> key_systems_info;
    //
    //     // Add key systems supported by the MediaClient implementation.
    //     if (GetMediaClient())
    //         GetMediaClient()->AddSupportedKeySystems(&key_systems_info);
    //
    //     // Clear Key is always supported.
    //     AddClearKey(&key_systems_info);
    //
    //     AddConcreteSupportedKeySystems(key_systems_info);
}

void KeySystemsImpl::AddConcreteSupportedKeySystems(
    const std::vector<KeySystemInfo>& concrete_key_systems)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(concrete_key_system_map_.empty());
    DCHECK(parent_key_system_map_.empty());

    for (const KeySystemInfo& info : concrete_key_systems) {
        DCHECK(!info.key_system.empty());
        DCHECK(info.max_audio_robustness != EmeRobustness::INVALID);
        DCHECK(info.max_video_robustness != EmeRobustness::INVALID);
        DCHECK(info.persistent_license_support != EmeSessionTypeSupport::INVALID);
        DCHECK(info.persistent_release_message_support != EmeSessionTypeSupport::INVALID);
        DCHECK(info.persistent_state_support != EmeFeatureSupport::INVALID);
        DCHECK(info.distinctive_identifier_support != EmeFeatureSupport::INVALID);

        // Supporting persistent state is a prerequsite for supporting persistent
        // sessions.
        if (info.persistent_state_support == EmeFeatureSupport::NOT_SUPPORTED) {
            DCHECK(info.persistent_license_support == EmeSessionTypeSupport::NOT_SUPPORTED);
            DCHECK(info.persistent_release_message_support == EmeSessionTypeSupport::NOT_SUPPORTED);
        }

        // persistent-release-message sessions are not currently supported.
        // http://crbug.com/448888
        DCHECK(info.persistent_release_message_support == EmeSessionTypeSupport::NOT_SUPPORTED);

        // If distinctive identifiers are not supported, then no other features can
        // require them.
        if (info.distinctive_identifier_support == EmeFeatureSupport::NOT_SUPPORTED) {
            DCHECK(info.persistent_license_support != EmeSessionTypeSupport::SUPPORTED_WITH_IDENTIFIER);
            DCHECK(info.persistent_release_message_support != EmeSessionTypeSupport::SUPPORTED_WITH_IDENTIFIER);
        }

        // Distinctive identifiers and persistent state can only be reliably blocked
        // (and therefore be safely configurable) for Pepper-hosted key systems. For
        // other platforms, (except for the AES decryptor) assume that the CDM can
        // and will do anything.
        bool can_block = info.use_aes_decryptor;
#if defined(ENABLE_PEPPER_CDMS)
        DCHECK_EQ(info.use_aes_decryptor, info.pepper_type.empty());
        if (!info.pepper_type.empty())
            can_block = true;
#endif
        if (!can_block) {
            DCHECK(info.distinctive_identifier_support == EmeFeatureSupport::ALWAYS_ENABLED);
            DCHECK(info.persistent_state_support == EmeFeatureSupport::ALWAYS_ENABLED);
        }

        DCHECK(!IsConcreteSupportedKeySystem(info.key_system))
            << "Key system '" << info.key_system << "' already registered";
        DCHECK(!parent_key_system_map_.count(info.key_system))
            << "'" << info.key_system << "' is already registered as a parent";
        concrete_key_system_map_[info.key_system] = info;
        if (!info.parent_key_system.empty()) {
            DCHECK(!IsConcreteSupportedKeySystem(info.parent_key_system))
                << "Parent '" << info.parent_key_system << "' "
                << "already registered concrete";
            DCHECK(!parent_key_system_map_.count(info.parent_key_system))
                << "Parent '" << info.parent_key_system << "' already registered";
            parent_key_system_map_[info.parent_key_system] = info.key_system;
        }
    }
}

bool KeySystemsImpl::IsConcreteSupportedKeySystem(
    const std::string& key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());
    return concrete_key_system_map_.count(key_system) != 0;
}

bool KeySystemsImpl::IsSupportedContainer(
    const std::string& container,
    SupportedCodecs key_system_supported_codecs) const
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(!container.empty());

    // When checking container support for EME, "audio/foo" should be treated the
    // same as "video/foo". Convert the |container| to achieve this.
    // TODO(xhwang): Replace this with real checks against supported initDataTypes
    // combined with supported demuxers.
    std::string canonical_container = container;
    if (container.find("audio/") == 0)
        canonical_container.replace(0, 6, "video/");

    // A container is supported iif at least one codec in that container is
    // supported.
    SupportedCodecs supported_codecs = GetCodecMaskForContainer(canonical_container);
    return (supported_codecs & key_system_supported_codecs) != 0;
}

bool KeySystemsImpl::IsSupportedContainerAndCodecs(
    const std::string& container,
    const std::vector<std::string>& codecs,
    SupportedCodecs key_system_supported_codecs) const
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(!container.empty());
    DCHECK(!codecs.empty());
    DCHECK(IsSupportedContainer(container, key_system_supported_codecs));

    SupportedCodecs container_supported_codecs = GetCodecMaskForContainer(container);

    for (size_t i = 0; i < codecs.size(); ++i) {
        if (codecs[i].empty())
            continue;

        EmeCodec codec = GetCodecForString(codecs[i]);

        // Unsupported codec.
        if (!(codec & key_system_supported_codecs))
            return false;

        // Unsupported codec/container combination, e.g. "video/webm" and "avc1".
        if (!(codec & container_supported_codecs))
            return false;
    }

    return true;
}

bool KeySystemsImpl::IsSupportedInitDataType(
    const std::string& key_system,
    EmeInitDataType init_data_type) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    // Locate |key_system|. Only concrete key systems are supported in unprefixed.
    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        NOTREACHED();
        return false;
    }

    // Check |init_data_type|.
    InitDataTypeMask available_init_data_types = key_system_iter->second.supported_init_data_types;
    switch (init_data_type) {
    case EmeInitDataType::UNKNOWN:
        return false;
    case EmeInitDataType::WEBM:
        return (available_init_data_types & kInitDataTypeMaskWebM) != 0;
    case EmeInitDataType::CENC:
        return (available_init_data_types & kInitDataTypeMaskCenc) != 0;
    case EmeInitDataType::KEYIDS:
        return (available_init_data_types & kInitDataTypeMaskKeyIds) != 0;
    }
    NOTREACHED();
    return false;
}

bool KeySystemsImpl::PrefixedIsSupportedKeySystemWithMediaMimeType(
    const std::string& mime_type,
    const std::vector<std::string>& codecs,
    const std::string& key_system)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    const std::string& concrete_key_system = PrefixedGetConcreteKeySystemNameFor(key_system);

    bool has_type = !mime_type.empty();

    key_systems_support_uma_.ReportKeySystemQuery(key_system, has_type);

    // Check key system support.
    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(concrete_key_system);
    if (key_system_iter == concrete_key_system_map_.end())
        return false;

    key_systems_support_uma_.ReportKeySystemSupport(key_system, false);

    if (!has_type) {
        DCHECK(codecs.empty());
        return true;
    }

    SupportedCodecs key_system_supported_codecs = key_system_iter->second.supported_codecs;

    if (!IsSupportedContainer(mime_type, key_system_supported_codecs))
        return false;

    if (!codecs.empty() && !IsSupportedContainerAndCodecs(mime_type, codecs, key_system_supported_codecs)) {
        return false;
    }

    key_systems_support_uma_.ReportKeySystemSupport(key_system, true);

    return true;
}

std::string KeySystemsImpl::GetKeySystemNameForUMA(
    const std::string& key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    KeySystemNameForUMAMap::const_iterator iter = key_system_name_for_uma_map_.find(key_system);
    if (iter == key_system_name_for_uma_map_.end())
        return kUnknownKeySystemNameForUMA;

    return iter->second;
}

bool KeySystemsImpl::UseAesDecryptor(
    const std::string& concrete_key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(concrete_key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        DLOG(ERROR) << concrete_key_system << " is not a known concrete system";
        return false;
    }

    return key_system_iter->second.use_aes_decryptor;
}

#if defined(ENABLE_PEPPER_CDMS)
std::string KeySystemsImpl::GetPepperType(
    const std::string& concrete_key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(concrete_key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        DLOG(FATAL) << concrete_key_system << " is not a known concrete system";
        return std::string();
    }

    const std::string& type = key_system_iter->second.pepper_type;
    DLOG_IF(FATAL, type.empty()) << concrete_key_system << " is not Pepper-based";
    return type;
}
#endif

void KeySystemsImpl::AddContainerMask(
    const std::string& container,
    uint32 mask)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(!container_to_codec_mask_map_.count(container));
    container_to_codec_mask_map_[container] = static_cast<EmeCodec>(mask);
}

void KeySystemsImpl::AddCodecMask(
    EmeMediaType media_type,
    const std::string& codec,
    uint32 mask)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(!codec_string_map_.count(codec));
    codec_string_map_[codec] = static_cast<EmeCodec>(mask);
    if (media_type == EmeMediaType::AUDIO) {
        audio_codec_mask_ |= mask;
    } else {
        video_codec_mask_ |= mask;
    }
}

bool KeySystemsImpl::IsSupportedKeySystem(const std::string& key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    if (!IsConcreteSupportedKeySystem(key_system))
        return false;

    // TODO(ddorwin): Move this to where we add key systems when prefixed EME is
    // removed (crbug.com/249976).
    if (!IsPotentiallySupportedKeySystem(key_system)) {
        // If you encounter this path, see the comments for the above function.
        DLOG(ERROR) << "Unrecognized key system " << key_system
                    << ". See code comments.";
        return false;
    }

    return true;
}

EmeConfigRule KeySystemsImpl::GetContentTypeConfigRule(
    const std::string& key_system,
    EmeMediaType media_type,
    const std::string& container_mime_type,
    const std::vector<std::string>& codecs) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    // Make sure the container matches |media_type|.
    SupportedCodecs media_type_codec_mask = EME_CODEC_NONE;
    switch (media_type) {
    case EmeMediaType::AUDIO:
        if (!base::StartsWith(container_mime_type, "audio/",
                base::CompareCase::SENSITIVE))
            return EmeConfigRule::NOT_SUPPORTED;
        media_type_codec_mask = audio_codec_mask_;
        break;
    case EmeMediaType::VIDEO:
        if (!base::StartsWith(container_mime_type, "video/",
                base::CompareCase::SENSITIVE))
            return EmeConfigRule::NOT_SUPPORTED;
        media_type_codec_mask = video_codec_mask_;
        break;
    }

    // Look up the key system's supported codecs.
    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        NOTREACHED();
        return EmeConfigRule::NOT_SUPPORTED;
    }
    SupportedCodecs key_system_codec_mask = key_system_iter->second.supported_codecs;
#if defined(OS_ANDROID)
    SupportedCodecs key_system_secure_codec_mask = key_system_iter->second.supported_secure_codecs;
#endif // defined(OS_ANDROID)

    // Check that the container is supported by the key system. (This check is
    // necessary because |codecs| may be empty.)
    SupportedCodecs container_codec_mask = GetCodecMaskForContainer(container_mime_type) & media_type_codec_mask;
    if ((key_system_codec_mask & container_codec_mask) == 0)
        return EmeConfigRule::NOT_SUPPORTED;

    // Check that the codecs are supported by the key system and container.
    EmeConfigRule support = EmeConfigRule::SUPPORTED;
    for (size_t i = 0; i < codecs.size(); i++) {
        SupportedCodecs codec = GetCodecForString(codecs[i]);
        if ((codec & key_system_codec_mask & container_codec_mask) == 0)
            return EmeConfigRule::NOT_SUPPORTED;
#if defined(OS_ANDROID)
        // Check whether the codec supports a hardware-secure mode. The goal is to
        // prevent mixing of non-hardware-secure codecs with hardware-secure codecs,
        // since the mode is fixed at CDM creation.
        //
        // Because the check for regular codec support is early-exit, we don't have
        // to consider codecs that are only supported in hardware-secure mode. We
        // could do so, and make use of HW_SECURE_CODECS_REQUIRED, if it turns out
        // that hardware-secure-only codecs actually exist and are useful.
        if ((codec & key_system_secure_codec_mask) == 0)
            support = EmeConfigRule::HW_SECURE_CODECS_NOT_ALLOWED;
#endif // defined(OS_ANDROID)
    }

    return support;
}

EmeConfigRule KeySystemsImpl::GetRobustnessConfigRule(
    const std::string& key_system,
    EmeMediaType media_type,
    const std::string& requested_robustness) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    EmeRobustness robustness = ConvertRobustness(requested_robustness);
    if (robustness == EmeRobustness::INVALID)
        return EmeConfigRule::NOT_SUPPORTED;
    if (robustness == EmeRobustness::EMPTY)
        return EmeConfigRule::SUPPORTED;

    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        NOTREACHED();
        return EmeConfigRule::NOT_SUPPORTED;
    }

    EmeRobustness max_robustness = EmeRobustness::INVALID;
    switch (media_type) {
    case EmeMediaType::AUDIO:
        max_robustness = key_system_iter->second.max_audio_robustness;
        break;
    case EmeMediaType::VIDEO:
        max_robustness = key_system_iter->second.max_video_robustness;
        break;
    }

    // We can compare robustness levels whenever they are not HW_SECURE_CRYPTO
    // and SW_SECURE_DECODE in some order. If they are exactly those two then the
    // robustness requirement is not supported.
    if ((max_robustness == EmeRobustness::HW_SECURE_CRYPTO && robustness == EmeRobustness::SW_SECURE_DECODE) || (max_robustness == EmeRobustness::SW_SECURE_DECODE && robustness == EmeRobustness::HW_SECURE_CRYPTO) || robustness > max_robustness) {
        return EmeConfigRule::NOT_SUPPORTED;
    }

    if (key_system == kWidevineKeySystem) {
#if defined(OS_CHROMEOS)
        // Hardware security requires remote attestation.
        if (robustness >= EmeRobustness::HW_SECURE_CRYPTO)
            return EmeConfigRule::IDENTIFIER_REQUIRED;

        // For video, recommend remote attestation if HW_SECURE_ALL is available,
        // because it enables hardware accelerated decoding.
        // TODO(sandersd): Only do this when hardware accelerated decoding is
        // available for the requested codecs.
        if (media_type == EmeMediaType::VIDEO && max_robustness == EmeRobustness::HW_SECURE_ALL) {
            return EmeConfigRule::IDENTIFIER_RECOMMENDED;
        }
#elif defined(OS_ANDROID)
        if (robustness > EmeRobustness::SW_SECURE_CRYPTO)
            return EmeConfigRule::HW_SECURE_CODECS_REQUIRED;
#endif // defined(OS_CHROMEOS)
    }

    return EmeConfigRule::SUPPORTED;
}

EmeSessionTypeSupport KeySystemsImpl::GetPersistentLicenseSessionSupport(
    const std::string& key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        NOTREACHED();
        return EmeSessionTypeSupport::INVALID;
    }
    return key_system_iter->second.persistent_license_support;
}

EmeSessionTypeSupport KeySystemsImpl::GetPersistentReleaseMessageSessionSupport(
    const std::string& key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        NOTREACHED();
        return EmeSessionTypeSupport::INVALID;
    }
    return key_system_iter->second.persistent_release_message_support;
}

EmeFeatureSupport KeySystemsImpl::GetPersistentStateSupport(
    const std::string& key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        NOTREACHED();
        return EmeFeatureSupport::INVALID;
    }
    return key_system_iter->second.persistent_state_support;
}

EmeFeatureSupport KeySystemsImpl::GetDistinctiveIdentifierSupport(
    const std::string& key_system) const
{
    DCHECK(thread_checker_.CalledOnValidThread());

    KeySystemInfoMap::const_iterator key_system_iter = concrete_key_system_map_.find(key_system);
    if (key_system_iter == concrete_key_system_map_.end()) {
        NOTREACHED();
        return EmeFeatureSupport::INVALID;
    }
    return key_system_iter->second.distinctive_identifier_support;
}

KeySystems* KeySystems::GetInstance()
{
    return KeySystemsImpl::GetInstance();
}

//------------------------------------------------------------------------------

std::string GetUnprefixedKeySystemName(const std::string& key_system)
{
    if (key_system == kClearKeyKeySystem)
        return kUnsupportedClearKeyKeySystem;

    if (key_system == kPrefixedClearKeyKeySystem)
        return kClearKeyKeySystem;

    return key_system;
}

std::string GetPrefixedKeySystemName(const std::string& key_system)
{
    DCHECK_NE(key_system, kPrefixedClearKeyKeySystem);

    if (key_system == kClearKeyKeySystem)
        return kPrefixedClearKeyKeySystem;

    return key_system;
}

bool PrefixedIsSupportedConcreteKeySystem(const std::string& key_system)
{
    return KeySystemsImpl::GetInstance()->IsConcreteSupportedKeySystem(
        key_system);
}

bool IsSupportedKeySystemWithInitDataType(const std::string& key_system,
    EmeInitDataType init_data_type)
{
    return KeySystemsImpl::GetInstance()->IsSupportedInitDataType(key_system,
        init_data_type);
}

bool PrefixedIsSupportedKeySystemWithMediaMimeType(
    const std::string& mime_type,
    const std::vector<std::string>& codecs,
    const std::string& key_system)
{
    return KeySystemsImpl::GetInstance()
        ->PrefixedIsSupportedKeySystemWithMediaMimeType(mime_type, codecs,
            key_system);
}

std::string GetKeySystemNameForUMA(const std::string& key_system)
{
    return KeySystemsImpl::GetInstance()->GetKeySystemNameForUMA(key_system);
}

bool CanUseAesDecryptor(const std::string& concrete_key_system)
{
    return KeySystemsImpl::GetInstance()->UseAesDecryptor(concrete_key_system);
}

#if defined(ENABLE_PEPPER_CDMS)
std::string GetPepperType(const std::string& concrete_key_system)
{
    return KeySystemsImpl::GetInstance()->GetPepperType(concrete_key_system);
}
#endif

// These two functions are for testing purpose only. The declaration in the
// header file is guarded by "#if defined(UNIT_TEST)" so that they can be used
// by tests but not non-test code. However, this .cc file is compiled as part of
// "media" where "UNIT_TEST" is not defined. So we need to specify
// "MEDIA_EXPORT" here again so that they are visible to tests.

MEDIA_EXPORT void AddContainerMask(const std::string& container, uint32 mask)
{
    KeySystemsImpl::GetInstance()->AddContainerMask(container, mask);
}

MEDIA_EXPORT void AddCodecMask(
    EmeMediaType media_type,
    const std::string& codec,
    uint32 mask)
{
    KeySystemsImpl::GetInstance()->AddCodecMask(media_type, codec, mask);
}

} // namespace media
