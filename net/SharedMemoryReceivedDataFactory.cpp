// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/child/shared_memory_received_data_factory.h"

#include <algorithm>

#include "content/common/resource_messages.h"
#include "ipc/ipc_sender.h"

namespace content {

class SharedMemoryReceivedDataFactory::SharedMemoryReceivedData final
    : public RequestPeer::ReceivedData {
 public:
  SharedMemoryReceivedData(
      const char* payload,
      int length,
      int encoded_length,
      scoped_refptr<SharedMemoryReceivedDataFactory> factory,
      SharedMemoryReceivedDataFactory::TicketId id)
      : payload_(payload),
        length_(length),
        encoded_length_(encoded_length),
        factory_(factory),
        id_(id) {}

  ~SharedMemoryReceivedData() override { factory_->Reclaim(id_); }

  const char* payload() const override { return payload_; }
  int length() const override { return length_; }
  int encoded_length() const override { return encoded_length_; }

 private:
  const char* const payload_;
  const int length_;
  const int encoded_length_;

  scoped_refptr<SharedMemoryReceivedDataFactory> factory_;
  SharedMemoryReceivedDataFactory::TicketId id_;

  DISALLOW_COPY_AND_ASSIGN(SharedMemoryReceivedData);
};

SharedMemoryReceivedDataFactory::SharedMemoryReceivedDataFactory(
    IPC::Sender* message_sender,
    int request_id,
    linked_ptr<base::SharedMemory> memory)
    : id_(0),
      oldest_(0),
      message_sender_(message_sender),
      request_id_(request_id),
      is_stopped_(false),
      memory_(memory) {
}

SharedMemoryReceivedDataFactory::~SharedMemoryReceivedDataFactory() {
  if (!is_stopped_)
    SendAck(released_tickets_.size());
}

scoped_ptr<RequestPeer::ReceivedData> SharedMemoryReceivedDataFactory::Create(
    int offset,
    int length,
    int encoded_length) {
  const char* start = static_cast<char*>(memory_->memory());
  const char* payload = start + offset;
  TicketId id = id_++;

  return make_scoped_ptr(
      new SharedMemoryReceivedData(payload, length, encoded_length, this, id));
}

void SharedMemoryReceivedDataFactory::Stop() {
  is_stopped_ = true;
  released_tickets_.clear();
  message_sender_ = nullptr;
}

class SharedMemoryReceivedDataFactory::TicketComparator final {
 public:
  explicit TicketComparator(TicketId oldest) : oldest_(oldest) {}
  bool operator()(TicketId x, TicketId y) const {
    if ((oldest_ <= x) == (oldest_ <= y))
      return x <= y;

    return (oldest_ <= x);
  }

 private:
  TicketId oldest_;
};

void SharedMemoryReceivedDataFactory::Reclaim(TicketId id) {
  if (is_stopped_)
    return;
  if (oldest_ != id) {
    released_tickets_.push_back(id);
    return;
  }

  ++oldest_;
  SendAck(1);
  if (released_tickets_.empty()) {
    // Fast path: (hopefully) the most typical case.
    return;
  }
  std::sort(released_tickets_.begin(), released_tickets_.end(),
            TicketComparator(oldest_));
  size_t count = 0;
  for (size_t i = 0;; ++i) {
    if (i == released_tickets_.size() ||
        released_tickets_[i] != static_cast<TicketId>(id + i + 1)) {
      count = i;
      break;
    }
  }
  released_tickets_.erase(released_tickets_.begin(),
                          released_tickets_.begin() + count);
  oldest_ += count;
  SendAck(count);
}

void SharedMemoryReceivedDataFactory::SendAck(size_t count) {
  for (size_t i = 0; i < count; ++i) {
    message_sender_->Send(new ResourceHostMsg_DataReceived_ACK(request_id_));
  }
}

}  // namespace content
