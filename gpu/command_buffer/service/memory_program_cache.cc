// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/memory_program_cache.h"

#include "base/base64.h"
#include "base/command_line.h"
#include "base/metrics/histogram.h"
#include "base/sha1.h"
#include "base/strings/string_number_conversions.h"
#include "gpu/command_buffer/common/constants.h"
#include "gpu/command_buffer/service/disk_cache_proto.pb.h"
#include "gpu/command_buffer/service/gl_utils.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/gpu_switches.h"
#include "gpu/command_buffer/service/shader_manager.h"
#include "gpu/command_buffer/service/shader_translator.h"
#include "ui/gl/gl_bindings.h"

namespace {

size_t GetCacheSizeBytes() {
  size_t size;
  const CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kGpuProgramCacheSizeKb) &&
      base::StringToSizeT(command_line->GetSwitchValueNative(
          switches::kGpuProgramCacheSizeKb),
          &size)) {
      return size * 1024;
  }
  return gpu::kDefaultMaxProgramCacheMemoryBytes;
}

}  // anonymous namespace

namespace gpu {
namespace gles2 {

namespace {

enum ShaderMapType {
  ATTRIB_MAP = 0,
  UNIFORM_MAP,
  VARYING_MAP
};

void StoreShaderInfo(ShaderMapType type, ShaderProto *proto,
                     const ShaderTranslator::VariableMap& map) {
  ShaderTranslator::VariableMap::const_iterator iter;
  for (iter = map.begin(); iter != map.end(); ++iter) {
    ShaderInfoProto* info = NULL;
    switch (type) {
      case UNIFORM_MAP:
        info = proto->add_uniforms();
        break;
      case ATTRIB_MAP:
        info = proto->add_attribs();
        break;
      case VARYING_MAP:
        info = proto->add_varyings();
        break;
      default: NOTREACHED();
    }

    info->set_key(iter->first);
    info->set_type(iter->second.type);
    info->set_size(iter->second.size);
    info->set_precision(iter->second.precision);
    info->set_static_use(iter->second.static_use);
    info->set_name(iter->second.name);
  }
}

void RetrieveShaderInfo(const ShaderInfoProto& proto,
                        ShaderTranslator::VariableMap* map) {
  ShaderTranslator::VariableInfo info(
      proto.type(), proto.size(), proto.precision(),
      proto.static_use(), proto.name());
  (*map)[proto.key()] = info;
}

void FillShaderProto(ShaderProto* proto, const char* sha,
                     const Shader* shader) {
  proto->set_sha(sha, gpu::gles2::ProgramCache::kHashLength);
  StoreShaderInfo(ATTRIB_MAP, proto, shader->attrib_map());
  StoreShaderInfo(UNIFORM_MAP, proto, shader->uniform_map());
  StoreShaderInfo(VARYING_MAP, proto, shader->varying_map());
}

void RunShaderCallback(const ShaderCacheCallback& callback,
                       GpuProgramProto* proto,
                       std::string sha_string) {
  std::string shader;
  proto->SerializeToString(&shader);

  std::string key;
  base::Base64Encode(sha_string, &key);
  callback.Run(key, shader);
}

}  // namespace

MemoryProgramCache::MemoryProgramCache()
    : max_size_bytes_(GetCacheSizeBytes()),
      curr_size_bytes_(0),
      store_(ProgramMRUCache::NO_AUTO_EVICT) {
}

MemoryProgramCache::MemoryProgramCache(const size_t max_cache_size_bytes)
    : max_size_bytes_(max_cache_size_bytes),
      curr_size_bytes_(0),
      store_(ProgramMRUCache::NO_AUTO_EVICT) {
}

MemoryProgramCache::~MemoryProgramCache() {}

void MemoryProgramCache::ClearBackend() {
  store_.Clear();
  DCHECK_EQ(0U, curr_size_bytes_);
}

ProgramCache::ProgramLoadResult MemoryProgramCache::LoadLinkedProgram(
    GLuint program,
    Shader* shader_a,
    const ShaderTranslatorInterface* translator_a,
    Shader* shader_b,
    const ShaderTranslatorInterface* translator_b,
    const LocationMap* bind_attrib_location_map,
    const ShaderCacheCallback& shader_callback) {
  char a_sha[kHashLength];
  char b_sha[kHashLength];
  DCHECK(shader_a && shader_a->signature_source() &&
         shader_b && shader_b->signature_source());
  ComputeShaderHash(
      *shader_a->signature_source(), translator_a, a_sha);
  ComputeShaderHash(
      *shader_b->signature_source(), translator_b, b_sha);

  char sha[kHashLength];
  ComputeProgramHash(a_sha,
                     b_sha,
                     bind_attrib_location_map,
                     sha);
  const std::string sha_string(sha, kHashLength);

  ProgramMRUCache::iterator found = store_.Get(sha_string);
  if (found == store_.end()) {
    return PROGRAM_LOAD_FAILURE;
  }
  const scoped_refptr<ProgramCacheValue> value = found->second;
  glProgramBinary(program,
                  value->format(),
                  static_cast<const GLvoid*>(value->data()),
                  value->length());
  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == GL_FALSE) {
    return PROGRAM_LOAD_FAILURE;
  }
  shader_a->set_attrib_map(value->attrib_map_0());
  shader_a->set_uniform_map(value->uniform_map_0());
  shader_a->set_varying_map(value->varying_map_0());
  shader_b->set_attrib_map(value->attrib_map_1());
  shader_b->set_uniform_map(value->uniform_map_1());
  shader_b->set_varying_map(value->varying_map_1());

  if (!shader_callback.is_null() &&
      !CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableGpuShaderDiskCache)) {
    scoped_ptr<GpuProgramProto> proto(
        GpuProgramProto::default_instance().New());
    proto->set_sha(sha, kHashLength);
    proto->set_format(value->format());
    proto->set_program(value->data(), value->length());

    FillShaderProto(proto->mutable_vertex_shader(), a_sha, shader_a);
    FillShaderProto(proto->mutable_fragment_shader(), b_sha, shader_b);
    RunShaderCallback(shader_callback, proto.get(), sha_string);
  }

  return PROGRAM_LOAD_SUCCESS;
}

void MemoryProgramCache::SaveLinkedProgram(
    GLuint program,
    const Shader* shader_a,
    const ShaderTranslatorInterface* translator_a,
    const Shader* shader_b,
    const ShaderTranslatorInterface* translator_b,
    const LocationMap* bind_attrib_location_map,
    const ShaderCacheCallback& shader_callback) {
  GLenum format;
  GLsizei length = 0;
  glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH_OES, &length);
  if (length == 0 || static_cast<unsigned int>(length) > max_size_bytes_) {
    return;
  }
  scoped_ptr<char[]> binary(new char[length]);
  glGetProgramBinary(program,
                     length,
                     NULL,
                     &format,
                     binary.get());
  UMA_HISTOGRAM_COUNTS("GPU.ProgramCache.ProgramBinarySizeBytes", length);

  char a_sha[kHashLength];
  char b_sha[kHashLength];
  DCHECK(shader_a && shader_a->signature_source() &&
         shader_b && shader_b->signature_source());
  ComputeShaderHash(
      *shader_a->signature_source(), translator_a, a_sha);
  ComputeShaderHash(
      *shader_b->signature_source(), translator_b, b_sha);

  char sha[kHashLength];
  ComputeProgramHash(a_sha,
                     b_sha,
                     bind_attrib_location_map,
                     sha);
  const std::string sha_string(sha, sizeof(sha));

  UMA_HISTOGRAM_COUNTS("GPU.ProgramCache.MemorySizeBeforeKb",
                       curr_size_bytes_ / 1024);

  // Evict any cached program with the same key in favor of the least recently
  // accessed.
  ProgramMRUCache::iterator existing = store_.Peek(sha_string);
  if(existing != store_.end())
    store_.Erase(existing);

  while (curr_size_bytes_ + length > max_size_bytes_) {
    DCHECK(!store_.empty());
    store_.Erase(store_.rbegin());
  }

  if (!shader_callback.is_null() &&
      !CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kDisableGpuShaderDiskCache)) {
    scoped_ptr<GpuProgramProto> proto(
        GpuProgramProto::default_instance().New());
    proto->set_sha(sha, kHashLength);
    proto->set_format(format);
    proto->set_program(binary.get(), length);

    FillShaderProto(proto->mutable_vertex_shader(), a_sha, shader_a);
    FillShaderProto(proto->mutable_fragment_shader(), b_sha, shader_b);
    RunShaderCallback(shader_callback, proto.get(), sha_string);
  }

  store_.Put(sha_string,
             new ProgramCacheValue(length,
                                   format,
                                   binary.release(),
                                   sha_string,
                                   a_sha,
                                   shader_a->attrib_map(),
                                   shader_a->uniform_map(),
                                   shader_a->varying_map(),
                                   b_sha,
                                   shader_b->attrib_map(),
                                   shader_b->uniform_map(),
                                   shader_b->varying_map(),
                                   this));

  UMA_HISTOGRAM_COUNTS("GPU.ProgramCache.MemorySizeAfterKb",
                       curr_size_bytes_ / 1024);
}

void MemoryProgramCache::LoadProgram(const std::string& program) {
  scoped_ptr<GpuProgramProto> proto(GpuProgramProto::default_instance().New());
  if (proto->ParseFromString(program)) {
    ShaderTranslator::VariableMap vertex_attribs;
    ShaderTranslator::VariableMap vertex_uniforms;
    ShaderTranslator::VariableMap vertex_varyings;

    for (int i = 0; i < proto->vertex_shader().attribs_size(); i++) {
      RetrieveShaderInfo(proto->vertex_shader().attribs(i), &vertex_attribs);
    }

    for (int i = 0; i < proto->vertex_shader().uniforms_size(); i++) {
      RetrieveShaderInfo(proto->vertex_shader().uniforms(i), &vertex_uniforms);
    }

    for (int i = 0; i < proto->vertex_shader().varyings_size(); i++) {
      RetrieveShaderInfo(proto->vertex_shader().varyings(i), &vertex_varyings);
    }

    ShaderTranslator::VariableMap fragment_attribs;
    ShaderTranslator::VariableMap fragment_uniforms;
    ShaderTranslator::VariableMap fragment_varyings;

    for (int i = 0; i < proto->fragment_shader().attribs_size(); i++) {
      RetrieveShaderInfo(proto->fragment_shader().attribs(i),
                         &fragment_attribs);
    }

    for (int i = 0; i < proto->fragment_shader().uniforms_size(); i++) {
      RetrieveShaderInfo(proto->fragment_shader().uniforms(i),
                         &fragment_uniforms);
    }

    for (int i = 0; i < proto->fragment_shader().varyings_size(); i++) {
      RetrieveShaderInfo(proto->fragment_shader().varyings(i),
                         &fragment_varyings);
    }

    scoped_ptr<char[]> binary(new char[proto->program().length()]);
    memcpy(binary.get(), proto->program().c_str(), proto->program().length());

    store_.Put(proto->sha(),
               new ProgramCacheValue(proto->program().length(),
                                     proto->format(),
                                     binary.release(),
                                     proto->sha(),
                                     proto->vertex_shader().sha().c_str(),
                                     vertex_attribs,
                                     vertex_uniforms,
                                     vertex_varyings,
                                     proto->fragment_shader().sha().c_str(),
                                     fragment_attribs,
                                     fragment_uniforms,
                                     fragment_varyings,
                                     this));

    UMA_HISTOGRAM_COUNTS("GPU.ProgramCache.MemorySizeAfterKb",
                         curr_size_bytes_ / 1024);
  } else {
    LOG(ERROR) << "Failed to parse proto file.";
  }
}

MemoryProgramCache::ProgramCacheValue::ProgramCacheValue(
    GLsizei length,
    GLenum format,
    const char* data,
    const std::string& program_hash,
    const char* shader_0_hash,
    const ShaderTranslator::VariableMap& attrib_map_0,
    const ShaderTranslator::VariableMap& uniform_map_0,
    const ShaderTranslator::VariableMap& varying_map_0,
    const char* shader_1_hash,
    const ShaderTranslator::VariableMap& attrib_map_1,
    const ShaderTranslator::VariableMap& uniform_map_1,
    const ShaderTranslator::VariableMap& varying_map_1,
    MemoryProgramCache* program_cache)
    : length_(length),
      format_(format),
      data_(data),
      program_hash_(program_hash),
      shader_0_hash_(shader_0_hash, kHashLength),
      attrib_map_0_(attrib_map_0),
      uniform_map_0_(uniform_map_0),
      varying_map_0_(varying_map_0),
      shader_1_hash_(shader_1_hash, kHashLength),
      attrib_map_1_(attrib_map_1),
      uniform_map_1_(uniform_map_1),
      varying_map_1_(varying_map_1),
      program_cache_(program_cache) {
  program_cache_->curr_size_bytes_ += length_;
  program_cache_->LinkedProgramCacheSuccess(program_hash);
}

MemoryProgramCache::ProgramCacheValue::~ProgramCacheValue() {
  program_cache_->curr_size_bytes_ -= length_;
  program_cache_->Evict(program_hash_);
}

}  // namespace gles2
}  // namespace gpu
