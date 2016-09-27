// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the GPUTrace class.
#ifndef GPU_COMMAND_BUFFER_SERVICE_GPU_TRACER_H_
#define GPU_COMMAND_BUFFER_SERVICE_GPU_TRACER_H_

#include <string>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/gpu_export.h"
#include "ui/gl/gl_bindings.h"

namespace gpu {
namespace gles2 {

// Id used to keep trace namespaces separate
enum GpuTracerSource {
  kTraceGroupMarker = 0,
  kTraceCHROMIUM = 1,
  kTraceDecoder = 2,
};

// Traces GPU Commands.
class GPUTracer {
 public:
  static scoped_ptr<GPUTracer> Create(gles2::GLES2Decoder* decoder);

  GPUTracer();
  virtual ~GPUTracer();

  // Scheduled processing in decoder begins.
  virtual bool BeginDecoding() = 0;

  // Scheduled processing in decoder ends.
  virtual bool EndDecoding() = 0;

  // Begin a trace marker.
  virtual bool Begin(const std::string& name, GpuTracerSource source) = 0;

  // End the last started trace marker.
  virtual bool End(GpuTracerSource source) = 0;

  virtual bool IsTracing() = 0;

  // Retrieve the name of the current open trace.
  // Returns empty string if no current open trace.
  virtual const std::string& CurrentName() const = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(GPUTracer);
};

class Outputter : public base::RefCounted<Outputter> {
 public:
  virtual void Trace(const std::string& name,
                     int64 start_time,
                     int64 end_time) = 0;

 protected:
  virtual ~Outputter() {}
  friend class base::RefCounted<Outputter>;
};

class TraceOutputter : public Outputter {
 public:
  static scoped_refptr<TraceOutputter> Create(const std::string& name);
  virtual void Trace(const std::string& name,
                     int64 start_time,
                     int64 end_time) OVERRIDE;

 protected:
  friend class base::RefCounted<Outputter>;
  explicit TraceOutputter(const std::string& name);
  virtual ~TraceOutputter();

  base::Thread named_thread_;
  uint64 local_trace_id_;

  DISALLOW_COPY_AND_ASSIGN(TraceOutputter);
};

class GPU_EXPORT Trace : public base::RefCounted<Trace> {
 public:
  explicit Trace(const std::string& name) : name_(name) {}

  virtual void Start() = 0;
  virtual void End() = 0;

  // True if the the results of this query are available.
  virtual bool IsAvailable() = 0;

  virtual bool IsProcessable();
  virtual void Process() = 0;

  virtual const std::string& name();

 protected:
  virtual ~Trace() {}

 private:
  friend class base::RefCounted<Trace>;

  std::string name_;

  DISALLOW_COPY_AND_ASSIGN(Trace);
};

class GPU_EXPORT GLARBTimerTrace : public Trace {
 public:
  GLARBTimerTrace(scoped_refptr<Outputter> outputter,
                  const std::string& name,
                  int64 offset);

  // Implementation of Tracer
  virtual void Start() OVERRIDE;
  virtual void End() OVERRIDE;
  virtual bool IsAvailable() OVERRIDE;
  virtual void Process() OVERRIDE;

 private:
  virtual ~GLARBTimerTrace();

  void Output();

  scoped_refptr<Outputter> outputter_;

  int64 offset_;
  int64 start_time_;
  int64 end_time_;
  bool end_requested_;

  GLuint queries_[2];

  DISALLOW_COPY_AND_ASSIGN(GLARBTimerTrace);
};

}  // namespace gles2
}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_GPU_TRACER_H_
