// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/shader_manager.h"

#include <utility>

#include "base/logging.h"
#include "base/strings/string_util.h"

namespace gpu {
namespace gles2 {

Shader::Shader(GLuint service_id, GLenum shader_type)
      : use_count_(0),
        service_id_(service_id),
        shader_type_(shader_type),
        valid_(false) {
}

Shader::~Shader() {
}

void Shader::IncUseCount() {
  ++use_count_;
}

void Shader::DecUseCount() {
  --use_count_;
  DCHECK_GE(use_count_, 0);
}

void Shader::MarkAsDeleted() {
  DCHECK_NE(service_id_, 0u);
  service_id_ = 0;
}

void Shader::SetStatus(
    bool valid, const char* log, ShaderTranslatorInterface* translator) {
  valid_ = valid;
  log_info_.reset(log ? new std::string(log) : NULL);
  if (translator && valid) {
    attrib_map_ = translator->attrib_map();
    uniform_map_ = translator->uniform_map();
    varying_map_ = translator->varying_map();
    name_map_ = translator->name_map();
  } else {
    attrib_map_.clear();
    uniform_map_.clear();
    varying_map_.clear();
    name_map_.clear();
  }
  if (valid && source_.get()) {
    signature_source_.reset(new std::string(source_->c_str()));
  } else {
    signature_source_.reset();
  }
}

const Shader::VariableInfo*
    Shader::GetAttribInfo(
        const std::string& name) const {
  VariableMap::const_iterator it = attrib_map_.find(name);
  return it != attrib_map_.end() ? &it->second : NULL;
}

const std::string* Shader::GetAttribMappedName(
    const std::string& original_name) const {
  for (VariableMap::const_iterator it = attrib_map_.begin();
       it != attrib_map_.end(); ++it) {
    if (it->second.name == original_name)
      return &(it->first);
  }
  return NULL;
}

const std::string* Shader::GetOriginalNameFromHashedName(
    const std::string& hashed_name) const {
  NameMap::const_iterator it = name_map_.find(hashed_name);
  if (it != name_map_.end())
    return &(it->second);
  return NULL;
}

const Shader::VariableInfo*
    Shader::GetUniformInfo(
        const std::string& name) const {
  VariableMap::const_iterator it = uniform_map_.find(name);
  return it != uniform_map_.end() ? &it->second : NULL;
}

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
  DCHECK(shaders_.empty());
}

void ShaderManager::Destroy(bool have_context) {
  while (!shaders_.empty()) {
    if (have_context) {
      Shader* shader = shaders_.begin()->second.get();
      if (!shader->IsDeleted()) {
        glDeleteShader(shader->service_id());
        shader->MarkAsDeleted();
      }
    }
    shaders_.erase(shaders_.begin());
  }
}

Shader* ShaderManager::CreateShader(
    GLuint client_id,
    GLuint service_id,
    GLenum shader_type) {
  std::pair<ShaderMap::iterator, bool> result =
      shaders_.insert(std::make_pair(
          client_id, scoped_refptr<Shader>(
              new Shader(service_id, shader_type))));
  DCHECK(result.second);
  return result.first->second.get();
}

Shader* ShaderManager::GetShader(GLuint client_id) {
  ShaderMap::iterator it = shaders_.find(client_id);
  return it != shaders_.end() ? it->second.get() : NULL;
}

bool ShaderManager::GetClientId(GLuint service_id, GLuint* client_id) const {
  // This doesn't need to be fast. It's only used during slow queries.
  for (ShaderMap::const_iterator it = shaders_.begin();
       it != shaders_.end(); ++it) {
    if (it->second->service_id() == service_id) {
      *client_id = it->first;
      return true;
    }
  }
  return false;
}

bool ShaderManager::IsOwned(Shader* shader) {
  for (ShaderMap::iterator it = shaders_.begin();
       it != shaders_.end(); ++it) {
    if (it->second.get() == shader) {
      return true;
    }
  }
  return false;
}

void ShaderManager::RemoveShader(Shader* shader) {
  DCHECK(shader);
  DCHECK(IsOwned(shader));
  if (shader->IsDeleted() && !shader->InUse()) {
    for (ShaderMap::iterator it = shaders_.begin();
         it != shaders_.end(); ++it) {
      if (it->second.get() == shader) {
        shaders_.erase(it);
        return;
      }
    }
    NOTREACHED();
  }
}

void ShaderManager::MarkAsDeleted(Shader* shader) {
  DCHECK(shader);
  DCHECK(IsOwned(shader));
  shader->MarkAsDeleted();
  RemoveShader(shader);
}

void ShaderManager::UseShader(Shader* shader) {
  DCHECK(shader);
  DCHECK(IsOwned(shader));
  shader->IncUseCount();
}

void ShaderManager::UnuseShader(Shader* shader) {
  DCHECK(shader);
  DCHECK(IsOwned(shader));
  shader->DecUseCount();
  RemoveShader(shader);
}

}  // namespace gles2
}  // namespace gpu


