#ifndef SRC_HANDLE_WRAP_H_
#define SRC_HANDLE_WRAP_H_

#include "async-wrap.h"
#include "util.h"
#include "uv.h"
#include "v8.h"

namespace node {

class Environment;

// Rules:
//
// - Do not throw from handle methods. Set errno.
//
// - MakeCallback may only be made directly off the event loop.
//   That is there can be no JavaScript stack frames underneath it.
//   (Is there any way to assert that?)
//
// - No use of v8::WeakReferenceCallback. The close callback signifies that
//   we're done with a handle - external resources can be freed.
//
// - Reusable?
//
// - The uv_close_cb is used to free the c++ object. The close callback
//   is not made into javascript land.
//
// - uv_ref, uv_unref counts are managed at this layer to avoid needless
//   js/c++ boundary crossing. At the javascript layer that should all be
//   taken care of.

class HandleWrap : public AsyncWrap {
 public:
  static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Ref(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void Unref(const v8::FunctionCallbackInfo<v8::Value>& args);

  static inline bool IsAlive(const HandleWrap* wrap) {
    return wrap != nullptr && wrap->GetHandle() != nullptr;
  }

  inline uv_handle_t* GetHandle() const { return handle__; }

 protected:
  HandleWrap(Environment* env,
             v8::Local<v8::Object> object,
             uv_handle_t* handle,
             AsyncWrap::ProviderType provider,
             AsyncWrap* parent = nullptr);
  virtual ~HandleWrap() override;

 private:
  friend class Environment;
  friend void GetActiveHandles(const v8::FunctionCallbackInfo<v8::Value>&);
  static void OnClose(uv_handle_t* handle);
  ListNode<HandleWrap> handle_wrap_queue_;
  unsigned int flags_;
  // Using double underscore due to handle_ member in tcp_wrap. Probably
  // tcp_wrap should rename it's member to 'handle'.
  uv_handle_t* handle__;

  static const unsigned int kUnref = 1;
  static const unsigned int kCloseCallback = 2;
};


}  // namespace node


#endif  // SRC_HANDLE_WRAP_H_
