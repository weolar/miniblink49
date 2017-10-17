// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_CHILD_SHARED_MEMORY_RECEIVED_DATA_FACTORY_H_
#define CONTENT_CHILD_SHARED_MEMORY_RECEIVED_DATA_FACTORY_H_

#include <vector>

#include "base/memory/linked_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/shared_memory.h"
#include "content/common/content_export.h"
#include "content/public/child/request_peer.h"

namespace IPC {
class Sender;
}  // namespace IPC

namespace content {

class CONTENT_EXPORT SharedMemoryReceivedDataFactory final
    : public base::RefCounted<SharedMemoryReceivedDataFactory> {
 public:
  SharedMemoryReceivedDataFactory(IPC::Sender* message_sender,
                                  int request_id,
                                  linked_ptr<base::SharedMemory> memory);

  scoped_ptr<RequestPeer::ReceivedData> Create(int offset,
                                               int length,
                                               int encoded_length);

  // Stops this factory. After calling this function, releasing issued data
  // won't send ack signal to the browser process.
  void Stop();

 private:
  // Here TicketId is uint32_t, but a factory may issue more data by reusing id.
  using TicketId = uint32_t;

  class SharedMemoryReceivedData;
  // Called by SharedMemoryReceivedData.
  void Reclaim(TicketId id);

  class TicketComparator;
  friend class base::RefCounted<SharedMemoryReceivedDataFactory>;
  ~SharedMemoryReceivedDataFactory();

  void SendAck(size_t count);

  TicketId id_;
  TicketId oldest_;
  std::vector<TicketId> released_tickets_;
  // We assume that |message_sender_| is valid until |Stop| is called.
  IPC::Sender* message_sender_;
  int request_id_;
  bool is_stopped_;
  // Just to keep the payload alive while issued data is alive.
  linked_ptr<base::SharedMemory> memory_;

  DISALLOW_COPY_AND_ASSIGN(SharedMemoryReceivedDataFactory);
};

}  // namespace content

#endif  // CONTENT_CHILD_SHARED_MEMORY_RECEIVED_DATA_FACTORY_H_
