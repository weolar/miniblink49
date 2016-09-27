// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gpu_tracer.h"

#include <deque>

#include "base/bind.h"
#include "base/debug/trace_event.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"

namespace gpu {
namespace gles2 {

static const unsigned int kProcessInterval = 16;
static TraceOutputter* g_outputter_thread = NULL;

scoped_refptr<TraceOutputter> TraceOutputter::Create(const std::string& name) {
  if (!g_outputter_thread) {
    g_outputter_thread = new TraceOutputter(name);
  }
  return g_outputter_thread;
}

TraceOutputter::TraceOutputter(const std::string& name)
    : named_thread_(name.c_str()), local_trace_id_(0) {
  named_thread_.Start();
  named_thread_.Stop();
}

TraceOutputter::~TraceOutputter() { g_outputter_thread = NULL; }

void TraceOutputter::Trace(const std::string& name,
                           int64 start_time,
                           int64 end_time) {
  TRACE_EVENT_COPY_BEGIN_WITH_ID_TID_AND_TIMESTAMP0(
      TRACE_DISABLED_BY_DEFAULT("gpu.device"),
      name.c_str(),
      local_trace_id_,
      named_thread_.thread_id(),
      start_time);
  TRACE_EVENT_COPY_END_WITH_ID_TID_AND_TIMESTAMP0(
      TRACE_DISABLED_BY_DEFAULT("gpu.device"),
      name.c_str(),
      local_trace_id_,
      named_thread_.thread_id(),
      end_time);
  ++local_trace_id_;
}

class NoopTrace : public Trace {
 public:
  explicit NoopTrace(const std::string& name) : Trace(name) {}

  // Implementation of Tracer
  virtual void Start() OVERRIDE {
    TRACE_EVENT_COPY_ASYNC_BEGIN0(
        TRACE_DISABLED_BY_DEFAULT("gpu.service"), name().c_str(), this);
  }
  virtual void End() OVERRIDE {
    TRACE_EVENT_COPY_ASYNC_END0(
        TRACE_DISABLED_BY_DEFAULT("gpu.service"), name().c_str(), this);
  }
  virtual bool IsAvailable() OVERRIDE { return true; }
  virtual bool IsProcessable() OVERRIDE { return false; }
  virtual void Process() OVERRIDE {}

 private:
  virtual ~NoopTrace() {}

  DISALLOW_COPY_AND_ASSIGN(NoopTrace);
};

struct TraceMarker {
  TraceMarker(const std::string& name, GpuTracerSource source)
      : name_(name), source_(source) {}

  std::string name_;
  GpuTracerSource source_;
  scoped_refptr<Trace> trace_;
};

class GPUTracerImpl
    : public GPUTracer,
      public base::SupportsWeakPtr<GPUTracerImpl> {
 public:
  GPUTracerImpl()
      : gpu_trace_srv_category(TRACE_EVENT_API_GET_CATEGORY_GROUP_ENABLED(
            TRACE_DISABLED_BY_DEFAULT("gpu.service"))),
        gpu_trace_dev_category(TRACE_EVENT_API_GET_CATEGORY_GROUP_ENABLED(
            TRACE_DISABLED_BY_DEFAULT("gpu.device"))),
        gpu_executing_(false),
        process_posted_(false) {}
  virtual ~GPUTracerImpl() {}

  // Implementation of gpu::gles2::GPUTracer
  virtual bool BeginDecoding() OVERRIDE;
  virtual bool EndDecoding() OVERRIDE;
  virtual bool Begin(const std::string& name, GpuTracerSource source) OVERRIDE;
  virtual bool End(GpuTracerSource source) OVERRIDE;
  virtual const std::string& CurrentName() const OVERRIDE;
  virtual bool IsTracing() OVERRIDE {
    return (*gpu_trace_srv_category != 0) || (*gpu_trace_dev_category != 0);
  }
  virtual void CalculateTimerOffset() {}

  // Process any completed traces.
  virtual void Process();
  virtual void ProcessTraces();

 protected:
  // Create a new trace.
  virtual scoped_refptr<Trace> CreateTrace(const std::string& name);

  const unsigned char* gpu_trace_srv_category;
  const unsigned char* gpu_trace_dev_category;

 protected:
  void IssueProcessTask();

  std::vector<TraceMarker> markers_;
  std::deque<scoped_refptr<Trace> > traces_;

  bool gpu_executing_;
  bool process_posted_;

  DISALLOW_COPY_AND_ASSIGN(GPUTracerImpl);
};

class GPUTracerARBTimerQuery : public GPUTracerImpl {
 public:
  explicit GPUTracerARBTimerQuery(gles2::GLES2Decoder* decoder);
  virtual ~GPUTracerARBTimerQuery();

  // Implementation of GPUTracerImpl
  virtual void ProcessTraces() OVERRIDE;

 protected:
  // Implementation of GPUTracerImpl.
  virtual bool BeginDecoding() OVERRIDE;
  virtual bool EndDecoding() OVERRIDE;
  virtual scoped_refptr<Trace> CreateTrace(const std::string& name) OVERRIDE;
  virtual void CalculateTimerOffset() OVERRIDE;

  scoped_refptr<Outputter> outputter_;

  bool gpu_timing_synced_;
  int64 timer_offset_;

  gles2::GLES2Decoder* decoder_;

  DISALLOW_COPY_AND_ASSIGN(GPUTracerARBTimerQuery);
};

bool Trace::IsProcessable() { return true; }

const std::string& Trace::name() { return name_; }

GLARBTimerTrace::GLARBTimerTrace(scoped_refptr<Outputter> outputter,
                                 const std::string& name,
                                 int64 offset)
    : Trace(name),
      outputter_(outputter),
      offset_(offset),
      start_time_(0),
      end_time_(0),
      end_requested_(false) {
  glGenQueries(2, queries_);
}

GLARBTimerTrace::~GLARBTimerTrace() { glDeleteQueries(2, queries_); }

void GLARBTimerTrace::Start() {
  TRACE_EVENT_COPY_ASYNC_BEGIN0(
      TRACE_DISABLED_BY_DEFAULT("gpu.service"), name().c_str(), this);
  glQueryCounter(queries_[0], GL_TIMESTAMP);
}

void GLARBTimerTrace::End() {
  glQueryCounter(queries_[1], GL_TIMESTAMP);
  end_requested_ = true;
  TRACE_EVENT_COPY_ASYNC_END0(
      TRACE_DISABLED_BY_DEFAULT("gpu.service"), name().c_str(), this);
}

bool GLARBTimerTrace::IsAvailable() {
  if (!end_requested_)
    return false;

  GLint done = 0;
  glGetQueryObjectiv(queries_[1], GL_QUERY_RESULT_AVAILABLE, &done);
  return !!done;
}

void GLARBTimerTrace::Process() {
  DCHECK(IsAvailable());

  GLuint64 timestamp;

  // TODO(dsinclair): It's possible for the timer to wrap during the start/end.
  // We need to detect if the end is less then the start and correct for the
  // wrapping.
  glGetQueryObjectui64v(queries_[0], GL_QUERY_RESULT, &timestamp);
  start_time_ = (timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_;

  glGetQueryObjectui64v(queries_[1], GL_QUERY_RESULT, &timestamp);
  end_time_ = (timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_;

  glDeleteQueries(2, queries_);
  outputter_->Trace(name(), start_time_, end_time_);
}

bool GPUTracerImpl::BeginDecoding() {
  if (gpu_executing_)
    return false;

  gpu_executing_ = true;

  if (IsTracing()) {
    // Begin a Trace for all active markers
    for (size_t i = 0; i < markers_.size(); i++) {
      markers_[i].trace_ = CreateTrace(markers_[i].name_);
      markers_[i].trace_->Start();
    }
  }
  return true;
}

bool GPUTracerImpl::EndDecoding() {
  if (!gpu_executing_)
    return false;

  // End Trace for all active markers
  if (IsTracing()) {
    for (size_t i = 0; i < markers_.size(); i++) {
      if (markers_[i].trace_) {
        markers_[i].trace_->End();
        if (markers_[i].trace_->IsProcessable())
          traces_.push_back(markers_[i].trace_);
        markers_[i].trace_ = 0;
      }
    }
    IssueProcessTask();
  }

  gpu_executing_ = false;
  return true;
}

bool GPUTracerImpl::Begin(const std::string& name, GpuTracerSource source) {
  if (!gpu_executing_)
    return false;

  // Push new marker from given 'source'
  markers_.push_back(TraceMarker(name, source));

  // Create trace
  if (IsTracing()) {
    scoped_refptr<Trace> trace = CreateTrace(name);
    trace->Start();
    markers_.back().trace_ = trace;
  }
  return true;
}

bool GPUTracerImpl::End(GpuTracerSource source) {
  if (!gpu_executing_)
    return false;

  // Pop last marker with matching 'source'
  for (int i = markers_.size() - 1; i >= 0; i--) {
    if (markers_[i].source_ == source) {
      // End trace
      if (IsTracing()) {
        scoped_refptr<Trace> trace = markers_[i].trace_;
        if (trace) {
          trace->End();
          if (trace->IsProcessable())
            traces_.push_back(trace);
          IssueProcessTask();
        }
      }

      markers_.erase(markers_.begin() + i);
      return true;
    }
  }
  return false;
}

void GPUTracerImpl::Process() {
  process_posted_ = false;
  ProcessTraces();
  IssueProcessTask();
}

void GPUTracerImpl::ProcessTraces() {
  while (!traces_.empty() && traces_.front()->IsAvailable()) {
    traces_.front()->Process();
    traces_.pop_front();
  }
}

const std::string& GPUTracerImpl::CurrentName() const {
  if (markers_.empty())
    return base::EmptyString();
  return markers_.back().name_;
}

scoped_refptr<Trace> GPUTracerImpl::CreateTrace(const std::string& name) {
  return new NoopTrace(name);
}

void GPUTracerImpl::IssueProcessTask() {
  if (traces_.empty() || process_posted_)
    return;

  process_posted_ = true;
  base::MessageLoop::current()->PostDelayedTask(
      FROM_HERE,
      base::Bind(&GPUTracerImpl::Process, base::AsWeakPtr(this)),
      base::TimeDelta::FromMilliseconds(kProcessInterval));
}

GPUTracerARBTimerQuery::GPUTracerARBTimerQuery(gles2::GLES2Decoder* decoder)
    : timer_offset_(0), decoder_(decoder) {
  outputter_ = TraceOutputter::Create("GL_ARB_timer_query");
}

GPUTracerARBTimerQuery::~GPUTracerARBTimerQuery() {
}

scoped_refptr<Trace> GPUTracerARBTimerQuery::CreateTrace(
    const std::string& name) {
  if (*gpu_trace_dev_category)
    return new GLARBTimerTrace(outputter_, name, timer_offset_);
  return GPUTracerImpl::CreateTrace(name);
}

bool GPUTracerARBTimerQuery::BeginDecoding() {
  if (*gpu_trace_dev_category) {
    // Make sure timing is synced before tracing
    if (!gpu_timing_synced_) {
      CalculateTimerOffset();
      gpu_timing_synced_ = true;
    }
  } else {
    // If GPU device category is off, invalidate timing sync
    gpu_timing_synced_ = false;
  }

  return GPUTracerImpl::BeginDecoding();
}

bool GPUTracerARBTimerQuery::EndDecoding() {
  bool ret = GPUTracerImpl::EndDecoding();

  // NOTE(vmiura_: glFlush() here can help give better trace results,
  // but it distorts the normal device behavior.
  return ret;
}

void GPUTracerARBTimerQuery::ProcessTraces() {
  TRACE_EVENT0("gpu", "GPUTracerARBTimerQuery::ProcessTraces");

  // Make owning decoder's GL context current
  if (!decoder_->MakeCurrent()) {
    // Skip subsequent GL calls if MakeCurrent fails
    traces_.clear();
    return;
  }

  while (!traces_.empty() && traces_.front()->IsAvailable()) {
    traces_.front()->Process();
    traces_.pop_front();
  }

  // Clear pending traces if there were are any errors
  GLenum err = glGetError();
  if (err != GL_NO_ERROR)
    traces_.clear();
}

void GPUTracerARBTimerQuery::CalculateTimerOffset() {
  TRACE_EVENT0("gpu", "GPUTracerARBTimerQuery::CalculateTimerOffset");

  // NOTE(vmiura): It would be better to use glGetInteger64v, however
  // it's not available everywhere.
  GLuint64 gl_now = 0;
  GLuint query;
  glFinish();
  glGenQueries(1, &query);
  glQueryCounter(query, GL_TIMESTAMP);
  glFinish();
  glGetQueryObjectui64v(query, GL_QUERY_RESULT, &gl_now);
  base::TimeTicks system_now = base::TimeTicks::NowFromSystemTraceTime();

  gl_now /= base::Time::kNanosecondsPerMicrosecond;
  timer_offset_ = system_now.ToInternalValue() - gl_now;
  glDeleteQueries(1, &query);
}

GPUTracer::GPUTracer() {}

GPUTracer::~GPUTracer() {}

scoped_ptr<GPUTracer> GPUTracer::Create(gles2::GLES2Decoder* decoder) {
  if (gfx::g_driver_gl.ext.b_GL_ARB_timer_query) {
    return scoped_ptr<GPUTracer>(new GPUTracerARBTimerQuery(decoder));
  }
  return scoped_ptr<GPUTracer>(new GPUTracerImpl());
}

}  // namespace gles2
}  // namespace gpu
