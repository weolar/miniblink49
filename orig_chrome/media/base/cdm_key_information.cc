// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/cdm_key_information.h"
#include "base/stl_util.h"

namespace media {

CdmKeyInformation::CdmKeyInformation()
    : status(INTERNAL_ERROR)
    , system_code(0)
{
}

CdmKeyInformation::CdmKeyInformation(const std::vector<uint8>& key_id,
    KeyStatus status,
    uint32 system_code)
    : key_id(key_id)
    , status(status)
    , system_code(system_code)
{
}

CdmKeyInformation::CdmKeyInformation(const std::string& key_id,
    KeyStatus status,
    uint32 system_code)
    : CdmKeyInformation(reinterpret_cast<const uint8*>(key_id.data()),
        key_id.size(),
        status,
        system_code)
{
}

CdmKeyInformation::CdmKeyInformation(const uint8* key_id_data,
    size_t key_id_length,
    KeyStatus status,
    uint32 system_code)
    : key_id(key_id_data, key_id_data + key_id_length)
    , status(status)
    , system_code(system_code)
{
}

CdmKeyInformation::~CdmKeyInformation()
{
}

} // namespace media
