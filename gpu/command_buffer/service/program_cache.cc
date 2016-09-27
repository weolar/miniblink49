// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/program_cache.h"

#include <string>
#include "base/memory/scoped_ptr.h"
#include "gpu/command_buffer/service/shader_manager.h"

namespace gpu {
namespace gles2 {

ProgramCache::ProgramCache() {}
ProgramCache::~ProgramCache() {}

void ProgramCache::Clear() {
  ClearBackend();
  link_status_.clear();
}

ProgramCache::LinkedProgramStatus ProgramCache::GetLinkedProgramStatus(
    const std::string& untranslated_a,
    const ShaderTranslatorInterface* translator_a,
    const std::string& untranslated_b,
    const ShaderTranslatorInterface* translator_b,
    const std::map<std::string, GLint>* bind_attrib_location_map) const {
  char a_sha[kHashLength];
  char b_sha[kHashLength];
  ComputeShaderHash(untranslated_a, translator_a, a_sha);
  ComputeShaderHash(untranslated_b, translator_b, b_sha);

  char sha[kHashLength];
  ComputeProgramHash(a_sha,
                     b_sha,
                     bind_attrib_location_map,
                     sha);
  const std::string sha_string(sha, kHashLength);

  LinkStatusMap::const_iterator found = link_status_.find(sha_string);
  if (found == link_status_.end()) {
    return ProgramCache::LINK_UNKNOWN;
  } else {
    return found->second;
  }
}

void ProgramCache::LinkedProgramCacheSuccess(
    const std::string& shader_a,
    const ShaderTranslatorInterface* translator_a,
    const std::string& shader_b,
    const ShaderTranslatorInterface* translator_b,
    const LocationMap* bind_attrib_location_map) {
  char a_sha[kHashLength];
  char b_sha[kHashLength];
  ComputeShaderHash(shader_a, translator_a, a_sha);
  ComputeShaderHash(shader_b, translator_b, b_sha);
  char sha[kHashLength];
  ComputeProgramHash(a_sha,
                     b_sha,
                     bind_attrib_location_map,
                     sha);
  const std::string sha_string(sha, kHashLength);

  LinkedProgramCacheSuccess(sha_string);
}

void ProgramCache::LinkedProgramCacheSuccess(const std::string& program_hash) {
  link_status_[program_hash] = LINK_SUCCEEDED;
}

void ProgramCache::ComputeShaderHash(
    const std::string& str,
    const ShaderTranslatorInterface* translator,
    char* result) const {
  std::string s((
      translator ? translator->GetStringForOptionsThatWouldAffectCompilation() :
                   std::string()) + str);
  base::SHA1HashBytes(reinterpret_cast<const unsigned char*>(s.c_str()),
                      s.length(), reinterpret_cast<unsigned char*>(result));
}

void ProgramCache::Evict(const std::string& program_hash) {
  link_status_.erase(program_hash);
}

namespace {
size_t CalculateMapSize(const std::map<std::string, GLint>* map) {
  if (!map) {
    return 0;
  }
  std::map<std::string, GLint>::const_iterator it;
  size_t total = 0;
  for (it = map->begin(); it != map->end(); ++it) {
    total += 4 + it->first.length();
  }
  return total;
}
}  // anonymous namespace

void ProgramCache::ComputeProgramHash(
    const char* hashed_shader_0,
    const char* hashed_shader_1,
    const std::map<std::string, GLint>* bind_attrib_location_map,
    char* result) const {
  const size_t shader0_size = kHashLength;
  const size_t shader1_size = kHashLength;
  const size_t map_size = CalculateMapSize(bind_attrib_location_map);
  const size_t total_size = shader0_size + shader1_size + map_size;

  scoped_ptr<unsigned char[]> buffer(new unsigned char[total_size]);
  memcpy(buffer.get(), hashed_shader_0, shader0_size);
  memcpy(&buffer[shader0_size], hashed_shader_1, shader1_size);
  if (map_size != 0) {
    // copy our map
    size_t current_pos = shader0_size + shader1_size;
    std::map<std::string, GLint>::const_iterator it;
    for (it = bind_attrib_location_map->begin();
         it != bind_attrib_location_map->end();
         ++it) {
      const size_t name_size = it->first.length();
      memcpy(&buffer.get()[current_pos], it->first.c_str(), name_size);
      current_pos += name_size;
      const GLint value = it->second;
      buffer[current_pos++] = value >> 24;
      buffer[current_pos++] = value >> 16;
      buffer[current_pos++] = value >> 8;
      buffer[current_pos++] = value;
    }
  }
  base::SHA1HashBytes(buffer.get(),
                      total_size, reinterpret_cast<unsigned char*>(result));
}

}  // namespace gles2
}  // namespace gpu
