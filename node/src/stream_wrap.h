#ifndef SRC_STREAM_WRAP_H_
#define SRC_STREAM_WRAP_H_

#include "stream_base.h"

#include "env.h"
#include "handle_wrap.h"
#include "string_bytes.h"
#include "v8.h"

namespace node {

// Forward declaration
class StreamWrap;

class StreamWrap : public HandleWrap, public StreamBase {
 public:
  static void Initialize(v8::Local<v8::Object> target,
                         v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context);

  int GetFD() override;
  void* Cast() override;
  bool IsAlive() override;
  bool IsClosing() override;
  bool IsIPCPipe() override;

  // JavaScript functions
  int ReadStart() override;
  int ReadStop() override;

  // Resource implementation
  int DoShutdown(ShutdownWrap* req_wrap) override;
  int DoTryWrite(uv_buf_t** bufs, size_t* count) override;
  int DoWrite(WriteWrap* w,
              uv_buf_t* bufs,
              size_t count,
              uv_stream_t* send_handle) override;

  inline uv_stream_t* stream() const {
    return stream_;
  }

  inline bool is_named_pipe() const {
    return stream()->type == UV_NAMED_PIPE;
  }

  inline bool is_named_pipe_ipc() const {
    return is_named_pipe() &&
           reinterpret_cast<const uv_pipe_t*>(stream())->ipc != 0;
  }

  inline bool is_tcp() const {
    return stream()->type == UV_TCP;
  }

 protected:
  StreamWrap(Environment* env,
             v8::Local<v8::Object> object,
             uv_stream_t* stream,
             AsyncWrap::ProviderType provider,
             AsyncWrap* parent = nullptr);

  ~StreamWrap() {
  }

  AsyncWrap* GetAsyncWrap() override;
  void UpdateWriteQueueSize();

  static void AddMethods(Environment* env,
                         v8::Local<v8::FunctionTemplate> target,
                         int flags = StreamBase::kFlagNone);

 private:
  static void SetBlocking(const v8::FunctionCallbackInfo<v8::Value>& args);

  // Callbacks for libuv
  static void OnAlloc(uv_handle_t* handle,
                      size_t suggested_size,
                      uv_buf_t* buf);

  static void OnRead(uv_stream_t* handle,
                     ssize_t nread,
                     const uv_buf_t* buf);
  static void OnReadCommon(uv_stream_t* handle,
                           ssize_t nread,
                           const uv_buf_t* buf,
                           uv_handle_type pending);
  static void AfterWrite(uv_write_t* req, int status);
  static void AfterShutdown(uv_shutdown_t* req, int status);

  // Resource interface implementation
  static void OnAfterWriteImpl(WriteWrap* w, void* ctx);
  static void OnAllocImpl(size_t size, uv_buf_t* buf, void* ctx);
  static void OnReadImpl(ssize_t nread,
                         const uv_buf_t* buf,
                         uv_handle_type pending,
                         void* ctx);

  uv_stream_t* const stream_;
};


}  // namespace node


#endif  // SRC_STREAM_WRAP_H_
