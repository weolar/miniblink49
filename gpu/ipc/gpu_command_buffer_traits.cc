// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/ipc/gpu_command_buffer_traits.h"
#include "gpu/command_buffer/common/mailbox_holder.h"

namespace IPC {

void ParamTraits<gpu::CommandBuffer::State> ::Write(Message* m,
                                                    const param_type& p) {
  WriteParam(m, p.num_entries);
  WriteParam(m, p.get_offset);
  WriteParam(m, p.put_offset);
  WriteParam(m, p.token);
  WriteParam(m, static_cast<int32>(p.error));
  WriteParam(m, p.generation);
}

bool ParamTraits<gpu::CommandBuffer::State> ::Read(const Message* m,
                                                   PickleIterator* iter,
                                                   param_type* p) {
  int32 temp;
  if (ReadParam(m, iter, &p->num_entries) &&
      ReadParam(m, iter, &p->get_offset) &&
      ReadParam(m, iter, &p->put_offset) &&
      ReadParam(m, iter, &p->token) &&
      ReadParam(m, iter, &temp) &&
      ReadParam(m, iter, &p->generation)) {
    p->error = static_cast<gpu::error::Error>(temp);
    return true;
  } else {
    return false;
  }
}

void ParamTraits<gpu::CommandBuffer::State> ::Log(const param_type& p,
                                                  std::string* l) {
  l->append("<CommandBuffer::State>");
}

void ParamTraits<gpu::Mailbox>::Write(Message* m, const param_type& p) {
  m->WriteBytes(p.name, sizeof(p.name));
}

bool ParamTraits<gpu::Mailbox>::Read(const Message* m,
                                     PickleIterator* iter,
                                     param_type* p) {
  const char* bytes = NULL;
  if (!m->ReadBytes(iter, &bytes, sizeof(p->name)))
    return false;
  DCHECK(bytes);
  memcpy(p->name, bytes, sizeof(p->name));
  return true;
}

void ParamTraits<gpu::Mailbox>::Log(const param_type& p, std::string* l) {
  for (size_t i = 0; i < sizeof(p.name); ++i)
    *l += base::StringPrintf("%02x", p.name[i]);
}

void ParamTraits<gpu::MailboxHolder>::Write(Message* m, const param_type& p) {
  WriteParam(m, p.mailbox);
  WriteParam(m, p.texture_target);
  WriteParam(m, p.sync_point);
}

bool ParamTraits<gpu::MailboxHolder>::Read(const Message* m,
                                           PickleIterator* iter,
                                           param_type* p) {
  if (!ReadParam(m, iter, &p->mailbox) ||
      !ReadParam(m, iter, &p->texture_target) ||
      !ReadParam(m, iter, &p->sync_point))
    return false;
  return true;
}

void ParamTraits<gpu::MailboxHolder>::Log(const param_type& p, std::string* l) {
  ParamTraits<gpu::Mailbox>::Log(p.mailbox, l);
  *l += base::StringPrintf(":%04x@%d", p.texture_target, p.sync_point);
}

}  // namespace IPC
