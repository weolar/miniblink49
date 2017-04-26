// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_FUNCTION_TEMPLATE_H_
#define GIN_FUNCTION_TEMPLATE_H_

#include "cef/include/base/cef_callback.h"
#include "base/logging.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/gin_export.h"
#include "v8.h"

namespace gin {

class PerIsolateData;

enum CreateFunctionTemplateFlags {
  HolderIsFirstArgument = 1 << 0,
};

namespace internal {

template<typename T>
struct CallbackParamTraits {
  typedef T LocalType;
};
template<typename T>
struct CallbackParamTraits<const T&> {
  typedef T LocalType;
};
template<typename T>
struct CallbackParamTraits<const T*> {
  typedef T* LocalType;
};


// CallbackHolder and CallbackHolderBase are used to pass a base::Callback from
// CreateFunctionTemplate through v8 (via v8::FunctionTemplate) to
// DispatchToCallback, where it is invoked.

// This simple base class is used so that we can share a single object template
// among every CallbackHolder instance.
class GIN_EXPORT CallbackHolderBase {
 public:
  v8::Local<v8::External> GetHandle(v8::Isolate* isolate);

 protected:
  explicit CallbackHolderBase(v8::Isolate* isolate);
  virtual ~CallbackHolderBase();

 private:
  static void FirstWeakCallback(
      const v8::WeakCallbackInfo<CallbackHolderBase>& data);
  static void SecondWeakCallback(
      const v8::WeakCallbackInfo<CallbackHolderBase>& data);

  v8::Global<v8::External> v8_ref_;

  DISALLOW_COPY_AND_ASSIGN(CallbackHolderBase);
};

template<typename Sig>
class CallbackHolder : public CallbackHolderBase {
 public:
  CallbackHolder(v8::Isolate* isolate,
                 const base::Callback<Sig>& callback,
                 int flags)
      : CallbackHolderBase(isolate), callback(callback), flags(flags) {}
  base::Callback<Sig> callback;
  int flags;
 private:
  virtual ~CallbackHolder() {}

  DISALLOW_COPY_AND_ASSIGN(CallbackHolder);
};

template<typename ClassType, typename Type>
class CallbackHolderGetSet : public CallbackHolderBase {
public:
    CallbackHolderGetSet(v8::Isolate* isolate, const base::Callback<Type(ClassType)>& get_callback, const base::Callback<void(ClassType, Type)>& set_callback)
        : CallbackHolderBase(isolate), get_callback_(get_callback), set_callback_(set_callback) {
    }
    base::Callback<Type(ClassType)> get_callback_;
    base::Callback<void(ClassType, Type)> set_callback_;
private:
    virtual ~CallbackHolderGetSet() {}

    DISALLOW_COPY_AND_ASSIGN(CallbackHolderGetSet);
};

template<typename T>
bool GetNextArgument(Arguments* args, int create_flags, bool is_first,
                     T* result) {
  bool b = false;
  if (is_first && (create_flags & HolderIsFirstArgument) != 0) {
    b = args->GetHolder(result);
  } else {
    b = args->GetNext(result);
  }
  if (!b)
      OutputDebugStringA("GetNextArgument failed!\n");
  return b;
}

// For advanced use cases, we allow callers to request the unparsed Arguments
// object and poke around in it directly.
inline bool GetNextArgument(Arguments* args, int create_flags, bool is_first,
                            Arguments* result) {
  *result = *args;
  return true;
}
inline bool GetNextArgument(Arguments* args, int create_flags, bool is_first,
                            Arguments** result) {
  *result = args;
  return true;
}

// It's common for clients to just need the isolate, so we make that easy.
inline bool GetNextArgument(Arguments* args, int create_flags,
                            bool is_first, v8::Isolate** result) {
  *result = args->isolate();
  return true;
}

// Classes for generating and storing an argument pack of integer indices
// (based on well-known "indices trick", see: http://goo.gl/bKKojn):
template <size_t... indices>
struct IndicesHolder {};

template <size_t requested_index, size_t... indices>
struct IndicesGenerator {
  using type = typename IndicesGenerator<requested_index - 1,
                                         requested_index - 1,
                                         indices...>::type;
};
template <size_t... indices>
struct IndicesGenerator<0, indices...> {
  using type = IndicesHolder<indices...>;
};

// Class template for extracting and storing single argument for callback
// at position |index|.
template <size_t index, typename ArgType>
struct ArgumentHolder {
  using ArgLocalType = typename CallbackParamTraits<ArgType>::LocalType;

  ArgLocalType value;
  bool ok;

  ArgumentHolder(Arguments* args, int create_flags)
      : ok(GetNextArgument(args, create_flags, index == 0, &value)) {
    if (!ok) {
      // Ideally we would include the expected c++ type in the error
      // message which we can access via typeid(ArgType).name()
      // however we compile with no-rtti, which disables typeid.
      args->ThrowError();
    }
  }
};

template <>
struct ArgumentHolder<1, const v8::FunctionCallbackInfo<v8::Value>&> {
    using ArgLocalType = const v8::FunctionCallbackInfo<v8::Value>&;
    ArgLocalType value;
    bool ok;

    ArgumentHolder(Arguments* args, int create_flags)
        : value(*args->getInfo())
        , ok(true) {

    }
};

// Class template for converting arguments from JavaScript to C++ and running
// the callback with them.
template <typename IndicesType, typename... ArgTypes>
class Invoker {};

template <size_t... indices, typename... ArgTypes>
class Invoker<IndicesHolder<indices...>, ArgTypes...>
    : public ArgumentHolder<indices, ArgTypes>... {
 public:
  // Invoker<> inherits from ArgumentHolder<> for each argument.
  // C++ has always been strict about the class initialization order,
  // so it is guaranteed ArgumentHolders will be initialized (and thus, will
  // extract arguments from Arguments) in the right order.
  Invoker(Arguments* args, int create_flags)
      : ArgumentHolder<indices, ArgTypes>(args, create_flags)..., args_(args) {
    // GCC thinks that create_flags is going unused, even though the
    // expansion above clearly makes use of it. Per jyasskin@, casting
    // to void is the commonly accepted way to convince the compiler
    // that you're actually using a parameter/varible.
    (void)create_flags;
  }

  bool IsOK() {
    return And(ArgumentHolder<indices, ArgTypes>::ok...);
  }

  template <typename ReturnType>
  void DispatchToCallback(base::Callback<ReturnType(ArgTypes...)> callback) {
    args_->Return(callback.Run(ArgumentHolder<indices, ArgTypes>::value...));
  }

  // In C++, you can declare the function foo(void), but you can't pass a void
  // expression to foo. As a result, we must specialize the case of Callbacks
  // that have the void return type.
  void DispatchToCallback(base::Callback<void(ArgTypes...)> callback) {
    callback.Run(ArgumentHolder<indices, ArgTypes>::value...);
  }

 private:
  static bool And() { return true; }
  template <typename... T>
  static bool And(bool arg1, T... args) {
    return arg1 && And(args...);
  }

  Arguments* args_;
};

// DispatchToCallback converts all the JavaScript arguments to C++ types and
// invokes the base::Callback.
template <typename Sig>
struct Dispatcher {};

template <typename ReturnType, typename... ArgTypes>
struct Dispatcher<ReturnType(ArgTypes...)> {
  static void DispatchToCallback(
      const v8::FunctionCallbackInfo<v8::Value>& info) {
    Arguments args(info);
    v8::Local<v8::External> v8_holder;
    CHECK(args.GetData(&v8_holder));
    CallbackHolderBase* holder_base = reinterpret_cast<CallbackHolderBase*>(
        v8_holder->Value());

    typedef CallbackHolder<ReturnType(ArgTypes...)> HolderT;
    HolderT* holder = static_cast<HolderT*>(holder_base);

    using Indices = typename IndicesGenerator<sizeof...(ArgTypes)>::type;
    Invoker<Indices, ArgTypes...> invoker(&args, holder->flags);
    if (invoker.IsOK())
      invoker.DispatchToCallback(holder->callback);
  }
};

template<typename ClassType, typename Type>
struct DispatcherAccessor {
    static void DispatchToCallbackGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Isolate* isolate = info.GetIsolate();

        v8::Local<v8::External> v8_holder;
        if (!ConvertFromV8(isolate, info.Data(), &v8_holder))
            return;

        CallbackHolderBase* holder_base = reinterpret_cast<CallbackHolderBase*>(v8_holder->Value());
        typedef CallbackHolderGetSet<ClassType, Type> HolderGetSetT;
        HolderGetSetT* holder = static_cast<HolderGetSetT*>(holder_base);

        ClassType obj;
        if (!ConvertFromV8(isolate, info.Holder(), &obj))
            return;

        Type result = holder->get_callback_.Run(obj);
        v8::Local<v8::Value> v8_result_value;
        if (!TryConvertToV8(isolate, result, &v8_result_value))
            return;
        info.GetReturnValue().Set(v8_result_value);
    }

    static void DispatchToCallbackSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
        v8::Isolate* isolate = info.GetIsolate();

        v8::Local<v8::External> v8_holder;
        if (!ConvertFromV8(isolate, info.Data(), &v8_holder))
            return;

        CallbackHolderBase* holder_base = reinterpret_cast<CallbackHolderBase*>(v8_holder->Value());
        typedef CallbackHolderGetSet<ClassType, Type> HolderGetSetT;
        HolderGetSetT* holder = static_cast<HolderGetSetT*>(holder_base);

        ClassType obj;
        if (ConvertFromV8(isolate, info.Holder(), &obj))
            return;

        Type arg;
        if (ConvertFromV8(isolate, value, &arg))
            return;

        holder->set_callback_.Run(obj, arg);
    }
};

}  // namespace internal


// CreateFunctionTemplate creates a v8::FunctionTemplate that will create
// JavaScript functions that execute a provided C++ function or base::Callback.
// JavaScript arguments are automatically converted via gin::Converter, as is
// the return value of the C++ function, if any.
//
// NOTE: V8 caches FunctionTemplates for a lifetime of a web page for its own
// internal reasons, thus it is generally a good idea to cache the template
// returned by this function.  Otherwise, repeated method invocations from JS
// will create substantial memory leaks. See http://crbug.com/463487.
template<typename Sig>
v8::Local<v8::FunctionTemplate> CreateFunctionTemplate(
    v8::Isolate* isolate, const base::Callback<Sig> callback,
    int callback_flags = 0) {
  typedef internal::CallbackHolder<Sig> HolderT;
  HolderT* holder = new HolderT(isolate, callback, callback_flags);

  return v8::FunctionTemplate::New(
      isolate,
      &internal::Dispatcher<Sig>::DispatchToCallback,
      ConvertToV8<v8::Local<v8::External> >(isolate,
                                             holder->GetHandle(isolate)));
}

template<typename ClassType, typename Type>
void SetMemberGetSetAccessor(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> obj_template, v8::Local<v8::String> name,
    const base::Callback<Type(ClassType)> get_callback,
    const base::Callback<void(ClassType, Type)> set_callback) {
    typedef internal::CallbackHolderGetSet<ClassType, Type> HolderGetSetT;
    HolderGetSetT* holder = new HolderGetSetT(isolate, get_callback, set_callback);

    obj_template->SetAccessor(name,
        &internal::DispatcherAccessor<ClassType, Type>::DispatchToCallbackGetter,
        &internal::DispatcherAccessor<ClassType, Type>::DispatchToCallbackSetter,
        ConvertToV8<v8::Local<v8::External> >(isolate, holder->GetHandle(isolate)));
}

// CreateFunctionHandler installs a CallAsFunction handler on the given
// object template that forwards to a provided C++ function or base::Callback.
template<typename Sig>
void CreateFunctionHandler(v8::Isolate* isolate,
                           v8::Local<v8::ObjectTemplate> tmpl,
                           const base::Callback<Sig> callback,
                           int callback_flags = 0) {
  typedef internal::CallbackHolder<Sig> HolderT;
  HolderT* holder = new HolderT(isolate, callback, callback_flags);
  tmpl->SetCallAsFunctionHandler(&internal::Dispatcher<Sig>::DispatchToCallback,
                                 ConvertToV8<v8::Local<v8::External> >(
                                     isolate, holder->GetHandle(isolate)));
}

}  // namespace gin

#endif  // GIN_FUNCTION_TEMPLATE_H_
